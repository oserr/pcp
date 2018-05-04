/**
 * @file nonblocking_list.h
 *
 * A lock-free concurrent doubley linked list.
 */

#pragma once

#include <atomic>
#include <mutex>

/**
 * A reader-writer non-blocking lock.
 */
struct RwLock {
  std::atomic_int counter{0};

  void ReadLock() noexcept {
    int val, old;
    do {
      while ((val = counter.load()) < 0)
        ;
      old = val++;
    } while (not counter.compare_exchange_weak(old, val));
  }

  void ReadUnlock() noexcept { --counter; }

  void WriteLock() noexcept {
    int val, old;
    do {
      while ((val = counter.load()) != 0)
        ;
      old = val--;
    } while (not counter.compare_exchange_weak(old, val));
  }

  void WriteUnlock() noexcept { ++counter; }
};

template <typename T> struct NonBlockingNode {
  T value{};
  NonBlockingNode *prev{nullptr};
  NonBlockingNode *next{nullptr};
  mutable RwLock lck{};

  /**
   * Initializes node with a specific value.
   * @param value The value to initialize the node with.
   */
  NonBlockingNode(T value) : value(value) {}

  /**
   * Initializes node with a value and the link pointers.
   * @param value The value to initialize the node with.
   * @param prev A pointer to the previous node.
   * @param next A pointer to the next node.
   */
  NonBlockingNode(T value, NonBlockingNode *prev, NonBlockingNode *next)
      : value(value), prev(prev), next(next) {}

  /* Default behavior for assignment/moves/dtor */
  NonBlockingNode() = default;
  NonBlockingNode(const NonBlockingNode &node) = default;
  NonBlockingNode(NonBlockingNode &&node) = default;
  NonBlockingNode &operator=(const NonBlockingNode &node) = default;
  NonBlockingNode &operator=(NonBlockingNode &&node) = default;
  ~NonBlockingNode() = default;
};

/**
 * A simple non-blocking concurrent doubly linked list, supporting the following
 * operations:
 * - inserting at the front of the list
 * - removing from anywhere in the list
 * - and finding elements
 */
template <typename T> struct NonBlockingList {
  using NodeType = NonBlockingNode<T>;
  NonBlockingNode<T> *head{nullptr};
  std::atomic_uint size{0};
  mutable RwLock lck{};

  ~NonBlockingList();
  NonBlockingNode<T> *Insert(T value);
  bool InsertUnique(T value);
  bool Remove(T value) noexcept;
  bool Contains(T value) const noexcept;
  bool Find(T &value) const noexcept;
  unsigned Size() const noexcept;
  bool Empty() const noexcept;
};

/**
 * Destroys the list. Since the list is being destroyed, we just lock the whole
 * list to prevent other threads from getting access to any part of the list.
 */
template <typename T> NonBlockingList<T>::~NonBlockingList() {
  lck.WriteLock();
  auto node = head;
  while (node) {
    auto prev = node;
    node = node->next;
    delete prev;
  }
  head = nullptr;
  lck.WriteUnlock();
}

/**
 * Inserts an element into the list at the front of the list.
 * @param value The value to insert into the list.
 */
template <typename T> NonBlockingNode<T> *NonBlockingList<T>::Insert(T value) {
  NodeType *node;
  lck.WriteLock();
  // The list is empty
  if (not head) {
    try {
      head = new NodeType(value);
    } catch (...) {
      lck.WriteUnlock();
      throw;
    }
    ++size;
    node = head;
    lck.WriteUnlock();
    return node;
  } else {
    head->lck.WriteLock();
    try {
      node = new NodeType(value, nullptr, head);
    } catch (...) {
      head->lck.WriteUnlock();
      lck.WriteUnlock();
      throw;
    }
    head->prev = node;
    head = node;
    ++size;
    head->next->lck.WriteUnlock();
    lck.WriteUnlock();
    return node;
  }
}

/**
 * Inserts an item only if the list does not cotain the item.
 * @param value The value to insert into the list.
 * @return True if the value was inserted, false otherwise.
 */
template <typename T> bool NonBlockingList<T>::InsertUnique(T value) {
  lck.WriteLock();

  if (not head) {
    try {
      head = new NodeType(value);
    } catch (...) {
      lck.WriteUnlock();
      return false;
    }
    ++size;
    lck.WriteUnlock();
    return true;
  }

  auto prev = head;
  prev->lck.WriteLock();
  auto curr = prev->next;
  lck.WriteUnlock();

  while (curr) {
    curr->lck.WriteLock();
    if (prev->value == value) {
      curr->lck.WriteUnlock();
      prev->lck.WriteUnlock();
      return false;
    }
    auto prevlck = &prev->lck;
    prev = curr;
    curr = curr->next;
    prevlck->WriteUnlock();
  }

  if (prev->value == value) {
    prev->lck.WriteUnlock();
    return false;
  }

  try {
    prev->next = new NodeType(std::move(value), prev, nullptr);
  } catch (...) {
    prev->lck.WriteUnlock();
    return false;
  }

  prev->lck.WriteUnlock();
  ++size;
  return true;
}

/**
 * Removes an element from the list if the element is found.
 * @param value The value to remove from the list.
 */
template <typename T> bool NonBlockingList<T>::Remove(T value) noexcept {
  lck.WriteLock();
  // The list is empty
  if (not head) {
    lck.WriteUnlock();
    return false;
  }

  head->lck.WriteLock();
  if (value == head->value and not head->next) {
    // The head matches and list only has one item
    auto node = head;
    head = nullptr;
    lck.WriteUnlock();
    --size;
    node->lck.WriteUnlock();
    delete node;
    return true;
  } else if (value == head->value) {
    // The head matches but list has more than one item
    auto node = head;
    head = head->next;
    if (head)
      head->prev = nullptr;
    lck.WriteUnlock();
    --size;
    node->lck.WriteUnlock();
    delete node;
    return true;
  }

  // Setup prev/curr pointers to traverse list hand-over-hand
  auto prev = head;
  auto curr = head->next;
  lck.WriteUnlock();

  while (curr) {
    curr->lck.WriteLock();
    if (curr->value == value)
      break;
    auto prevlck = &prev->lck;
    prev = curr;
    curr = curr->next;
    prevlck->WriteUnlock();
  }

  if (not curr) {
    // We did not find matching value
    prev->lck.WriteUnlock();
    return false;
  } else {
    // We found matching value
    prev->next = curr->next;
    if (curr->next)
      curr->next->prev = prev;
    curr->lck.WriteUnlock();
    prev->lck.WriteUnlock();
    --size;
    delete curr;
    return true;
  }
}

/**
 * Removes an element from the list if the element is found.
 * @param value The value to remove from the list.
 */
template <typename T>
bool NonBlockingList<T>::Contains(T value) const noexcept {
  lck.ReadLock();

  if (not head) {
    lck.ReadUnlock();
    return false;
  }

  head->lck.ReadLock();
  if (value == head->value) {
    head->lck.ReadUnlock();
    lck.ReadUnlock();
    return true;
  }

  auto prev = head;
  auto curr = head->next;
  lck.ReadUnlock();

  while (curr) {
    curr->lck.ReadLock();
    if (value == curr->value) {
      curr->lck.ReadUnlock();
      prev->lck.ReadUnlock();
      return true;
    }
    auto prevlck = &prev->lck;
    prev = curr;
    curr = curr->next;
    prevlck->ReadUnlock();
  }

  prev->lck.ReadUnlock();

  return false;
}

template <typename T> bool NonBlockingList<T>::Find(T &value) const noexcept {
  lck.ReadLock();

  if (not head) {
    lck.ReadUnlock();
    return false;
  }

  head->lck.ReadLock();
  if (value == head->value) {
    value = head->value;
    head->lck.ReadUnlock();
    lck.ReadUnlock();
    return true;
  }

  auto prev = head;
  auto curr = head->next;
  lck.ReadUnlock();

  while (curr) {
    curr->lck.ReadLock();
    if (value == curr->value) {
      value = head->value;
      curr->lck.ReadUnlock();
      prev->lck.ReadUnlock();
      return true;
    }
    auto prevlck = &prev->lck;
    prev = curr;
    curr = curr->next;
    prevlck->ReadUnlock();
  }

  prev->lck.ReadUnlock();

  return false;
}

/**
 * @return The number of elements in the list.
 */
template <typename T> unsigned NonBlockingList<T>::Size() const noexcept {
  return size.load();
}

/**
 * @return True if the list is empty, false otherwise.
 */
template <typename T> bool NonBlockingList<T>::Empty() const noexcept {
  return size.load() == 0u;
}

/**
 * Output stream operator for NonBlockingList.
 * @param os The output stream.
 * @param lst The list to be inserted into the output stream.
 * @return A reference to the output stream.
 * @details The format is: (value1,value2,..,valueN).
 */
template <typename T>
std::ostream &operator<<(std::ostream &os, const NonBlockingList<T> &lst) {
  os << '(';

  lst.lck.ReadLock();

  if (not lst.head) {
    lst.lck.ReadUnlock();
    return os << ')';
  }

  auto prev = lst.head;
  prev->lck.ReadLock();

  os << prev->value;

  auto curr = prev->next;
  lst.lck.ReadUnlock();

  while (curr) {
    curr->lck.ReadLock();
    os << ',' << curr->value;
    auto prevlck = &prev->lck;
    prev = curr;
    curr = curr->next;
    prevlck->ReadUnlock();
  }

  prev->lck.ReadUnlock();

  os << ')';

  return os;
}

/**
 * Equality operator for NonBlockingList.
 * @param lhs The list on the left side.
 * @param rhs The list on right side.
 * @return True if the lists have the same number of elements and all the
 *  elements are the same. Empty lists are considered equal.
 *
 * @details The result of the operation reflects what the function sees as it
 *  traverses the list, but is possible for the lists to change while the lists
 *  are being traversed. For example, as a given thread is running the equality
 *  operator, a second thread might replace one of the values in one of the
 *  nodes that have already been evaluated by the other thread. Thus, by the
 *  end, the thread doing the equality check might think that the lists are
 *  equal, even though it might not be true anymore.
 */
template <typename T>
bool operator==(const NonBlockingList<T> &lhs, const NonBlockingList<T> &rhs) {
  if (lhs.size.load() != rhs.size.load())
    return false;

  lhs.lck.ReadLock();
  rhs.lck.ReadLock();
  if (not lhs.head and not rhs.head) {
    lhs.lck.ReadUnlock();
    rhs.lck.ReadUnlock();
    return true;
  }

  auto prev1 = lhs.head;
  auto prev2 = rhs.head;
  prev1->lck.ReadLock();
  prev2->lck.ReadLock();
  auto curr1 = prev1->next;
  auto curr2 = prev2->next;

  lhs.lck.ReadUnlock();
  rhs.lck.ReadUnlock();

  while (curr1 and curr2) {
    curr1->lck.ReadLock();
    curr2->lck.ReadLock();

    if (curr1->value != curr2->value) {
      curr2->lck.ReadUnlock();
      curr1->lck.ReadUnlock();
      prev2->lck.ReadUnlock();
      prev1->lck.ReadUnlock();
      return false;
    }

    auto prev1lck = &prev1->lck;
    auto prev2lck = &prev2->lck;
    prev1 = curr1;
    prev2 = curr2;
    curr1 = curr1->next;
    curr2 = curr2->next;

    prev2lck->ReadUnlock();
    prev1lck->ReadUnlock();
  }

  prev2->lck.ReadUnlock();
  prev1->lck.ReadUnlock();

  // If one still has items then they are different.
  return curr1 == nullptr and curr2 == nullptr;
}

/**
 * Non-equality operator for NonBlockingList.
 * @param lhs The list on the left side.
 * @param rhs The list on right side.
 * @return True if the lists are not the same, false otherwise.
 */
template <typename T>
bool operator!=(const NonBlockingList<T> &lhs, const NonBlockingList<T> &rhs) {
  return not(lhs == rhs);
}
