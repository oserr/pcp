/**
 * @file lb.cpp
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
#include <thread>
#include <vector>

#include "coarse_grain_list.h"
#include "dllist.h"
#include "fine_grain_list.h"
#include "list_runner.h"
#include "lockfree_list.h"
#include "nonblocking_list.h"

namespace {
void usage(const char *name);
void usageErr(const char *name);
void checkArgs(const RunnerParams &params);
} // anonymous namespace

int main(int argc, char *argv[]) {
  constexpr int expectedArgs = 4;
  static struct option longOptions[] = {
      {"help", no_argument, nullptr, 'h'},
      {"numbers", required_argument, nullptr, 'n'},
      {"iserts", required_argument, nullptr, 'i'},
      {"removals", required_argument, nullptr, 'r'},
      {"lookups", required_argument, nullptr, 'l'},
      {"scaling", required_argument, nullptr, 's'},
      {"affinity", no_argument, nullptr, 'a'},
      {"preload", optional_argument, nullptr, 'p'},
      {0, 0, 0, 0}};
  int nArgs = 0;
  RunnerParams params;
  int opt;
  while ((opt = getopt_long(argc, argv, "hn:i:r:l:s:ap::", longOptions,
                            nullptr)) != -1) {
    switch (opt) {
    case 'h':
      usage(argv[0]);
      std::exit(EXIT_SUCCESS);
    case 'n':
      params.nPerThread = std::stoull(optarg);
      ++nArgs;
      break;
    case 'i':
      params.inserts = std::stof(optarg);
      ++nArgs;
      break;
    case 'r':
      params.removals = std::stof(optarg);
      ++nArgs;
      break;
    case 'l':
      params.lookups = std::stof(optarg);
      ++nArgs;
      break;
    case 's':
      switch (std::stoi(optarg)) {
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
      params.preload = std::strlen(optarg) ? std::stof(optarg) : 0.5;
      break;
    case '?':
    default:
      usageErr(argv[0]);
    }
  }

  assert(nArgs == expectedArgs);
  checkArgs(params);
  std::vector<RunnerResults> results;
  ListRunner runner(params);
  results.push_back(runner.RunSingle<DlList>("DlList"));
  results.push_back(runner.Run<CoarseGrainList>("CoarseGrainList"));
  results.push_back(runner.Run<FineGrainList>("FineGrainList"));
  results.push_back(runner.Run<NonBlockingList>("NonBlockingList"));
  results.push_back(runner.Run<LockFreeList>("LockFreeList"));

  std::cout << "list,n,inserts,removals,lookups,scalingMode,"
            << "withAffinity,preload,nThreads...\n";
  std::copy(results.begin(), results.end(),
            std::ostream_iterator<RunnerResults>(std::cout, "\n"));
  std::exit(EXIT_SUCCESS);
}

namespace {

void usage(const char *name) {
  std::printf("Usage: %s [options]\n", name);
  std::printf("Program Options:\n");
  std::printf("  -h  --help\n");
  std::printf("     Prints this help message.\n");
  std::printf("  -n  --numbers  <UNSIGNED>\n");
  std::printf("     The total number of operations to be performed.\n");
  std::printf("     With problem scaling, the number is fixed and gets\n");
  std::printf("     divided when more than one thread is running. With\n");
  std::printf("     memory scaling, this reresents the number of opertions\n");
  std::printf("     to be performed by each thread.\n");
  std::printf("  -i  --inserts  <FLOAT>\n");
  std::printf("     A number between 0 and 1 representing the percent of\n");
  std::printf("     insertions to be performed. The sum of i, r, and l\n");
  std::printf("     must be no less than 0.01 from 1.\n");
  std::printf("  -r  --removals <FLOAT>\n");
  std::printf("     A number between 0 and 1 representing the percent of\n");
  std::printf("     removals to be performed. The sum of i, r, and l must\n");
  std::printf("     be no less than 0.01 from 1.\n");
  std::printf("  -l  --lookups  <FLOAT>\n");
  std::printf("     A number between 0 and 1 representing the percent of\n");
  std::printf("     lookups to be performed. The sum of i, r, and l must\n");
  std::printf("     be no less than 0.01 from 1.\n");
  std::printf("  -s  --scaling  <p|P|m|M>\n");
  std::printf("     The scaling type, where p or P represent problem\n");
  std::printf("     scaling, and m or M represent memory scaling.\n");
  std::printf("  -a  --affinity\n");
  std::printf("     The threading affinity. This runs each thread on a\n");
  std::printf("     separate core. If the flag is not provided, then no\n");
  std::printf("     effort is made to schedule threads in their own cores.\n");
  std::printf("  -p  --preload  <FLOAT>\n");
  std::printf("     Percent of numbers to preload. The argument is\n");
  std::printf("     optional. If it is not provided, then .5 is used by\n");
  std::printf("     default.\n");
}

void usageErr(const char *name) {
  std::printf("Error: unexpected argument or option provided\n");
  usage(name);
  std::exit(EXIT_FAILURE);
}

void checkArgs(const RunnerParams &params) {
  assert(params.inserts >= 0.0 and params.inserts <= 1.0);
  assert(params.removals >= 0.0 and params.removals <= 1.0);
  assert(params.lookups >= 0.0 and params.lookups <= 1.0);
  assert(params.preload >= 0.0 and params.preload <= 1.0);
  auto total = params.inserts + params.removals + params.lookups;
  assert(std::fabs(1.0 - total) <= 0.01);
}

} // anonymous namespace
