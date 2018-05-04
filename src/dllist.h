/**
 * @file dllist.h
 *
 * A simple doubly-linked list.
 */

#pragma once

#include <ostream>

#include "dlnode.h"

/**
 * A simple doubly linked list with very basic operations:
 * - inserting at the front of the list
 * - removing anywhere from the list
 * - and finding elements
 */
template <typename T> struct DlList {
  DlNode<T> *head{nullptr};
  unsigned size{0};

  virtual ~DlList();
  virtual DlNode<T> *Insert(T value);
  virtual bool InsertUnique(T value);
  virtual bool Remove(T value) noexcept;
  virtual bool Contains(T value) const noexcept;
  virtual bool Find(T &value) const noexcept;
  virtual unsigned Size() const noexcept;
  virtual bool Empty() const noexcept;
};

/**
 * Destroys the list.
 */
template <typename T> DlList<T>::~DlList() {
  auto node = head;
  while (node) {
    auto prev = node;
    node = node->next;
    delete prev;
  }
}

/**
 * Inserts an element into the list at the front of the list.
 * @param value The value to insert into the list.
 */
template <typename T> DlNode<T> *DlList<T>::Insert(T value) {
  // The list is empty
  if (not head) {
    head = new DlNode<T>(std::move(value));
    ++size;
    return head;
  } else {
    auto node = new DlNode<T>(std::move(value), nullptr, head);
    head->prev = node;
    head = node;
    ++size;
    return node;
  }
}

/**
 * Inserts an item only if the list does not cotain the item.
 * @param value The value to insert into the list.
 * @return True if the value was inserted, false otherwise.
 */
template <typename T> bool DlList<T>::InsertUnique(T value) {
  if (not head) {
    head = new DlNode<T>(std::move(value));
    ++size;
    return true;
  }

  auto node = head;
  for (; node; node = node->next) {
    if (node->value == value)
      return false;
    if (not node->next)
      break;
  }

  node->next = new DlNode<T>(std::move(value), node, nullptr);
  ++size;
  return true;
}

/**
 * Removes an element from the list if the element is found.
 * @param value The value to remove from the list.
 */
template <typename T> bool DlList<T>::Remove(T value) noexcept {
  // The list is empty
  if (not head)
    return false;

  auto node = head;
  while (node) {
    if (node->value == value) {
      if (head == node) {
        head = node->next;
        if (head)
          head->prev = nullptr;
        --size;
        delete node;
      } else {
        if (node->prev)
          node->prev->next = node->next;
        if (node->next)
          node->next->prev = node->prev;
        --size;
        delete node;
      }
      return true;
    }
    node = node->next;
  }

  return false;
}

/**
 * Removes an element from the list if the element is found.
 * @param value The value to remove from the list.
 */
template <typename T> bool DlList<T>::Contains(T value) const noexcept {
  auto node = head;
  while (node) {
    if (value == node->value)
      return true;
    node = node->next;
  }
  return false;
}

template <typename T> bool DlList<T>::Find(T &value) const noexcept {
  auto node = head;
  while (node) {
    if (value == node->value) {
      value = node->value;
      return true;
    }
    node = node->next;
  }
  return NULL;
}

/**
 * @return The number of elements in the list.
 */
template <typename T> unsigned DlList<T>::Size() const noexcept { return size; }

/**
 * @return True if the list is empty, false otherwise.
 */
template <typename T> bool DlList<T>::Empty() const noexcept {
  return size == 0u;
}

/**
 * Output stream operator for DlList.
 * @param os The output stream.
 * @param dlList The list to be inserted into the output stream.
 * @return A reference to the output stream.
 * @details The format is: (value1,value2,..,valueN).
 */
template <typename T>
std::ostream &operator<<(std::ostream &os, const DlList<T> &dlList) {
  os << "(";
  auto node = dlList.head;
  if (node) {
    os << node->value;
    node = node->next;
  }
  while (node) {
    os << ',' << node->value;
    node = node->next;
  }
  os << ')';
  return os;
}

/**
 * Equality operator for DlList.
 * @param lList the DlList on the left side.
 * @param rList The DlList on right side.
 * @return True if the lists have the same number of elements and all the
 *  elements are the same. Empty lists are considered equal.
 */
template <typename T>
bool operator==(const DlList<T> &lList, const DlList<T> &rList) {
  if (lList.size != rList.size)
    return false;
  auto node1 = lList.head;
  auto node2 = rList.head;
  while (node1 and node2) {
    if (node1->value != node2->value)
      return false;
    node1 = node1->next;
    node2 = node2->next;
  }
  return true;
}

/**
 * Non-equality operator for DlList.
 * @param lList the DlList on the left side.
 * @param rList The DlList on right side.
 * @return True if the lists are not the same, false otherwise.
 */
template <typename T>
bool operator!=(const DlList<T> &lList, const DlList<T> &rList) {
  return not(lList == rList);
}
