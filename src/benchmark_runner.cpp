/**
 * @file list_runner.cpp
 *
 * Non-template definitions for ListRunner.
 */

#include <cmath>
#include <ostream>
#include <thread>

#include "list_runner.h"
#include "util.h"

std::ostream &operator<<(std::ostream &os, ScalingMode mode) {
  const char *ptr;
  switch (mode) {
  case ScalingMode::Problem:
    ptr = "problem";
    break;
  case ScalingMode::Memory:
    ptr = "memory";
    break;
  default:
    ptr = "unknown";
    break;
  }
  return os << ptr;
}

std::ostream &operator<<(std::ostream &os, const RunnerResults &rr) {
  os << rr.listName << ',' << rr.params.nPerThread << ',' << rr.params.inserts
     << ',' << rr.params.removals << ',' << rr.params.lookups << ','
     << rr.params.scalingMode << ',' << rr.params.withAffinity << ','
     << rr.params.preload << ',';
  auto first = rr.runTimes.begin();
  auto last = rr.runTimes.end();
  if (first != last)
    os << *first++;
  while (first != last)
    os << ',' << *first++;
  return os;
}

/**
 * Initializes a ListRunner with the specs of the benchmark.
 *
 * @param params The parameters to use for running the benchmark.
 */
BenchmarkRunner::BenchmarkRunner(const RunnerParams &params)
    : params(params), nCores(std::thread::hardware_concurrency()) {
  PrepareNumbers();
}

void BenchmarkRunner::PrepareNumbers() {
  size_t n = params.nPerThread;
  if (params.scalingMode == ScalingMode::Memory)
    n *= nCores;
  for (size_t i = 0; i < n; ++i)
    numbers.push_back(i);
}

ChunkParams BenchmarkRunner::GetChunkParams(size_t threadId,
                                            size_t nThreads) const noexcept {
  size_t start, startNext, chunk, nPreload;
  if (params.scalingMode == ScalingMode::Memory) {
    start = threadId * params.nPerThread;
    startNext = start + params.nPerThread;
  } else {
    auto base = numbers.size() / nThreads;
    auto extra = numbers.size() % nThreads;
    auto nextId = threadId + 1;
    start = threadId < extra ? threadId * (base + 1) : threadId * base + extra;
    startNext = nextId < extra ? nextId * (base + 1) : nextId * base + extra;
  }
  chunk = startNext - start;
  nPreload = params.preload * chunk;
  return {start, startNext, chunk, nPreload};
}
