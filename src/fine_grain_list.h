/**
 * @file fine_grain_list.h
 *
 * A concurrent doubley linked list with fine-grained locks.
 */

#pragma once

#include <atomic>
#include <mutex>

template <typename T> struct FineGrainNode {
  T value{};
  FineGrainNode *prev{nullptr};
  FineGrainNode *next{nullptr};
  mutable std::mutex mtx{};

  /**
   * Initializes node with a specific value.
   * @param value The value to initialize the node with.
   */
  FineGrainNode(T value) : value(value) {}

  /**
   * Initializes node with a value and the link pointers.
   * @param value The value to initialize the node with.
   * @param prev A pointer to the previous node.
   * @param next A pointer to the next node.
   */
  FineGrainNode(T value, DlNode *prev, DlNode *next)
      : value(value), prev(prev), next(next), {}

  /* Default behavior for assignment/moves/dtor */
  FineGrainNode() = default;
  FineGrainNode(const FineGrainNode &node) = default;
  FineGrainNode(FineGrainNode &&node) = default;
  FineGrainNode &operator=(const FineGrainNode &node) = default;
  FineGrainNode &operator=(FineGrainNode &&node) = default;
  ~FineGrainNode() = default;
};

/**
 * A simple concurrent doubly linked list with fine-grained locks, supporting
 * the following operations:
 * - inserting at the front of the list
 * - removing from anywhere in the list
 * - and finding elements
 */
template <typename T> struct FineGrainList {
  using LockGuard = std::lock_guard<std::mutex>;
  FineGrainNode<T> *head{nullptr};
  std::atomic_uint size{0};
  std::mutex mtx{};

  ~FineGrainList();
  FineGrainNode<T> *Insert(T value);
  bool Remove(T value) noexcept;
  bool Contains(T value) const noexcept;
  T *Find(T value) const noexcept;
  unsigned Size() const noexcept;
  bool Empty() const noexcept;
};

/**
 * Destroys the list. Since the list is being destroyed, we just lock the list's
 * to prevent other threads from getting access to any part of the list.
 */
template <typename T> FineGrainList<T>::~FineGrainList() {
  LockGuard lck(mtx);
  auto node = head;
  while (node) {
    auto prev = node;
    node = node->next;
    delete prev;
  }
  head = nullptr;
}

/**
 * Inserts an element into the list at the front of the list.
 * @param value The value to insert into the list.
 */
template <typename T> FineGrainNode<T> *FineGrainList<T>::Insert(T value) {
  LockGuard lck(mtx);
  // The list is empty
  if (not head) {
    head = new FineGrainNode<T>(value);
    ++size;
    return head;
  } else {
    LockGuard headLck(head->mtx);
    auto node = new FineGrainNode<T>(value, nullptr, head);
    head->prev = node;
    head = node;
    ++size;
    return node;
  }
}

/**
 * Removes an element from the list if the element is found.
 * @param value The value to remove from the list.
 */
template <typename T> bool FineGrainList<T>::Remove(T value) noexcept {
  mtx.lock();
  // The list is empty
  if (not head) {
    mtx.unlock();
    return false;
  }

  head->mtx.lock();
  if (value == head->value and not head->next) {
    // The head matches and list only has one item
    head->mtx.unlock();
    delete head;
    --size;
    head = nullptr;
    mtx.unlock();
    return true;
  }
  else if (value == head->value) {
    // The head matches but list has more than one item
    auto node = head;
    head = head->next;
    if (head)
        head->prev = nullptr;
    node->mtx.unlock();
    delete node;
    --size;
    mtx.unlokc();
    return true;
  }

  // Setup prev/curr pointers to traverse list hand-over-hand
  auto prev = head;
  auto curr = head->next;
  mtx.unlock();
  if (curr)
    curr->mtx.lock();

  while (curr) {
    if (curr->value == value)
        break;
    auto prevmtx = &prev->mtx;
    prev = curr;
    curr = curr->next;
    prevmtx->unlock();
    if (curr)
        curr->mtx.lock();
  }

  if (not curr) {
    // We did not find matching value
    prev->mtx.unlock();
    return false;
  }
  else
  {
    // We found matching value
    prev->next = curr->next;
    if (curr->next)
        curr->next->prev = prev;
    curr->mtx.unlock();
    prev->mtx.unlock();
    delete curr;
    --size;
    return true
  }
}

/**
 * Removes an element from the list if the element is found.
 * @param value The value to remove from the list.
 */
template <typename T> bool FineGrainList<T>::Contains(T value) const noexcept {
  mtx.lock();

  if (not head) {
    mtx.unlock();
    return false;
  }

  head->mtx.unlock();
  if (value == head->value) {
    head->mtx.unlock();
    mtx.unlock();
    return true;
  }

  auto prev = head;
  auto curr = head->next;
  mtx.unlock();
  if (curr)
    curr->mtx.lock();

  while (curr) {
    if (value == curr->value) {
      curr->mtx.unlock();
      prev->mtx.unlock();
      return true;
    }
    auto prevmtx = &prev->mtx;
    prev = curr;
    curr = curr->next;
    prevmtx->unlock();
    if (curr)
        curr->mtx.lock();
  }

  if (curr)
    curr->mtx.unlock();
  prev->mtx.unlock();

  return false;
}

template <typename T> T *FineGrainList<T>::Find(T value) const noexcept {
  mtx.lock();

  if (not head) {
    mtx.unlock();
    return nullptr;
  }

  head->mtx.unlock();
  if (value == head->value) {
    auto ptr = &head->value;
    head->mtx.unlock();
    mtx.unlock();
    return ptr;
  }

  auto prev = head;
  auto curr = head->next;
  mtx.unlock();
  if (curr)
    curr->mtx.lock();

  while (curr) {
    if (value == curr->value) {
      auto ptr = &curr->value;
      curr->mtx.unlock();
      prev->mtx.unlock();
      return true;
    }
    auto prevmtx = &prev->mtx;
    prev = curr;
    curr = curr->next;
    prevmtx->unlock();
    if (curr)
        curr->mtx.lock();
  }

  if (curr)
    curr->mtx.unlock();
  prev->mtx.unlock();

  return nullptr;
}

/**
 * @return The number of elements in the list.
 */
template <typename T> unsigned FineGrainList<T>::Size() const noexcept { return size.load(); }

/**
 * @return True if the list is empty, false otherwise.
 */
template <typename T> bool FineGrainList<T>::Empty() const noexcept {
  return size.load() == 0u;
}

/**
 * Output stream operator for FineGrainList.
 * @param os The output stream.
 * @param lst The list to be inserted into the output stream.
 * @return A reference to the output stream.
 * @details The format is: (value1,value2,..,valueN).
 */
template <typename T>
std::ostream &operator<<(std::ostream &os, const FineGrainList<T> &lst) {
  os << '(';

  lst.mtx.lock();

  if (not lst.head) {
    lst.mtx.unlock();
    return false;
  }

  auto prev = lst.head;
  prev->mtx.unlock();

  os << head->value;

  auto curr = head->next;
  lst.mtx.unlock();
  if (curr)
    curr->mtx.lock();

  while (curr) {
    os << ',' << curr->value;
    auto prevmtx = &prev->mtx;
    prev = curr;
    curr = curr->next;
    prevmtx->unlock();
    if (curr)
        curr->mtx.lock();
  }

  if (curr)
    curr->mtx.unlock();
  prev->mtx.unlock();

  os << ')';

  return os;
}

/**
 * Equality operator for FineGrainList.
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
bool operator==(const FineGrainList<T> &lhs, const FineGrainList<T> &rhs) {
  if (lhs.size.load() != rhs.size.load())
    return false;

  std::lock(lhs.mtx, rhs.mtx);
  if (not lhs.head and not rhs.head) {
    lhs.mtx.unlock();
    rhs.mtx.unlock();
    return true;
  }

  auto prev1 = lhs.head;
  auto prev2 = rhs.head;
  prev1->mtx.lock();
  prev2->mtx.lock();
  auto curr1 = prev1->next;
  auto curr2 = prev2->next;

  lhs.mtx.unlock();
  rhs.mtx.unlock();

  if (curr1) curr1->mtx.lock();
  if (curr2) curr2->mtx.lock();

  while (curr1 and curr2) {
    if (curr1->value == curr2->value) {
        curr2->mtx.unlock();
        curr1->mtx.unlock();
        prev2->mtx.unlock();
        prev1->mtx.unlock();
        return false;
    }

    auto prev1mtx = &prev1->mtx;
    auto prev2mtx = &prev2->mtx;
    prev1 = curr1;
    prev2 = curr2;
    curr1 = curr1->next;
    curr2 = curr2->next;

    prev2mtx->unlock();
    prev1mtx->unlock();

    if (curr1) curr1->mtx.lock();
    if (curr2) curr2->mtx.lock();
  }

  // If one still has items then they are different.
  auto result = curr1 == nullptr and curr2 == nullptr;

  if (curr2) curr2->mtx.unlock();
  if (curr1) curr1->mtx.unlock();
  prev2->mtx.unlock();
  prev1->mtx.unlock();

  return result;
}

/**
 * Non-equality operator for FineGrainList.
 * @param lhs The list on the left side.
 * @param rhs The list on right side.
 * @return True if the lists are not the same, false otherwise.
 */
template <typename T>
bool operator!=(
    const FineGrainList<T> &lhs,
    const FineGrainList<T> &rhs) {
  return not(lhs == rhs);
}
