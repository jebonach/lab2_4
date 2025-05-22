#pragma once
#include "N-aryTree.hpp"
#include "errors.hpp"
#include <string>
#include <vector>
#include <queue>
#include <sstream>
#include <iostream>
#include <limits>
#include <cmath>

std::vector<std::size_t> parsePath(const std::string& s, char sep = '/');

template<typename T>
void printTree(const NAryTree<T>& tr);

void runUI();