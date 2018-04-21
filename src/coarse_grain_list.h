/**
 * @file coarse_grain_list.h
 *
 * A doubly-linked list coarse grained locks.
 */

#pragma once

#include <mutex>
#include <ostream>

#include "dllist.h"

/**
 * A simple doubly linked list with very basic operations:
 * - inserting at the front of the list
 * - removing anywhere from the list
 * - and finding elements
 */
template <typename T> struct CoarseGrainList : DlList<T> {
  using LockGuard = std::lock_guard<std::mutex>;
  mutable std::mutex mtx;

  DlNode<T> *Insert(T value) override;
  bool Remove(T value) noexcept;
  bool Contains(T value) const noexcept override;
  T *Find(T value) const noexcept override;
  unsigned Size() const noexcept override;
  bool Empty() const noexcept override;
};


/**
 * Inserts an element into the list at the front of the list.
 * @param value The value to insert into the list.
 */
template <typename T> DlNode<T> *CoarseGrainList<T>::Insert(T value) {
  LockGuard lck(mtx);
  return DlList<T>::Insert(value);
}

/**
 * Removes an element from the list if the element is found.
 * @param value The value to remove from the list.
 */
template <typename T> bool CoarseGrainList<T>::Remove(T value) noexcept {
  LockGuard lck(mtx);
  return DlList<T>::Remove(value);
}

/**
 * Removes an element from the list if the element is found.
 * @param value The value to remove from the list.
 */
template <typename T> bool CoarseGrainList<T>::Contains(T value) const noexcept {
  LockGuard lck(mtx);
  return DlList<T>::Contains(value);
}

template <typename T> T *CoarseGrainList<T>::Find(T value) const noexcept {
  LockGuard lck(mtx);
  return DlList<T>::Find(value);
}

/**
 * @return The number of elements in the list.
 */
template <typename T> unsigned CoarseGrainList<T>::Size() const noexcept
{
  LockGuard lck(mtx);
  return DlList<T>::Size();
}

/**
 * @return True if the list is empty, false otherwise.
 */
template <typename T> bool CoarseGrainList<T>::Empty() const noexcept {
  LockGuard lck(mtx);
  return DlList<T>::Empty();
}

/**
 * Output stream operator for CoarseGrainList.
 * @param os The output stream.
 * @param lst The list to be inserted into the output stream.
 * @return A reference to the output stream.
 * @details The format is: (value1,value2,..,valueN).
 */
template <typename T>
std::ostream &operator<<(std::ostream &os, const CoarseGrainList<T> &lst) {
  using LockGuard = typename CoarseGrainList<T>::LockGuard;
  LockGuard lck(lst.mtx);
  return os << *dynamic_cast<const DlList<T>*>(&lst);
}

/**
 * Equality operator for CoarseGrainList.
 * @param lhs The list on the left hand side.
 * @param rhs The list on the right hand side.
 * @return True if the lists have the same number of elements and all the
 *  elements are the same. Empty lists are considered equal.
 */
template <typename T>
bool operator==(const CoarseGrainList<T> &lhs, const CoarseGrainList<T> &rhs) {
    if (&lhs.mtx == &rhs.mtx)
        return true;
    using LockGuard = typename CoarseGrainList<T>::LockGuard;
    std::lock(lhs.mtx, rhs.mtx);
    LockGuard lck1(lhs.mtx, std::adopt_lock);
    LockGuard lck2(rhs.mtx, std::adopt_lock);
    using Base = const DlList<T>*;
    return *dynamic_cast<Base>(&lhs) == *dynamic_cast<Base>(&rhs);
}

/**
 * Non-equality operator for CoarseGrainList.
 * @param lhs The list on the left side.
 * @param rhs The list on right side.
 * @return True if the lists are not the same, false otherwise.
 */
template <typename T>
bool operator!=(const CoarseGrainList<T> &lhs, const CoarseGrainList<T> &rhs) {
  return not(lhs == rhs);
}

