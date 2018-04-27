/**
 * @file lb.cpp
 *
 * Runs the benchmark for the lists.
 */

#include <cstdlib>
#include <iostream>

#include "coarse_grain_list.h"
#include "dllist.h"
#include "fine_grain_list.h"
#include "list_runner.h"
#include "nonblocking_list.h"

int main(int argc, char *argv[]) {
  ListRunner runner(4, 1000, .8, .1, .1);
  runner.Run<FineGrainList>("FineGrainList");
  runner.PrintSummary(std::cout);
  exit(EXIT_SUCCESS);
}
