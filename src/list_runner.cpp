/**
 * @file list_runner.cpp
 *
 * Non-template definitions for ListRunner.
 */

#include <cmath>
#include <ostream>

#include "list_runner.h"

/**
 * Initializes a ListRunner with the specs of the benchmark.
 *
 * @param nThreads The number of threads to use for the benchmark.
 * @param nPerThread The total number of numbers assigned to each thread.
 * @param percentInserts The percent of insertions to be carried out.
 * @param percentRemoves The percent of removals to be carried out.
 * @param percentLookups The percent of lookups to be carried out.
 *
 * @details Initializes a vector of unique numbers that the threads will use to
 * get their chunk of numbers to be used in the test. Verifies that the percents
 * sum up to at last .99.
 */
ListRunner::ListRunner(size_t nThreads, size_t nPerThread, float percentInserts,
                       float percentRemoves, float percentContains)
    : nThread(nThreads), nPerThread(nPerThread), n(nThreads * nPerThread),
      percentInserts(percentInserts), percentRemoves(percentRemoves),
      percentLookups(percentLookups), numbers(n) {
  assert(nThreads and nPerThread);
  auto totalPercent = percentInserts + percentRemoves + percentLookups;
  assert(std::fabs(1.0 - totalPercent) < kTolerance);
  for (size_t i = 0; i < n; ++i)
    numbers[i] = i;
}

/**
 * Prints a summary of the benchmark.
 *
 * @param os The output stream where stats are printed.
 */
void ListRunner::PrintSummary(std::ostream &os) {
  os << "------ " << listName << " -----\n"
     << "run time:      " << runTime << '\n'
     << "# threads:     " << nThreads << '\n'
     << "#s per thread: " << nPerThread << '\n'
     << "% inserts:     " << percentInserts << '\n'
     << "% removes:     " << percentRemoves << '\n'
     << "% lookups:     " << percentLookups << '\n';
}
