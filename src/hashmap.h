/**
 * @file hashmap.cpp
 *
 * A simple HashMap.
 */

#pragma once

#include <memory>
#include <ostream>

#include "dllist.h"

/**
 * A simple hashmap with very basic operations:
 * - insert (key, value)
 * - remove (key)
 * - has (key)
 */
template <typename K, typename V, template <typename> class TList>
struct HashMap {
  struct Element {
    K key{};
    V value{};
    /* ctor */
    Element() = default;
    Element(K key, V value) : key(std::move(key)), value(std::move(value)) {}
    Element(K key) : key(std::move(key)) {}
    Element(const Element &e) = default;
    Element &operator=(const Element &e) {
      key = e.key;
      value = e.value;
      return *this;
    }
    bool operator==(const Element &e) { return key == e.key; }
    bool operator!=(const Element &e) { return !(key == e.key); }
    friend std::ostream &operator<<(std::ostream &os, const Element &e) {
      os << e.key << ": " << e.value;
      return os;
    }
  };

  constexpr static int NUM_BUCKETS = 1000;
  std::unique_ptr<TList<Element>[]> buckets;
  const size_t nBuckets;
  unsigned size{0};
  std::hash<K> hasher{};

  /**
   * Constructs the HashMap with the default number of buckets, 1000.
   */
  HashMap() : buckets(new TList<Element>[NUM_BUCKETS]), nBuckets(NUM_BUCKETS) {}

  /**
   * Constructs a HashMap with a specified number of buckets.
   * @param nBuckets The number of buckets for the HashMap.
   */
  HashMap(size_t nBuckets)
      : buckets(new TList<Element>[nBuckets]), nBuckets(nBuckets) {}

  bool Insert(K key, V value);
  bool Remove(K key);
  bool Has(K key) const noexcept;
  unsigned Size() const noexcept;
  V operator[](K key);
};

/**
 * Inserts an element into the HashMap if the key does not exist.
 * @param key The key to look for.
 * @param value The value for the key.
 * @return True if the (key, value) pair were inserted into the map.
 */
template <typename K, typename V, template <typename> class TList>
bool HashMap<K, V, TList>::Insert(K key, V value) {
  auto &lst = buckets[hasher(key) % nBuckets];
  if (lst.InsertUnique(Element(key, value))) {
    ++size;
    return true;
  }
  return false;
}

/**
 * Removes the element from the HashMap that has the given key.
 * @param key The key to look for.
 * @return True if the key is removed from the map, false otherwise.
 */
template <typename K, typename V, template <typename> class TList>
bool HashMap<K, V, TList>::Remove(K key) {
  auto &lst = buckets[hasher(key) % nBuckets];
  if (lst.Remove(Element(key))) {
    --size;
    return true;
  }
  return false;
}

/**
 * Checks if a key exists in the HashMap
 * @param key The key to look for.
 * @return True if the hash map contains the key, false otherwise.
 */
template <typename K, typename V, template <typename> class TList>
bool HashMap<K, V, TList>::Has(K key) const noexcept {
  return buckets[hasher(key) % nBuckets].Contains(Element(key));
}

/**
 * @return The number of elements in the HashMap.
 */
template <typename K, typename V, template <typename> class TList>
unsigned HashMap<K, V, TList>::Size() const noexcept {
  return size;
}

/**
 * Gets a reference to an element in the HashMap.
 * @param key The key to look for.
 * @return Reference to the value, null if there is no such key
 */
template <typename K, typename V, template <typename> class TList>
V HashMap<K, V, TList>::operator[](K key) {
  Element e(key);
  auto bucket = hasher(key) % nBuckets;
  bool flag = buckets[bucket].Find(e);
  if (flag) {
    return e.value;
  } else {
    // could be changed to throw exception
    if (buckets[bucket].InsertUnique(e)) {
      size++;
    }
    return e.value;
  }
}

/**
 * Output stream operator for HashMap.
 * @param os The output stream.
 * @param hm The HashMap to be inserted into the output stream.
 * @return A reference to the output stream.
 */
template <typename K, typename V, template <typename> class TList>
std::ostream &operator<<(std::ostream &os, const HashMap<K, V, TList> &hm) {
  os << "{ ";
  for (unsigned i = 0; i < hm.nBuckets; i++) {
    os << "Bucket[" << i << "] -> " << hm.buckets[i] << '\n';
  }
  os << " }";
  return os;
}
