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
#include <iostream>
#include <ostream>
#include <random>
#include <thread>

#include "util.h"

enum class ScalingMode { Problem, Memory };

std::ostream &operator<<(std::ostream &os, ScalingMode mode);

struct ChunkParams {
  size_t start;
  size_t startNext;
  size_t chunk;
  size_t nPreload;
};

struct RunnerParams {
  size_t nPerThread{0};
  float inserts{0.0};
  float removals{0.0};
  float lookups{0.0};
  ScalingMode scalingMode{ScalingMode::Problem};
  bool withAffinity{false};
  float preload{0.0};

  RunnerParams() = default;
  RunnerParams(size_t nPerThread, float inserts, float removals, float lookups)
      : nPerThread(nPerThread), inserts(inserts), removals(removals),
        lookups(lookups) {}
};

struct RunnerResults {
  std::string listName;
  RunnerParams params;
  std::vector<double> runTimes;

  RunnerResults() = default;
  RunnerResults(const std::string &listName, const RunnerParams &params)
      : listName(listName), params(params) {}
};

std::ostream &operator<<(std::ostream &os, const RunnerResults &results);

struct ListRunner {
  static constexpr size_t kSeed = 117;
  static constexpr float kTolerance = 0.01;
  RunnerParams params;
  unsigned nCores;
  std::vector<int> numbers;

  template <typename TList>
  void Run(size_t threadId, size_t nThreads, TList &lst, std::vector<int> &buf);
  template <typename TList>
  std::vector<std::vector<int>> DoPreload(TList &lst, size_t nThreads);
  void PrepareNumbers();
  ChunkParams GetChunkParams(size_t threadId, size_t nThreads) const noexcept;

  ListRunner(const RunnerParams &params);
  template <template <typename> class TList>
  RunnerResults Run(const std::string &description);
  template <template <typename> class TList>
  RunnerResults RunSingle(const std::string &description);
};

template <template <typename> class TList>
RunnerResults ListRunner::Run(const std::string &listName) {
  using namespace std::chrono;
  using ListType = TList<int>;
  RunnerResults results(listName, params);
  std::thread threads[nCores];
  for (size_t c = 1; c <= nCores; ++c) {
    ListType lst;
    auto buffers = DoPreload(lst, c);
    auto timeStart = steady_clock::now();
    for (size_t t = 1; t < c; ++t) {
      threads[t] = std::thread(&ListRunner::Run<ListType>, this, t, c,
                               std::ref(lst), std::ref(buffers[t]));
    }
    Run(0, c, lst, buffers[0]);
    for (size_t t = 1; t < c; ++t)
      threads[t].join();
    auto dur = steady_clock::now() - timeStart;
    auto runTime = duration_cast<duration<double>>(dur).count();
    results.runTimes.push_back(runTime);
  }
  return results;
}

template <template <typename> class TList>
RunnerResults ListRunner::RunSingle(const std::string &listName) {
  using namespace std::chrono;
  using ListType = TList<int>;
  RunnerResults results(listName, params);
  ListType lst;
  auto buffers = DoPreload(lst, 1);
  assert(buffers.size() == 1);
  auto timeStart = steady_clock::now();
  Run(0, 1, lst, buffers[0]);
  auto dur = steady_clock::now() - timeStart;
  auto runTime = duration_cast<duration<double>>(dur).count();
  results.runTimes.push_back(runTime);
  return results;
}

template <typename TList>
std::vector<std::vector<int>> ListRunner::DoPreload(TList &lst,
                                                    size_t nThreads) {
  std::vector<std::vector<int>> buffers;
  for (size_t i = 0; i < nThreads; ++i) {
    auto cp = GetChunkParams(i, nThreads);
    std::vector<int> buf(cp.chunk);
    size_t k = 0;
    auto last = std::min(cp.startNext, cp.start + cp.nPreload);
    for (auto j = cp.start; j < last; ++j) {
      auto num = numbers[j];
      lst.Insert(num);
      buf[k++] = num;
    }
    buffers.emplace_back(std::move(buf));
  }
  return buffers;
}

template <typename TList>
void ListRunner::Run(size_t threadId, size_t nThreads, TList &lst,
                     std::vector<int> &buf) {
  if (params.withAffinity)
    setCoreAffinity(threadId);

  const auto rThreshold = params.inserts + params.removals;
  auto cp = GetChunkParams(threadId, nThreads);
  size_t nCount = cp.nPreload;
  auto genRand = std::bind(std::uniform_real_distribution<float>(),
                           std::default_random_engine(kSeed * threadId));
  auto first = cp.start + cp.nPreload;
  for (size_t ops = 0; ops < cp.chunk;) {
    auto r = genRand();
    if (r < params.inserts and first < cp.startNext) {
      auto num = numbers[first++];
      buf[nCount++] = num;
      ++ops;

      // Take turns inserting via Insert and InsertUnique
      if (genRand() < 0.5)
        lst.Insert(num);
      else
        lst.InsertUnique(num);
    } else if (r < rThreshold) {
      if (nCount) {
        size_t index = genRand() * (nCount - 1);
        lst.Remove(buf[index]);
        buf[index] = buf[nCount--];
        ++ops;
      }
    } else {
      if (nCount) {
        // Generate an index at random
        size_t index = genRand() * (nCount - 1);
        lst.Contains(buf[index]);
        ++ops;
      }
    }
    // Make sure we don't get stuck in an infinite loop.
    if (first >= cp.startNext and nCount == 0 and ops < cp.chunk)
      break;
  }
}
