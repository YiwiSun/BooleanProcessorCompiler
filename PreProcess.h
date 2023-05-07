#include <iostream>
#include <algorithm>
#include <metis.h>

#include "VCDTypes.h"

#ifndef PREPROCESS_H
#define PREPROCESS_H

using namespace std;

/**
 * @brief Generate 'Input INFO List' for Scheduling and Instruction Generation(IG)
 *
 * @param part
 * @param luts
 * @param dffs
 * @param nNodes
 * @param net_from_id
 * @param net_from_id_dff
 * @param pinbitValues
 * @param assign_pairs
 * @param vcd_values
 * @param interface_addr
 */
void PreProcess(vector<idx_t> &part,
                map<int, LutType> &luts, map<int, DffType> &dffs, int &nNodes,
                map<string, int> &net_from_id, map<string, int> &net_from_id_dff,
                map<string, TimedValues *> &pinbitValues, map<string, string> &assign_pairs, 
                unordered_map<string, vector<short> *> &vcd_values,
                map<string, pair<int, int>> &interface_addr);

#endif