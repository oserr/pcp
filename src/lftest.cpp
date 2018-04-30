/**
 * @file lftest.cpp
 *
 * Runs the benchmark for the lists.
 */

#include <cstdlib>
#include <iostream>

#include "lockfree_list.h"

int main(int argc, char *argv[]) {
  LockFreeList<int> lf;
  std::cout << "Initial Size: " << lf.Size() << std::endl;
  
  lf.Insert(1);
  lf.Insert(2);
  lf.Insert(3);
  lf.Insert(4);
  std::cout << "Inserting 1 2 3 4: " << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;
  
  lf.Remove(2);
  //lf.Remove(3);
  std::cout << "Removing 2: "  << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;
  
  lf.Remove(1);
  std::cout << "Removing 1: "  << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;
  
  lf.Remove(3);
  std::cout << "Removing 3: "  << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;

  lf.Remove(4);
  std::cout << "Removing 4: "  << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;

  lf.Remove(4);
  std::cout << "Removing 4: "  << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;

  lf.Insert(4);
  lf.InsertUnique(4);
  std::cout << "Inserting 4: " << lf << std::endl;
  std::cout << "Size: " << lf.Size() << std::endl;
  
  LockFreeNode<int> *node = lf.head->next;
  //int x = lf.head->next->value;
  std::cout << "First element: " << node->value << std::endl;
  
  exit(EXIT_SUCCESS);
}


