/**
 * @file util.cpp
 *
 * Definitions for utility functions.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <pthread.h>

#include <cctype>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "util.h"

void setCoreAffinity(size_t coreId) {
  coreId %= std::thread::hardware_concurrency();
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(coreId, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::istringstream iss(s);
  std::vector<std::string> splits;
  for (std::string word; std::getline(iss, word, delim);)
    splits.emplace_back(std::move(word));
  return splits;
}

std::string toLower(const std::string &s) {
  auto word(s);
  for (auto &c : word)
    c = std::tolower(c);
  return word;
}
