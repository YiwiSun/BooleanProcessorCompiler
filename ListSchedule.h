#include <iostream>
#include <algorithm>
#include <vector>
#include <metis.h>

#include "VCDTypes.h"
#include "ArchDefine.h"

#ifndef LISTSCHEDULE_H
#define LISTSCHEDULE_H

using namespace std;

/**
 * @brief Modified List Scheduling
 * @ref Hardware design and CAD for processor-based logic emulation systems.
 */

class ListSch {
public:
    //! Instance a new Class
    ListSch();

    //! Destructor
    ~ListSch();

    vector<vector<int>> nodes_in_per_bp(N_PROCESSORS, vector<int>());

    // Modified List Scheduling
    vector<vector<int>> MLS(vector<vector<int>> &levels_ASAP, vector<vector<int>> &levels_ALAP, vector<vector<int>> &edges,
                            map<int, LutType> &luts, map<int, DffType> &dffs, int &nNode, int nParts, vector<idx_t> &part);
    int allocate_and_collapse_IMM(int &v, vector<int> &Max_Cycle, const int &maxcycle, const int &type, map<int, LutType> &luts, map<int, DffType> &dffs,
                                  vector<int> &BPSch, vector<idx_t> &part);
    void FF_allocate(vector<vector<int>> &nodes_in_per_bp, map<int, LutType> &luts, map<int, DffType> &dffs);
};

#endif