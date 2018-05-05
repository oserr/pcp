/**
 * @file util.h
 *
 * Utility prototyes.
 */

#pragma once

#include <string>
#include <vector>

void setCoreAffinity(size_t coreId);

std::vector<std::string> split(const std::string &s, char delim);

std::string toLower(const std::string &s);
