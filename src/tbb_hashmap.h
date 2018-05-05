/**
 * @file tbb_hashmap.cpp
 *
 * Wrapper for tbb hashmap
 */

#pragma once

#include <utility>

#include "tbb/concurrent_hash_map.h"

template <typename K, typename V> struct TbbHashMap {
  using size_type = typename tbb::concurrent_hash_map<K, V>::size_type;
  using accessor = typename tbb::concurrent_hash_map<K, V>::accessor;
  using const_accessor =
      typename tbb::concurrent_hash_map<K, V>::const_accessor;
  constexpr static size_type NUM_BUCKETS = 1000;
  tbb::concurrent_hash_map<K, V> table;

  /**
   * Constructs the TbbHashMap with the default number of buckets, 1000.
   */
  TbbHashMap() : table(NUM_BUCKETS) {}

  /**
   * Constructs a TbbHashMap with a specified number of buckets.
   * @param nBuckets The number of buckets for the TbbHashMap.
   */
  TbbHashMap(size_type nBuckets) : table(nBuckets) {}

  ~TbbHashMap() = default;
  bool Insert(K key, V value);
  bool Remove(K key);
  bool Has(K key) const noexcept;
  unsigned Size() const noexcept;
  V operator[](K key);
};

/**
 * Inserts an element into the TbbHashMap if the key does not exist.
 * @param key The key to look for.
 * @param value The value for the key.
 * @return True if the (key, value) pair were inserted into the map.
 */
template <typename K, typename V>
bool TbbHashMap<K, V>::Insert(K key, V value) {
  accessor a;
  return table.insert(a, std::make_pair(key, value));
}

/**
 * Removes the element from the TbbHashMap that has the given key.
 * @param key The key to look for.
 * @return True if the key is removed from the map, false otherwise.
 */
template <typename K, typename V> bool TbbHashMap<K, V>::Remove(K key) {
  return table.erase(key);
}

/**
 * Checks if a key exists in the TbbHashMap
 * @param key The key to look for.
 * @return True if the hash map contains the key, false otherwise.
 */
template <typename K, typename V>
bool TbbHashMap<K, V>::Has(K key) const noexcept {
  const_accessor ca;
  return table.find(ca, key);
}

/**
 * @return The number of elements in the TbbHashMap.
 */
template <typename K, typename V>
unsigned TbbHashMap<K, V>::Size() const noexcept {
  return table.size();
}

/**
 * Gets a reference to an element in the HashMap.
 * @param key The key to look for.
 * @return Reference to the value, null if there is no such key
 */
template <typename K, typename V> V TbbHashMap<K, V>::operator[](K key) {
  accessor a;
  if (table.find(a, key)) {
    return a->second;
  } else {
    V value;
    // This insert might fail, but we're not testing this.
    table.insert(a, std::make_pair(key, value));
    return value;
  }
}
