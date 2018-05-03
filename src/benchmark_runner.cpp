/**
 * @file benchmark_runner.cpp
 *
 * Non-template definitions for BenchmarkRunner.
 */

#include <cmath>
#include <ostream>
#include <thread>

#include "benchmark_runner.h"
#include "util.h"

const unsigned RunnerParams::nCores = std::thread::hardware_concurrency();

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
  os << rr.listName << ',' << rr.params.nCores << ',' << rr.params.minThreads
     << ',' << rr.params.maxThreads << ',' << rr.params.n << ','
     << rr.params.inserts << ',' << rr.params.removals << ','
     << rr.params.lookups << ',' << rr.params.scalingMode << ','
     << rr.params.withAffinity << ',' << rr.params.preload << ',';
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
BenchmarkRunner::BenchmarkRunner(const RunnerParams &params) : params(params) {
  PrepareNumbers();
}

void BenchmarkRunner::PrepareNumbers() {
  size_t n = params.n;
  if (params.scalingMode == ScalingMode::Memory)
    n *= params.nCores;
  for (size_t i = 0; i < n; ++i)
    numbers.push_back(i);
}

ChunkParams BenchmarkRunner::GetChunkParams(size_t threadId,
                                            size_t nThreads) const noexcept {
  size_t start, startNext, chunk, nPreload;
  if (params.scalingMode == ScalingMode::Memory) {
    start = threadId * params.n;
    startNext = start + params.n;
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
