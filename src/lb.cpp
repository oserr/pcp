/**
 * @file lb.cpp
 *
 * Runs the benchmark for the lists.
 */

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <thread>

#include "coarse_grain_list.h"
#include "dllist.h"
#include "fine_grain_list.h"
#include "list_runner.h"
#include "nonblocking_list.h"

int main() {
  std::vector<RunnerResults> results;
  RunnerParams params(10000, .80, .10, .10);
  for (auto b : {false, true}) {
    params.withAffinity = b;
    ListRunner runner(params);
    results.push_back(runner.Run<CoarseGrainList>("CoarseGrainList"));
    results.push_back(runner.Run<FineGrainList>("FineGrainList"));
    results.push_back(runner.Run<NonBlockingList>("NonBlockingList"));
  }
  std::cout << "list,nPerThread,%inserts,%removals,%lookups,"
            << "%scalingMode,%withAffinity,%preload,nThreads...\n";
  std::copy(results.begin(), results.end(),
            std::ostream_iterator<RunnerResults>(std::cout, "\n"));
  exit(EXIT_SUCCESS);
}
