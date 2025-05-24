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
#include <ctime>
#include <cstdlib>
#include <queue> 

std::vector<std::size_t> parsePath(const std::string& s, char sep = '/');

void createRandomTree(std::vector<NAryTree<int>*>& objs);

void findEl(std::vector<NAryTree<int>*>& objs);

template<typename T>
void printTree(const NAryTree<T>& tr);

void runUI();