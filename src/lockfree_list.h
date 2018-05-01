/**
 * @file lockfree_list.h
 *
 * A simple lockfree linked list.
 */

#pragma once

#include <atomic>
#include <mutex>
#include <ostream>

template <typename T> struct LockFreeNode {
  T value;
  std::atomic<LockFreeNode *> prev;
  std::atomic<LockFreeNode *> next;

  /**
   * Default ctor. Initalizes everything to default or nullptr.
   */
  LockFreeNode() : value(), prev(nullptr), next(nullptr) {}

  /**
   * Initializes node with specific value, and sets prev/next to nullptr.
   * @param value The value to initialize the node with.
   */
  LockFreeNode(T value) : value(value), prev(nullptr), next(nullptr) {}

  /**
   * Initializes node with a value, and with points to neighboring nodes.
   * @param value The value to initialize the node with.
   * @param prev A pointer to the previous node.
   * @param next A pointer to the next node.
   */
  LockFreeNode(T value, LockFreeNode *prev, LockFreeNode *next)
      : value(value), prev(prev), next(next) {}

  /* Default behavior for assignment/moves/dtor */
  LockFreeNode(const LockFreeNode &node) = default;
  LockFreeNode(LockFreeNode &&node) = default;
  LockFreeNode &operator=(const LockFreeNode &node) = default;
  LockFreeNode &operator=(LockFreeNode &&node) = default;
  ~LockFreeNode() = default;
};

/**
 * A simple lockfree single linked list with very basic operations:
 * - Insert(): inserts a value at the front of the list
 * - InsertUnique(): inserts a value only if it does not exists in the list
 * already
 * - Remove(): remove a value from the list
 * - Contains(): Checks if a value is in the list
 * - Size(): Returns the size of the list
 */
template <typename T> struct LockFreeList {
  LockFreeNode<T> *head;
  LockFreeNode<T> *tail;
  std::atomic_uint size{0};
  // for printing the list
  using LockGuard = std::lock_guard<std::mutex>;
  mutable std::mutex mtx;

  LockFreeList();
  ~LockFreeList();
  virtual bool Insert(T value);
  virtual bool InsertUnique(T value);
  virtual bool Remove(T value) noexcept;
  virtual bool Contains(T value) const noexcept;
  virtual unsigned Size() const noexcept;
  virtual bool Empty() const noexcept;

  LockFreeNode<T> *search(T value, LockFreeNode<T> **left_node) const;
  static bool is_marked(LockFreeNode<T> *);
  static LockFreeNode<T> *get_marked(LockFreeNode<T> *);
  static LockFreeNode<T> *get_unmarked(LockFreeNode<T> *);
  LockFreeNode<T> *get_next_unmarked(LockFreeNode<T> *) const;
};

template <typename T> bool LockFreeList<T>::is_marked(LockFreeNode<T> *addr) {
  return 0x1 & (long)addr;
}

template <typename T>
LockFreeNode<T> *LockFreeList<T>::get_marked(LockFreeNode<T> *addr) {
  return (LockFreeNode<T> *)((long)addr | 0x01);
}

template <typename T>
LockFreeNode<T> *LockFreeList<T>::get_unmarked(LockFreeNode<T> *addr) {
  return (LockFreeNode<T> *)((long)addr & ~0x01);
}

template <typename T>
LockFreeNode<T> *LockFreeList<T>::get_next_unmarked(LockFreeNode<T> *node) const {
  LockFreeNode<T> *next = get_unmarked(node->next);
  while(next != tail && is_marked(next->next)){
    next = get_unmarked(next->next);
  }
  return next;
}

template <typename T>
LockFreeNode<T> *LockFreeList<T>::search(T value,
                                         LockFreeNode<T> **left_node) const {
  LockFreeNode<T> *left_node_nxt, *right_node;
  while (1) {
    LockFreeNode<T> *node = head;
    LockFreeNode<T> *node_nxt = head->next;

    while (1) {
      if (!is_marked(node_nxt)) {
        (*left_node) = node;
        left_node_nxt = node_nxt;
      }
      // advance node
      node = get_unmarked(node_nxt);
      if (node == tail) {
        break;
      }
      node_nxt = node->next;
      if (!is_marked(node_nxt) && node->value == value) {
        break;
      }
    }
    right_node = node;

    if (left_node_nxt == right_node) {
      return right_node;
    } else {
      if (std::atomic_compare_exchange_weak(&(*left_node)->next,
                                            (LockFreeNode<T> **)&left_node_nxt,
                                            right_node)) {
        return right_node;
      }
    }
  }
}

/**
 * Initializes the list.
 */
template <typename T> LockFreeList<T>::LockFreeList() {
  head = new LockFreeNode<T>();
  tail = new LockFreeNode<T>();
  head->next = tail;
}

/**
 * Destroys the list.
 */
template <typename T> LockFreeList<T>::~LockFreeList() {
  auto node = head;
  while (node) {
    auto prev = node;
    node = get_unmarked(node->next);
    delete prev;
  }
}

/**
 * Inserts an element into the list at the front of the list.
 * @param value The value to insert into the list.
 */
template <typename T> bool LockFreeList<T>::Insert(T value) {
  LockFreeNode<T> *new_node = new LockFreeNode<T>(value, head->next, nullptr);
  while (!std::atomic_compare_exchange_weak(
      &head->next, (LockFreeNode<T> **)&new_node->next, new_node))
    ;
  size++;
  return true;
}

/**
 * Inserts an item only if the list does not cotain the item.
 * @param value The value to insert into the list.
 * @return True if the value was inserted, false otherwise.
 */
template <typename T> bool LockFreeList<T>::InsertUnique(T value) {
  LockFreeNode<T> *right_node, *left_node;
  LockFreeNode<T> *new_node = new LockFreeNode<T>(value, nullptr, nullptr);
  while (1) {
    right_node = search(value, &left_node);
    if (right_node != tail) {
      return false;
    } else {
      new_node->next = right_node;
      if (std::atomic_compare_exchange_weak(
              &left_node->next, (LockFreeNode<T> **)&right_node, new_node)) {
        size++;
        return true;
      }
    }
  }
}

/**
 * Removes an element from the list if the element is found.
 * @param value The value to remove from the list.
 */
template <typename T> bool LockFreeList<T>::Remove(T value) noexcept {
  LockFreeNode<T> *right_node, *left_node, *right_node_next;
  while (1) {
    right_node = search(value, &left_node);
    if ((right_node == tail) || (right_node->value != value)) {
      return false;
    }
    right_node_next = right_node->next;
    if (!is_marked(right_node_next)) {
      // logically delete node
      if (std::atomic_compare_exchange_weak(
              &right_node->next, (LockFreeNode<T> **)&right_node_next,
              get_marked(right_node_next))) {
        size--;
        break;
      }
    }
  }
  // physically delete node
  if (!std::atomic_compare_exchange_weak(&(left_node->next),
                                         (LockFreeNode<T> **)&right_node,
                                         right_node_next)) {
    search(value, &left_node);
  }
  return true;
}

/**
 * Checks if an element exists in the list
 * @param value The value to look for in the list.
 */
template <typename T> bool LockFreeList<T>::Contains(T value) const noexcept {
  auto node = get_unmarked(head->next);
  while (node != tail) {
    if (value == node->value && !is_marked(node->next)) {
      return true;
    }
    node = get_unmarked(node->next);
  }
  return false;
}

/**
 * @return The number of elements in the list.
 */
template <typename T> unsigned LockFreeList<T>::Size() const noexcept {
  return size;
}

/**
 * @return True if the list is empty, false otherwise.
 */
template <typename T> bool LockFreeList<T>::Empty() const noexcept {
  return size == 0u;
}

/**
 * Output stream operator.
 * @param os The output stream.
 * @param dlList The list to be inserted into the output stream.
 * @return A reference to the output stream.
 * @details The format is: (value1,value2,..,valueN).
 */
template <typename T>
std::ostream &operator<<(std::ostream &os, const LockFreeList<T> &dlList) {
  using LockGuard = typename LockFreeList<T>::LockGuard;
  LockGuard lck(dlList.mtx);
  os << "(";
  auto node = LockFreeList<T>::get_unmarked(dlList.head->next);
  while (node != dlList.tail) {
    if (!LockFreeList<T>::is_marked(node->next)) {
      os << node->value;
      break;
    }
    node = LockFreeList<T>::get_unmarked(node->next);
  }
  if (node == dlList.tail) {
    os << ')';
    return os;
  }

  node = LockFreeList<T>::get_unmarked(node->next);
  while (node != dlList.tail) {
    if (!LockFreeList<T>::is_marked(node->next)) {
      os << ',' << node->value;
    }
    node = LockFreeList<T>::get_unmarked(node->next);
  }
  os << ')';
  return os;
}

/**
 * Equality operator for LockFreeList.
 * @param lhs The list on the left side.
 * @param rhs The list on right side.
 * @return True if the lists have the same number of elements and all the
 *  elements are the same. Empty lists are considered equal.
 */
template <typename T>
bool operator==(const LockFreeList<T> &lhs, const LockFreeList<T> &rhs) {
  using LockGuard = typename LockFreeList<T>::LockGuard;
  LockGuard lck1(lhs.mtx);
  LockGuard lck2(rhs.mtx);

  if (lhs.size != rhs.size)
    return false;

  LockFreeNode<T> *lhs_node = lhs.get_next_unmarked(lhs.head);
  LockFreeNode<T> *rhs_node = rhs.get_next_unmarked(rhs.head);

  while(lhs_node != lhs.tail && rhs_node != rhs.tail){
    if(lhs_node->value != rhs_node->value){
      return false;
    }
    lhs_node = lhs.get_next_unmarked(lhs_node);
    rhs_node = rhs.get_next_unmarked(rhs_node);
  }
  return true;
}
