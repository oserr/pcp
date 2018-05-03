/**
 * @file benchmark_runner.h
 *
 * A harness that can run a benchmarking test for the lists and hash maps.
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
  size_t n{0};
  float inserts{0.0};
  float removals{0.0};
  float lookups{0.0};
  ScalingMode scalingMode{ScalingMode::Problem};
  bool withAffinity{false};
  float preload{0.0};
  static const unsigned nCores;
  unsigned minThreads{1};
  unsigned maxThreads{nCores};

  RunnerParams() = default;
  RunnerParams(size_t n, float inserts, float removals, float lookups)
      : n(n), inserts(inserts), removals(removals), lookups(lookups) {}
};

struct RunnerResults {
  std::string name;
  RunnerParams params;
  std::vector<double> runTimes;

  RunnerResults() = default;
  RunnerResults(const std::string &name, const RunnerParams &params)
      : name(name), params(params) {}
};

std::ostream &operator<<(std::ostream &os, const RunnerResults &results);

struct BenchmarkRunner {
  static constexpr size_t kSeed = 117;
  static constexpr float kTolerance = 0.01;
  RunnerParams params;
  std::vector<int> numbers;

  void PrepareNumbers();
  ChunkParams GetChunkParams(size_t threadId, size_t nThreads) const noexcept;

  BenchmarkRunner(const RunnerParams &params);

  // List functions

  template <typename TList>
  std::vector<std::vector<int>> PreloadList(TList &lst, size_t nThreads);

  template <template <typename> class TList>
  RunnerResults RunListSingle(const std::string &listName);

  template <template <typename> class TList>
  RunnerResults RunList(const std::string &listName);

  template <typename TList>
  void RunList(size_t threadId, size_t nThreads, TList &lst,
               std::vector<int> &buf);

  // Hashmap functions

  template <typename TMap>
  std::vector<std::vector<int>> PreloadMap(TMap &hashMap, size_t nThreads);

  template <template <typename, typename> class TMap>
  RunnerResults RunMapSingle(const std::string &mapName);

  template <template <typename, typename> class TMap>
  RunnerResults RunMap(const std::string &mapName);

  template <typename TMap>
  void RunMap(size_t threadId, size_t nThreads, TMap &hashMap,
              std::vector<int> &buf);
};

template <typename TList>
std::vector<std::vector<int>> BenchmarkRunner::PreloadList(TList &lst,
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

template <template <typename> class TList>
RunnerResults BenchmarkRunner::RunListSingle(const std::string &listName) {
  using namespace std::chrono;
  using ListType = TList<int>;
  RunnerResults results(listName, params);
  ListType lst;
  auto buffers = PreloadList(lst, 1);
  assert(buffers.size() == 1);
  auto timeStart = steady_clock::now();
  RunList(0, 1, lst, buffers[0]);
  auto dur = steady_clock::now() - timeStart;
  auto runTime = duration_cast<duration<double>>(dur).count();
  results.runTimes.push_back(runTime);
  return results;
}

template <template <typename> class TList>
RunnerResults BenchmarkRunner::RunList(const std::string &listName) {
  using namespace std::chrono;
  using ListType = TList<int>;
  RunnerResults results(listName, params);
  std::thread threads[params.maxThreads];
  for (size_t c = params.minThreads; c <= params.maxThreads; ++c) {
    ListType lst;
    auto buffers = PreloadList(lst, c);
    auto timeStart = steady_clock::now();
    for (size_t t = 1; t < c; ++t) {
      threads[t] = std::thread(&BenchmarkRunner::RunList<ListType>, this, t, c,
                               std::ref(lst), std::ref(buffers[t]));
    }
    RunList(0, c, lst, buffers[0]);
    for (size_t t = 1; t < c; ++t)
      threads[t].join();
    auto dur = steady_clock::now() - timeStart;
    auto runTime = duration_cast<duration<double>>(dur).count();
    results.runTimes.push_back(runTime);
  }
  return results;
}

template <typename TList>
void BenchmarkRunner::RunList(size_t threadId, size_t nThreads, TList &lst,
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

template <typename TMap>
std::vector<std::vector<int>> BenchmarkRunner::PreloadMap(TMap &hashMap,
                                                          size_t nThreads) {
  std::vector<std::vector<int>> buffers;
  for (size_t i = 0; i < nThreads; ++i) {
    auto cp = GetChunkParams(i, nThreads);
    std::vector<int> buf(cp.chunk);
    size_t k = 0;
    auto last = std::min(cp.startNext, cp.start + cp.nPreload);
    for (auto j = cp.start; j < last; ++j) {
      auto num = numbers[j];
      hashMap.Insert(num, num);
      buf[k++] = num;
    }
    buffers.emplace_back(std::move(buf));
  }
  return buffers;
}

template <template <typename, typename> class TMap>
RunnerResults BenchmarkRunner::RunMapSingle(const std::string &mapName) {
  using namespace std::chrono;
  using MapType = TMap<int, int>;
  RunnerResults results(mapName, params);
  MapType hashMap(params.n);
  auto buffers = PreloadMap(hashMap, 1);
  assert(buffers.size() == 1);
  auto timeStart = steady_clock::now();
  RunMap(0, 1, hashMap, buffers[0]);
  auto dur = steady_clock::now() - timeStart;
  auto runTime = duration_cast<duration<double>>(dur).count();
  results.runTimes.push_back(runTime);
  return results;
}

template <template <typename, typename> class TMap>
RunnerResults BenchmarkRunner::RunMap(const std::string &mapName) {
  using namespace std::chrono;
  using MapType = TMap<int, int>;
  RunnerResults results(mapName, params);
  std::thread threads[params.maxThreads];
  for (size_t c = params.minThreads; c <= params.maxThreads; ++c) {
    MapType hashMap(params.scalingMode == ScalingMode::Problem ? params.n
                                                               : params.n * c);
    auto buffers = PreloadMap(hashMap, c);
    auto timeStart = steady_clock::now();
    for (size_t t = 1; t < c; ++t) {
      threads[t] = std::thread(&BenchmarkRunner::RunMap<MapType>, this, t, c,
                               std::ref(hashMap), std::ref(buffers[t]));
    }
    RunMap(0, c, hashMap, buffers[0]);
    for (size_t t = 1; t < c; ++t)
      threads[t].join();
    auto dur = steady_clock::now() - timeStart;
    auto runTime = duration_cast<duration<double>>(dur).count();
    results.runTimes.push_back(runTime);
  }
  return results;
}

template <typename TMap>
void BenchmarkRunner::RunMap(size_t threadId, size_t nThreads, TMap &hashMap,
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
      hashMap.Insert(num, num);
      ++ops;
    } else if (r < rThreshold) {
      if (nCount) {
        size_t index = genRand() * (nCount - 1);
        hashMap.Remove(buf[index]);
        buf[index] = buf[nCount--];
        ++ops;
      }
    } else {
      if (nCount) {
        // Generate an index at random
        size_t index = genRand() * (nCount - 1);
        hashMap.Has(buf[index]);
        ++ops;
      }
    }
    // Make sure we don't get stuck in an infinite loop.
    if (first >= cp.startNext and nCount == 0 and ops < cp.chunk)
      break;
  }
}
