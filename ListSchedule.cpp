#include <iostream>
#include <algorithm>
#include <vector>
#include <bitset>
#include <bits/stdc++.h>

#include "ListSchedule.h"
#include "VCDTypes.h"
#include "ArchDefine.h"

using namespace std;

/**
 * @brief Modified List Scheduling
 * @ref Hardware design and CAD for processor-based logic emulation systems.
 */

//! Instance a new Class
ListSch::ListSch() {

}

//! Destructor
ListSch::~ListSch() {

}

// Modified List Scheduling
vector<vector<int>> ListSch::MLS(vector<vector<int>> &levels_ASAP, vector<vector<int>> &levels_ALAP, vector<vector<int>> &edges,
                                 map<int, LutType> &luts, map<int, DffType> &dffs, int &nNode, int nParts, vector<idx_t> &part)
{
    vector<int> node_level_ASAP(nNode);
    vector<int> node_level_ALAP(nNode);
    vector<int> Fanout(nNode, 0);
    vector<int> done(nNode, 0);
    vector<int> CPN;                // Critical Path Nodes
    int CL = levels_ASAP.size();    // Critical Length
    vector<vector<int>> SchList(CL, vector<int> ());
    vector<vector<int>> nodes_in_per_bp(N_PROCESSORS, vector<int>());
    for (auto i = levels_ASAP.begin(); i != levels_ASAP.end(); i++)
    {
        for (auto j = i->begin(); j != i->end(); j++)
        {
            node_level_ASAP[*j] = distance(levels_ASAP.begin(), i);
        }
    } 
    for (auto i = levels_ALAP.begin(); i != levels_ALAP.end(); i++)
    {
        for (auto j = i->begin(); j != i->end(); j++)
        {
            node_level_ALAP[*j] = distance(levels_ALAP.begin(), i);
        }
    }
    for (auto n = 0; n < nNode; n++)
    {
        if (node_level_ASAP[n] == node_level_ALAP[n])
            CPN.push_back(n);
    }
    for (auto &e: edges) { Fanout[e[0]]++; }
    for (auto level = 0; level < CL; level++)
    {
        vector<int> ToSch;                     // nodes under scheduling
        vector<pair<int, int>> Vertex_MOB;     // node ID : mobility of schedulable nodes
        vector<pair<int, int>> Vertex_Fanout;  // node ID : fanout of schedulable nodes
        for (auto n = 0; n < nNode; n++)
        {
            if (done[n] == 0 && node_level_ASAP[n] <= level && node_level_ALAP[n] >= level)
                ToSch.push_back(n);
        }
        for (auto v : ToSch) { Vertex_MOB.push_back(make_pair(v, node_level_ALAP[v] - level)); }
        sort(Vertex_MOB.begin(), Vertex_MOB.end(), [](const pair<int, int> &a, const pair<int, int> &b)
             { return a.second < b.second; });
        vector<int> Max_Cycle(nParts, 0); 
        vector<int> BPSch(nParts, 0);
        while (!ToSch.empty())
        {
            for (auto v : Vertex_MOB)
            {
                if (find(CPN.begin(), CPN.end(), v.first) != CPN.end())
                {
                    int OK = allocate_and_collapse_IMM(v.first, Max_Cycle, 0, 1, done, SchList, level, luts, dffs, BPSch, part, nodes_in_per_bp);
                    ToSch.erase(remove(ToSch.begin(), ToSch.end(), v.first), ToSch.end());
                    if (OK)
                    {
                        SchList[level].push_back(v.first);
                        done[v.first] = 1;
                    }
                }
                else if (v.second == 0)
                {
                    int OK = allocate_and_collapse_IMM(v.first, Max_Cycle, 0, 1, done, SchList, level, luts, dffs, BPSch, part, nodes_in_per_bp);
                    ToSch.erase(remove(ToSch.begin(), ToSch.end(), v.first), ToSch.end());
                    if (OK)
                    {
                        SchList[level].push_back(v.first);
                        done[v.first] = 1;
                    }                  
                }
            }   
            for (auto v : ToSch) { Vertex_Fanout.push_back(make_pair(v, Fanout[v])); }
            sort(Vertex_Fanout.begin(), Vertex_Fanout.end(), [](const pair<int, int> &a, const pair<int, int> &b)
                 { return a.second > b.second; });
            int maxcycle = *max_element(Max_Cycle.begin(), Max_Cycle.end());
            for (auto v : Vertex_Fanout)
            {
                int OK = allocate_and_collapse_IMM(v.first, Max_Cycle, maxcycle, 0, done, SchList, level, luts, dffs, BPSch, part, nodes_in_per_bp);
                ToSch.erase(remove(ToSch.begin(), ToSch.end(), v.first), ToSch.end());
                if (OK)
                {
                    SchList[level].push_back(v.first);
                    done[v.first] = 1;
                }
            }
        }
    }
    if (count(done.begin(), done.end(), 1) == nNode)
        cout << "ListSchedule OK!" << endl;
    else
        cout << "ListSchedule ERROR!" << endl;

    
    // debug
    for (auto iter = nodes_in_per_bp.begin(); iter != nodes_in_per_bp.end(); iter++)
    {
        if (!(*iter).empty())
        {
            cout << "[Processor " << distance(nodes_in_per_bp.begin(), iter) << "]" << endl;
            for (auto node = iter->begin(); node != iter->end(); node++)
            {
                if (*node < luts.size())
                    cout << setw(7) << *node;
                else
                {
                    auto new_node = "*" + to_string(*node);
                    cout << setw(7) << new_node;
                }
            }
            cout << endl;
        }
    }

    FF_allocate(nodes_in_per_bp, luts, dffs);

    return SchList;
}

int ListSch::allocate_and_collapse_IMM(int &v, vector<int> &Max_Cycle, const int &maxcycle, const int &type, vector<int> &done, vector<vector<int>> &SchList, int &level,
                                       map<int, LutType> &luts, map<int, DffType> &dffs, vector<int> &BPSch, vector<idx_t> &part, vector<vector<int>> &nodes_in_per_bp)
{
    int cur_part = part[v];
    int cur_BP = BPSch[cur_part];
    if (!type && Max_Cycle[cur_part] == (maxcycle + 1))
    {
        return 0;
    }
    if (cur_BP == N_PROCESSORS_PER_CLUSTER - 1)
    {
        BPSch[cur_part] = 0;
        Max_Cycle[cur_part]++;
    }
    else
    {
        BPSch[cur_part]++;
    }
    if (v < luts.size())
    {
        for (int i = 0; i < luts[v].in_net_from_type.size(); i++)
        {
            if (luts[v].in_net_from_type[i] == 0)
            {
                auto iter = find(SchList[level].begin(), SchList[level].end(), luts[v].in_net_from_id[i]);
                if (!done[luts[v].in_net_from_id[i]] || iter != SchList[level].end())
                    {return 0;}
            }
        }
        luts[v].node_addr = make_pair(cur_part, cur_BP);
        nodes_in_per_bp[cur_part * N_PROCESSORS_PER_CLUSTER + cur_BP].push_back(v);
    }
    else
    {
        for (int i = 0; i < dffs[v - luts.size()].in_net_from_type.size(); i++)
        {
            if (dffs[v - luts.size()].in_net_from_type[i] == 0)
            {
                auto iter = find(SchList[level].begin(), SchList[level].end(), dffs[v - luts.size()].in_net_from_id[i]);
                if (!done[dffs[v - luts.size()].in_net_from_id[i]] || iter != SchList[level].end())
                    {return 0;}
            }
        }
        dffs[v - luts.size()].node_addr = make_pair(cur_part, cur_BP);
        nodes_in_per_bp[cur_part * N_PROCESSORS_PER_CLUSTER + cur_BP].push_back(v);
    }
    return 1;
}

void ListSch::FF_allocate(vector<vector<int>> &nodes_in_per_bp, map<int, LutType> &luts, map<int, DffType> &dffs)
{
    for (auto iter = nodes_in_per_bp.begin(); iter != nodes_in_per_bp.end(); iter++)
    {
        if (!(*iter).empty())
        {
            int addr {0};
             for (auto node = iter->begin(); node != iter->end(); node++) 
            {
                if (*node >= luts.size())
                {
                    dffs[*node - luts.size()].FF_Datamem_Addr = addr;
                    addr++;
                }
            }
        }
    }
}