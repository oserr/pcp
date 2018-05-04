/**
 * @file libcuckoo_hashmap.cpp
 *
 * Wrapper for cuckoo hashmap
 */

#pragma once

#include <ostream>

#include <libcuckoo/cuckoohash_map.hh>

/**
 * A simple doubly linked list with very basic operations:
 * - inserting at the front of the list
 * - removing anywhere from the list
 * - and finding elements
 */
template <typename K, typename V> struct LibCuckooHashMap {
  constexpr static int NUM_BUCKETS = 1000;
  cuckoohash_map<K, V> table;

  /**
   * Constructs the LibCuckooHashMap with the default number of buckets, 1000.
   */
  LibCuckooHashMap() : table(NUM_BUCKETS) {}

  /**
   * Constructs a LibCuckooHashMap with a specified number of buckets.
   * @param nBuckets The number of buckets for the LibCuckooHashMap.
   */
  LibCuckooHashMap(size_t nBuckets) : table(nBuckets) {}

  ~LibCuckooHashMap() {}
  bool Insert(K key, V value);
  bool Remove(K key);
  bool Has(K key) const noexcept;
  unsigned Size() const noexcept;
  V operator[](K key);
};

/**
 * Inserts an element into the LibCuckooHashMap if the key does not exist.
 * @param key The key to look for.
 * @param value The value for the key.
 * @return True if the (key, value) pair were inserted into the map.
 */
template <typename K, typename V>
bool LibCuckooHashMap<K, V>::Insert(K key, V value) {
  return table.insert(key, value);
}

/**
 * Removes the element from the LibCuckooHashMap that has the given key.
 * @param key The key to look for.
 * @return True if the key is removed from the map, false otherwise.
 */
template <typename K, typename V> bool LibCuckooHashMap<K, V>::Remove(K key) {
  return table.erase(key);
}

/**
 * Checks if a key exists in the LibCuckooHashMap
 * @param key The key to look for.
 * @return True if the hash map contains the key, false otherwise.
 */
template <typename K, typename V>
bool LibCuckooHashMap<K, V>::Has(K key) const noexcept {
  return table.contains(key);
}

/**
 * @return The number of elements in the LibCuckooHashMap.
 */
template <typename K, typename V>
unsigned LibCuckooHashMap<K, V>::Size() const noexcept {
  return table.size();
}

/**
 * Gets a reference to an element in the HashMap.
 * @param key The key to look for.
 * @return Reference to the value, null if there is no such key
 */
template <typename K, typename V> V LibCuckooHashMap<K, V>::operator[](K key) {
  V value;
  if (table.find(key, value)) {
    return value;
  } else {
    table.insert(key, value);
    return value;
  }
}
