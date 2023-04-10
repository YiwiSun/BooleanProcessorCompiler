#include <iostream>
#include <algorithm>
#include <metis.h>

#include "PartGraph.h"
#include "ArchDefine.h"
#include "VCDTypes.h"

using namespace std;

/**
 * @brief Coarse-grained partition using METIS API
 * 
 */

//! Instance a new Class
Part::Part() {

}

//! Destructor
Part::~Part() {

}

void Part::Partition(map<int, LutType> &luts, map<int, DffType> &dffs,
                map<std::string, int> &net_from_id, map<std::string, int> &net_from_id_dff,
                map<std::string, std::string> &assign_pairs, vector<idx_t> &part, vector<vector<int>> &edges)
{
    int edge_num = 0;
    vector<vector<int>> adjncys(luts.size() + dffs.size());
    auto total_size = luts.size() + dffs.size();
    for (int i = 0; i < total_size; ++i)
    {
        if (i < luts.size())
        {
            LutType cur_lut = luts[i];
            std::vector<string> _in_net = cur_lut.in_ports;
            for (unsigned j = 0; j < _in_net.size(); ++j)
            {
                string cur_in = _in_net[j];
                if (net_from_id.find(cur_in) != net_from_id.end())
                {
                    edge_num += 1;
                    int _id = net_from_id[cur_in];
                    adjncys[luts[i].num].push_back(_id);
                    adjncys[_id].push_back(luts[i].num);
                    edges.push_back({_id, i});
                }
                else if (net_from_id_dff.find(cur_in) != net_from_id_dff.end())
                {
                    edge_num += 1;
                    int _id = net_from_id_dff[cur_in];
                    adjncys[luts[i].num].push_back(_id + luts.size());
                    adjncys[_id + luts.size()].push_back(luts[i].num);
                }
                else if (assign_pairs.find(cur_in) != assign_pairs.end())
                {
                    string temp = assign_pairs[cur_in];
                    if (net_from_id.find(temp) != net_from_id.end())
                    {
                        edge_num += 1;
                        int _id = net_from_id[temp];
                        adjncys[luts[i].num].push_back(_id);
                        adjncys[_id].push_back(luts[i].num);
                        edges.push_back({_id, i});
                    }
                    else if (net_from_id_dff.find(temp) != net_from_id_dff.end())
                    {
                        edge_num += 1;
                        int _id = net_from_id_dff[temp];
                        adjncys[luts[i].num].push_back(_id + luts.size());
                        adjncys[_id + luts.size()].push_back(luts[i].num);
                    }
                }
            }
        }
        else
        {
            DffType cur_dff = dffs[i - luts.size()];
            std::vector<string> _in_net = cur_dff.dff_in_ports;
            for (unsigned j = 0; j < _in_net.size(); ++j)
            {
                string cur_in = _in_net[j];
                if (net_from_id.find(cur_in) != net_from_id.end())
                {
                    edge_num += 1;
                    int _id = net_from_id[cur_in];
                    adjncys[dffs[i - luts.size()].num + luts.size()].push_back(_id);
                    adjncys[_id].push_back(dffs[i - luts.size()].num + luts.size());
                    edges.push_back({_id, i});
                }
                else if (net_from_id_dff.find(cur_in) != net_from_id_dff.end())
                {
                    edge_num += 1;
                    int _id = net_from_id_dff[cur_in];
                    adjncys[dffs[i - luts.size()].num + luts.size()].push_back(_id + luts.size());
                    adjncys[_id + luts.size()].push_back(dffs[i - luts.size()].num + luts.size());
                }
                else if (assign_pairs.find(cur_in) != assign_pairs.end())
                {
                    string temp = assign_pairs[cur_in];
                    if (net_from_id.find(temp) != net_from_id.end())
                    {
                        edge_num += 1;
                        int _id = net_from_id[temp];
                        adjncys[dffs[i - luts.size()].num + luts.size()].push_back(_id);
                        adjncys[_id].push_back(dffs[i - luts.size()].num + luts.size());
                        edges.push_back({_id, i});
                    }
                    else if (net_from_id_dff.find(temp) != net_from_id_dff.end())
                    {
                        edge_num += 1;
                        int _id = net_from_id_dff[temp];
                        adjncys[dffs[i - luts.size()].num + luts.size()].push_back(_id + luts.size());
                        adjncys[_id + luts.size()].push_back(dffs[i - luts.size()].num + luts.size());
                    }
                }
            }
        }
    }
    idx_t nVertices = luts.size() + dffs.size();
    idx_t nEdges = edge_num;
    vector<vector<idx_t>> nAdjncys = adjncys;

    // generation the adjacency structure of the graph
    vector<idx_t> xadj(0);
    vector<idx_t> adjncy(0);
    for (int i = 0; i < nAdjncys.size(); i++)
    {
        xadj.push_back(adjncy.size()); // csr: row offsets (https://www.cnblogs.com/xbinworld/p/4273506.html)
        for (int j = 0; j < nAdjncys[i].size(); j++)
        {
            adjncy.push_back(nAdjncys[i][j]);
        }
    }
    xadj.push_back(adjncy.size());
    assert(xadj.size() == (nVertices + 1));
    assert(adjncy.size() == (nEdges * 2));
    part = part_func(xadj, adjncy, METIS_PartGraphRecursive);
}

vector<idx_t> Part::part_func(vector<idx_t> &xadj, vector<idx_t> &adjncy, /*vector<idx_t> &adjwgt, */ decltype(METIS_PartGraphKway) *METIS_PartGraphFunc)
{
    idx_t nVertices = xadj.size() - 1;
    idx_t nEdges = adjncy.size() / 2;
    idx_t nWeights = 1;
    idx_t nParts = (nVertices + N_PROCESSORS_PER_CLUSTER * N_LUTS_PER_PROCESSOR - 1) / (N_PROCESSORS_PER_CLUSTER * N_LUTS_PER_PROCESSOR);
    assert(nParts <= MAX_CLUSTERS);
    idx_t objval;
    vector<idx_t> part(nVertices, 0);
    int ret = METIS_PartGraphFunc(&nVertices, &nWeights, xadj.data(), adjncy.data(),
                                  NULL, NULL, NULL, &nParts, NULL,
                                  NULL, NULL, &objval, part.data());
    if (ret != rstatus_et::METIS_OK)
    {
        std::cout << "METIS ERROR!" << endl;
    }
    std::cout << "METIS OK!" << endl;
    std::cout << "parts size: " << nParts << endl;

    if (nParts == 1 && part[0] == 1)
    {
        for (int i = 0; i < part.size(); i++)
        {
            part[i] -= 1;
        }
    }

    return part;
}