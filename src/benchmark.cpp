/**
 * @file benchmark.cpp
 *
 * Runs the benchmark for the lists.
 */

#include <getopt.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string>
#include <thread>
#include <vector>

#include "benchmark_runner.h"
#include "coarse_grain_list.h"
#include "dllist.h"
#include "fine_grain_list.h"
#include "hashmap.h"
#include "lockfree_list.h"
#include "nonblocking_list.h"

namespace {
// Aliases
template <typename T, typename K> using DlListMap = HashMap<T, K, DlList>;
template <typename T, typename K>
using CoarseGrainListMap = HashMap<T, K, CoarseGrainList>;
template <typename T, typename K>
using FineGrainListMap = HashMap<T, K, FineGrainList>;
template <typename T, typename K>
using NonBlockingListMap = HashMap<T, K, NonBlockingList>;
template <typename T, typename K>
using LockFreeListMap = HashMap<T, K, LockFreeList>;

void usage(const char *name);
void usageErr(const char *name);
void checkArgs(const RunnerParams &params);
void printResults(const std::vector<RunnerResults> &results,
                  const RunnerParams &params, bool pretty);
} // anonymous namespace

int main(int argc, char *argv[]) {
  static struct option longOptions[] = {
      {"help", no_argument, nullptr, 'h'},
      {"numbers", required_argument, nullptr, 'n'},
      {"inserts", required_argument, nullptr, 'i'},
      {"removals", required_argument, nullptr, 'r'},
      {"lookups", required_argument, nullptr, 'l'},
      {"scaling", required_argument, nullptr, 's'},
      {"affinity", no_argument, nullptr, 'a'},
      {"preload", required_argument, nullptr, 'p'},
      {"min-threads", required_argument, nullptr, 'm'},
      {"max-threads", required_argument, nullptr, 'x'},
      {"pretty", no_argument, nullptr, 'f'},
      {0, 0, 0, 0}};
  bool isPrettyFormat = false;
  RunnerParams params;
  int opt;
  while ((opt = getopt_long(argc, argv, "hn:i:r:l:s:ap:m:x:", longOptions,
                            nullptr)) != -1) {
    switch (opt) {
    case 'h':
      usage(argv[0]);
      std::exit(EXIT_SUCCESS);
    case 'n':
      params.n = std::stoull(optarg);
      break;
    case 'i':
      params.inserts = std::stof(optarg);
      break;
    case 'r':
      params.removals = std::stof(optarg);
      break;
    case 'l':
      params.lookups = std::stof(optarg);
      break;
    case 's':
      switch (optarg[0]) {
      case 'p':
      case 'P':
        params.scalingMode = ScalingMode::Problem;
        break;
      case 'm':
      case 'M':
        params.scalingMode = ScalingMode::Memory;
        break;
      default:
        usageErr(argv[0]);
      }
      break;
    case 'a':
      params.withAffinity = true;
      break;
    case 'p':
      params.preload = std::stof(optarg);
      break;
    case 'm':
      params.minThreads = std::stoul(optarg);
      break;
    case 'x':
      params.maxThreads = std::stoul(optarg);
      break;
    case 'f':
      isPrettyFormat = true;
      break;
    case '?':
    default:
      usageErr(argv[0]);
    }
  }

  checkArgs(params);
  std::vector<RunnerResults> results;
  BenchmarkRunner runner(params);

  // Lists

  results.push_back(runner.RunListSingle<DlList>("DlList"));
  results.push_back(runner.RunList<CoarseGrainList>("CoarseGrainList"));
  results.push_back(runner.RunList<FineGrainList>("FineGrainList"));
  results.push_back(runner.RunList<NonBlockingList>("NonBlockingList"));
  results.push_back(runner.RunList<LockFreeList>("LockFreeList"));

  // Maps

  results.push_back(runner.RunMapSingle<DlListMap>("DlListMap"));
  results.push_back(runner.RunMap<CoarseGrainListMap>("CoarseGrainListMap"));
  results.push_back(runner.RunMap<FineGrainListMap>("FineGrainListMap"));
  results.push_back(runner.RunMap<NonBlockingListMap>("NonBlockingListMap"));
  results.push_back(runner.RunMap<LockFreeListMap>("LockFreeListMap"));

  printResults(results, params, isPrettyFormat);
  std::exit(EXIT_SUCCESS);
}

namespace {

void usage(const char *name) {
  std::printf("Usage: %s [options]\n", name);
  std::printf("Program Options:\n");
  std::printf("\t-h  --help\n");
  std::printf("\t\tPrints this help message.\n");
  std::printf("\t-n  --numbers  <UNSIGNED>\n");
  std::printf("\t\tThe total number of operations to be performed.\n");
  std::printf("\t\tWith problem scaling, the number is fixed and gets\n");
  std::printf("\t\tdivided when more than one thread is running. With\n");
  std::printf("\t\tmemory scaling, this reresents the number of opertions\n");
  std::printf("\t\tto be performed by each thread.\n");
  std::printf("\t-i  --inserts  <FLOAT>\n");
  std::printf("\t\tA number between 0 and 1 representing the percent of\n");
  std::printf("\t\tinsertions to be performed. The sum of i, r, and l\n");
  std::printf("\t\tmust be no less than 0.01 from 1.\n");
  std::printf("\t-r  --removals <FLOAT>\n");
  std::printf("\t\tA number between 0 and 1 representing the percent of\n");
  std::printf("\t\tremovals to be performed. The sum of i, r, and l must\n");
  std::printf("\t\tbe no less than 0.01 from 1.\n");
  std::printf("\t-l  --lookups  <FLOAT>\n");
  std::printf("\t\tA number between 0 and 1 representing the percent of\n");
  std::printf("\t\tlookups to be performed. The sum of i, r, and l must\n");
  std::printf("\t\tbe no less than 0.01 from 1.\n");
  std::printf("\t-s  --scaling  <p|P|m|M>\n");
  std::printf("\t\tThe scaling type, where p or P represent problem\n");
  std::printf("\t\tscaling, and m or M represent memory scaling. Runs with\n");
  std::printf("\t\tproblem scaling by defualt.\n");
  std::printf("\t-a  --affinity\n");
  std::printf("\t\tThe threading affinity. This runs each thread on a\n");
  std::printf("\t\tseparate core. If the flag is not provided, then no\n");
  std::printf("\t\teffort is made to schedule threads in their own cores.\n");
  std::printf("\t\tDoes not run with program affinity by default.\n");
  std::printf("\t-p  --preload  <FLOAT>\n");
  std::printf("\t\tPercent of numbers to preload. The percent is out of\n");
  std::printf("\t\tthe total number of operations to be performed per\n");
  std::printf("\t\tthread.\n");
  std::printf("\t-m  --min-threads  <UNSIGNED>\n");
  std::printf("\t\tThe minimum number of threads to run. 1 by default.\n");
  std::printf("\t-x  --max-threads  <UNSIGNED>\n");
  std::printf("\t\tThe maximum number of threads to run. Number of\n");
  std::printf("\t\tvirtual cores on the machine is used by default.\n");
  std::printf("\t-f  --pretty\n");
  std::printf("\t\tOutputs the results in a more readable format.\n");
}

void usageErr(const char *name) {
  std::printf("Error: unexpected argument or option provided\n");
  usage(name);
  std::exit(EXIT_FAILURE);
}

void checkArgs(const RunnerParams &params) {
  assert(params.n);
  assert(params.inserts >= 0.0 and params.inserts <= 1.0);
  assert(params.removals >= 0.0 and params.removals <= 1.0);
  assert(params.lookups >= 0.0 and params.lookups <= 1.0);
  assert(params.preload >= 0.0 and params.preload <= 1.0);
  assert(params.minThreads >= 1 and params.minThreads <= params.maxThreads);
  auto total = params.inserts + params.removals + params.lookups;
  assert(std::fabs(1.0 - total) <= 0.01);
}

void printResults(const std::vector<RunnerResults> &results,
                  const RunnerParams &params, bool pretty = false) {
  if (not pretty) {
    std::cout << "list,cores,minThreads,maxThreads,n,inserts,removals,"
              << "lookups,scalingMode,withAffinity,preload,runtimes...\n";
    std::cout << std::boolalpha;
    std::copy(results.begin(), results.end(),
              std::ostream_iterator<RunnerResults>(std::cout, "\n"));
  } else {
    std::printf("Concurrency stats:\n");
    std::printf("\tcores=%u\n", params.nCores);
    std::printf("\tminThreads=%u\n", params.minThreads);
    std::printf("\tmaxThreads=%u\n", params.maxThreads);
    std::printf("\taffinity=%s\n", params.withAffinity ? "true" : "false");
    std::printf("Use-profile stats:\n");
    std::printf("\tn=%lu\n", params.n);
    std::printf("\tinserts=%.2f\n", params.inserts);
    std::printf("\tremovals=%.2f\n", params.removals);
    std::printf("\tlookups=%.2f\n", params.lookups);
    std::printf("\tpreload=%.2f\n", params.preload);
    for (auto &r : results) {
      std::printf("%s\n", r.name.c_str());
      auto j = params.minThreads;
      for (auto first = r.runTimes.begin(); first != r.runTimes.end(); ++first)
        std::printf("\t%u threads - %.5f seconds\n", j++, *first);
    }
  }
}

} // anonymous namespace
