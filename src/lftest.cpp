/**
 * @file lftest.cpp
 *
 * Runs the benchmark for the lists.
 */

#include <climits>
#include <cstdlib>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

#include "lockfree_list.h"

int main(int argc, char *argv[]) {
  LockFreeList<int> lf;
  std::cout << "Initial Size: " << lf.Size() << std::endl;

  std::stringstream test;
  test << "19845)";
  test.seekg(1);
  int x;
  test >> x;
  std::cout << "test string to number: " << x << std::endl;

  lf.Insert(1);
  lf.Insert(2);
  lf.Insert(3);
  lf.Insert(4);
  std::cout << "Inserting 1 2 3 4: " << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;

  lf.Remove(2);
  // lf.Remove(3);
  std::cout << "Removing 2: " << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;

  lf.Remove(1);
  std::cout << "Removing 1: " << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;

  lf.Remove(3);
  std::cout << "Removing 3: " << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;

  lf.Remove(4);
  std::cout << "Removing 4: " << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;

  lf.Remove(4);
  std::cout << "Removing 4: " << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;

  lf.Insert(4);
  lf.InsertUnique(4);
  std::cout << "Inserting 4: " << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;

  std::cout << "First element: " << (*lf.head->next).value << std::endl;

  /* Test == operator */
  LockFreeList<int> lf1, lf2;
  std::vector<int> numbers;
  int size = 4;
  std::default_random_engine gen(
      std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> dist_insert(INT_MIN, INT_MAX);
  std::uniform_int_distribution<int> dist_remove(0, size - 1);

  for (int i = 0; i < size; i++) {
    int x = dist_insert(gen);
    numbers.push_back(x);
    lf1.Insert(x);
    lf2.Insert(x);
  }
  for (int i = 0; i < size / 10; i++) {
    int index = dist_remove(gen);
    std::cout << numbers[index] << std::endl;
    int x = numbers[index];
    lf1.Remove(x);
    lf2.Remove(x);
  }
  if (dist_insert(gen) > 0) {
    lf2.Remove(numbers[dist_remove(gen)]);
  }
  if (lf1 == lf2) {
    std::cout << "lf1 and lf2 are EQUAL" << std::endl;
    std::cout << lf1 << std::endl;
  } else {
    std::cout << "lf1 and lf2 are NOT EQUAL" << std::endl;
    std::cout << "lf1: " << lf1 << std::endl;
    std::cout << "lf2: " << lf2 << std::endl;
  }

  exit(EXIT_SUCCESS);
}
