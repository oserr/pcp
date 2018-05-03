/**
 * @file util.cpp
 *
 * Definitions for utility functions.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>
#include <thread>

#include "util.h"

void setCoreAffinity(size_t coreId) {
  coreId %= std::thread::hardware_concurrency();
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(coreId, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}
