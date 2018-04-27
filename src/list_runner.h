/**
 * @file list_runner.h
 *
 * A harness that can run a benchmarking test for concurrent lists with the
 * following API:
 * - Insert(T value)
 * - InsertUnique(T value)
 * - Remove(T value)
 * - Contain(T value)
 */

#pragma once

#include <cassert>
#include <chrono>
#include <cmath>
#include <ostream>
#include <random>
#include <thread>

class ListRunner {
  static constexpr size_t kSeed = 117;
  static constexpr float kTolerance = 0.01;
  size_t nThreads;      // The number of threads to use for the benchmark.
  size_t nPerThread;    // The number of items per thread.
  size_t n;             // The total number of elements to run the test with.
  float percentInserts; // The percent of insertions to run.
  float percentRemoves; // The percent of removals to run.
  float percentLookups; // The percent of lookups to run.
  double runTime;       // The run time of the experiment in seconds.
  std::string listName;
  std::vector<int> numbers;

  template <typename TList> void Run(size_t threadId, TList &lst);

public:
  ListRunner(size_t nThreads, size_t nPerThread, float percentInserts,
             float percentRemoves, float percentLookups);
  template <template <typename> class TList>
  void Run(const std::string &description);
  void PrintSummary(std::ostream &os);
};

template <typename TList> void ListRunner::Run(size_t threadId, TList &lst) {
  constexpr auto removesThreshold = percentInserts + percentRemoves;
  size_t nCount = 0;
  int buf[nPerThread];
  auto genRand = std::bind(std::uniform_real_distribution<float>(),
                           std::default_random_engine(kSeed * threadId));
  auto first = threadId * nPerThread;
  for (auto last = first + nPerThread; first < last;) {
    auto r = genRand();
    if (r < percentInserts) {
      auto num = numbers[first++];
      buf[nCount++] = num;

      // Take turns inserting via Insert and InsertUnique
      if (genRand() < 0.5)
        lst.Insert(num);
      else
        lst.InsertUnique(num);
    } else if (r < removesThreshold) {
      if (nCount)
        lst.Remove(buf[nCount--]);
    } else {
      if (nCount) {
        // Generate an index at random
        size_t index = genRand() * (nCount - 1);
        lst.Contains(buf[index]);
      }
    }
  }
}

template <template <typename> class TList>
void ListRunner::Run(const std::string &description) {
  using namespace std::chrono;
  using ListType = TList<int>;
  listName = description;
  ListType lst;
  std::thread threads[nThreads];
  auto tnow = steady_clock::now();
  for (size_t i = 1; i < nThreads; ++i)
    threads[i] = std::thread(&ListRunner::Run<ListType>, i, std::ref(lst));
  // Thread 0 runs this
  Run(0, lst);
  for (size_t i = 1; i < nThreads; ++i)
    threads[i].join();
  auto dur = steady_clock::now() - tnow;
  // duration<double> is in seconds
  runTime = duration_cast<duration<double>>(dur).count();
}
