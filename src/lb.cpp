/**
 * @file lb.cpp
 *
 * Runs the benchmark for the lists.
 */

#include <cstdlib>
#include <iostream>
#include <thread>

#include "coarse_grain_list.h"
#include "dllist.h"
#include "fine_grain_list.h"
#include "list_runner.h"
#include "nonblocking_list.h"

int main() {
  const unsigned nMachineThreads = std::thread::hardware_concurrency();
  std::cout << "HARDWARE CONCURRENCY = " << nMachineThreads << '\n';
  for (unsigned t = 1; t <= nMachineThreads; ++t) {
    ListRunner runner(t, 1000, .8, .1, .1);
    if (t == 1) {
      runner.Run<DlList>("DlList");
      runner.PrintSummary(std::cout);
    }
    runner.Run<CoarseGrainList>("CoarseGrainList");
    runner.PrintSummary(std::cout);
    runner.Run<FineGrainList>("FineGrainList");
    runner.PrintSummary(std::cout);
    runner.Run<NonBlockingList>("NonBlockingList");
    runner.PrintSummary(std::cout);
  }
  exit(EXIT_SUCCESS);
}
