/**
 * @file hashmap.cpp
 *
 * A simple HashMap.
 */

#pragma once

#include "dllist.h"
#include <ostream>

/**
 * A simple hashmap with very basic operations:
 * - insert (key, value)
 * - remove (key)
 * - has (key)
 */
template <typename K, typename V> class HashMap {
public:
  class Element {
  public:
    K key;
    V value;
    /* ctor */
    Element() {}
    Element(K key, V value) : key(key), value(value) {}
    Element(const Element &e) = default;
    bool operator==(const Element &e) { return key == e.key; }
    friend std::ostream &operator<<(std::ostream &os, const Element &e) {
      os << e.key << ": " << e.value;
      return os;
    }
  };

  const static int MAX_BUCKETS = 1000;
  DlList<Element> map[MAX_BUCKETS];
  unsigned size{0};

  /* default ctr */
  // HashMap();

  void Insert(K key, V value);
  void Remove(K key);
  bool Has(K key);
  unsigned Size() const noexcept;
  V &operator[](K key);

  /* dtor */
  //~HashMap();
};

/**
 * Inserts an element into the HashMap or modifies its value if
 * it already exists.
 * @param key The key to look for.
 * @param value The value for the key.
 */
template <typename K, typename V> void HashMap<K, V>::Insert(K key, V value) {
  Element new_elem(key, value);
  std::hash<K> hash;
  int bucket = hash(key) % MAX_BUCKETS;
  Element *elem = map[bucket].Find(new_elem);
  if (elem) {
    elem->value = new_elem.value;
  } else {
    map[bucket].Insert(new_elem);
    size++;
  }
}

/**
 * Removes the element from the HashMap that has the given key.
 * @param key The key to look for.
 */
template <typename K, typename V> void HashMap<K, V>::Remove(K key) {
  Element e;
  e.key = key;
  std::hash<K> hash;
  int bucket = hash(key) % MAX_BUCKETS;
  if (map[bucket].Remove(e)) {
    size--;
  }
}

/**
 * Checks if a key exists in the HashMap
 * @param key The key to look for.
 */
template <typename K, typename V> bool HashMap<K, V>::Has(K key) {
  Element e;
  e.key = key;
  std::hash<K> hash;
  int bucket = hash(key) % MAX_BUCKETS;
  return map[bucket].Contains(e);
}

/**
 * @return The number of elements in the HashMap.
 */
template <typename K, typename V>
unsigned HashMap<K, V>::Size() const noexcept {
  return size;
}

/**
 * Gets a reference to an element in the HashMap
 * @param key The key to look for.
 * @return Reference to the value, null if there is no such key
 */
template <typename K, typename V> V &HashMap<K, V>::operator[](K key) {
  Element e;
  e.key = key;
  std::hash<K> hash;
  int bucket = hash(key) % MAX_BUCKETS;
  Element *elem = map[bucket].Find(e);
  if (elem) {
    return elem->value;
  } else {
    // could be changed to throw exception
    map[bucket].Insert(e);
    size++;
    return map[bucket].Find(e)->value;
  }
}

/**
 * Output stream operator for HashMap.
 * @param os The output stream.
 * @param hm The HashMap to be inserted into the output stream.
 * @return A reference to the output stream.
 */
template <typename K, typename V>
std::ostream &operator<<(std::ostream &os, const HashMap<K, V> &hm) {
  os << "{ ";
  for (unsigned i = 0; i < hm.MAX_BUCKETS; i++) {
    if (hm.map[i].Size()) {
      os << "Bucket[" << i << "] -> " << hm.map[i] << std::endl;
    }
  }
  os << " }";
  return os;
}
