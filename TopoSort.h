#include <iostream>
#include <algorithm>
#include <vector>

#include "VCDTypes.h"

#ifndef TOPOSORT_H
#define TOPOSORT_H

using namespace std;

/**
 * @brief Levelization via TopoSort with ASAP/ALAP
 * 
 * @param n 
 * @param edges 
 * @return vector<vector<int>> 
 */
vector<vector<int>> topoSortASAP(int n, vector<vector<int>> &edges);
vector<vector<int>> topoSortALAP(int n, vector<vector<int>> &edges);

#endif