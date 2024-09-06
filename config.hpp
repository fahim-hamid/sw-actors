#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "caf/all.hpp"
#include <vector>
#include <string>
#include <chrono>
#include <utility>
#include <algorithm>

// include the necessary headers
#include "readFasta.hpp"
#include "makePairs.hpp"

// Declare the global variables
std::vector<std::string> sequences;
std::vector<std::pair<int, int>> pairs;
std::vector<int> workList1;
std::vector<int> workList2;
auto start = std::chrono::high_resolution_clock::now();

#endif // CONFIG_HPP