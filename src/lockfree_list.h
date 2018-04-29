/**
 * @file lockfree_list.h
 *
 * A simple lockfree linked list.
 */

#pragma once

#include <ostream>
#include <atomic>
#include <mutex>

#include "dlnode.h"

/**
 * A simple lockfree single linked list with very basic operations:
 * - Insert(): inserts a value at the front of the list
 * - InsertUnique(): inserts a value only if it does not exists in the list already
 * - Remove(): remove a value from the list
 * - Contains(): Checks if a value is in the list
 * - Size(): Returns the size of the list
 */
template <typename T> struct LockFreeList {
  DlNode<T> *head;
  DlNode<T> *tail;
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

  private:
    DlNode<T> *search(T value, DlNode<T> **left_node) const;
    bool is_marked(DlNode<T> *) const;
    DlNode<T> *get_marked(DlNode<T> *) const;
    DlNode<T> *get_unmarked(DlNode<T> *) const;
};

template <typename T> bool LockFreeList<T>::is_marked(DlNode<T> *addr) const{
  return 0x1 & (long)addr;
}

template <typename T> DlNode<T> *LockFreeList<T>::get_marked(DlNode<T> *addr) const{
  return (DlNode<T> *)((long)addr | 0x01); 
}

template <typename T> DlNode<T> *LockFreeList<T>::get_unmarked(DlNode<T> *addr) const{
  return (DlNode<T> *)((long)addr & ~0x01); 
}


template <typename T> DlNode<T> *LockFreeList<T>::search(T value, DlNode<T> **left_node) const{
  DlNode<T> *left_node_nxt, *right_node;
  while(1){
    DlNode<T> *node = head;
    DlNode<T> *node_nxt = head->next;

    while(1){
      if (!is_marked(node_nxt)) {
        (*left_node) = node;
        left_node_nxt = node_nxt;
      }
      // advance node
      node = get_unmarked(node_nxt);
      if(node == tail)
        break;
      node_nxt = node->next;
      if(!is_marked(node_nxt) && node->value == value)
        break;
    }
    right_node = node;
 
    if (left_node_nxt == right_node){
      return right_node;
    }
    else{
      //if (std::atomic_compare_exchange_weak(&(*left_node)->next, left_node_nxt, right_node))
        return right_node;
    }
  }
}

/**
 * Initializes the list.
 */
template <typename T> LockFreeList<T>::LockFreeList() {
  head = new DlNode<T>();
  tail = new DlNode<T>();
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
  DlNode<T> *new_node = new DlNode<T>(value, head->next, nullptr);
  while(!std::atomic_compare_exchange_weak(&head->next, &new_node->next, new_node));
  size++;
  return true;
}

/**
 * Inserts an item only if the list does not cotain the item.
 * @param value The value to insert into the list.
 * @return True if the value was inserted, false otherwise.
 */
template <typename T> bool LockFreeList<T>::InsertUnique(T value) {
  DlNode<T> *right_node, *left_node;
  DlNode<T> *new_node = new DlNode<T>(value, nullptr, nullptr);
  while(1){
    right_node = search(value, &left_node);
    if (right_node != tail)
      return false;
    else{
      new_node->next = right_node;
      //if(std::atomic_compare_exchange_weak(&left_node->next, &right_node, new_node)){
        size++;
        return true;
      //}
    }
  } 
}

/**
 * Removes an element from the list if the element is found.
 * @param value The value to remove from the list.
 */
template <typename T> bool LockFreeList<T>::Remove(T value) noexcept {
  DlNode<T> *right_node, *left_node;
  while(1){
    right_node = search(value, &left_node);
    if ((right_node == tail) || (right_node->value != value))
      return false;
    DlNode<T> *right_node_next = right_node->next;
    if (!is_marked(right_node_next)){
      //logically delete node
      //if (std::atomic_compare_exchange_weak(&right_node->next, right_node_next, get_marked(right_node_next))){
        break;
        size--;
      //}
    }
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
    if (value == node->value && !is_marked(node->next))
      return true;
    node = get_unmarked(node->next);
  }
  return false;
}

/**
 * @return The number of elements in the list.
 */
template <typename T> unsigned LockFreeList<T>::Size() const noexcept { return size; }

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
  auto node = get_unmarked(dlList.head->next);
  if (node != dlList.tail && !is_marked(node->next)) {
    os << node->value;
    node = get_unmarked(node->next);
    while (node != dlList.tail && !is_marked(node->next)) {
      os << ',' << node->value;
      node = get_unmarked(node->next);
    }
  }
  os << ')';
  return os;
}
