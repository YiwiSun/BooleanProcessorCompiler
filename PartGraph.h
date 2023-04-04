#include <iostream>
#include <algorithm>
#include <metis.h>

#include "VCDTypes.h"

#ifndef PARTGRAPH_H
#define PARTGRAPH_h

using namespace std;

/**
 * @brief Coarse-grained partition using METIS API
 *
 */
class Part {
public:
    //! Instance a new Class
    Part();

    //! Destructor
    ~Part();

    void Partition(map<int, LutType> &luts, map<int, DffType> &dffs,
                   map<std::string, int> &net_from_id, map<std::string, int> &net_from_id_dff, map<std::string, std::string> &assign_pairs, 
                   vector<idx_t> &part, vector<vector<int>> &edges);

    vector<idx_t> part_func(vector<idx_t> &xadj, vector<idx_t> &adjncy,
                            /*vector<idx_t> &adjwgt, */ decltype(METIS_PartGraphKway) *METIS_PartGraphFunc);
};

#endif