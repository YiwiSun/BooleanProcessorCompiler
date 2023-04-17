#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <bitset>
#include <bits/stdc++.h>

#include "TopoSort.h"
#include "VCDTypes.h"

using namespace std;

/**
 * @brief Levelization via TopoSort with ASAP/ALAP
 *
 * @param n
 * @param edges
 * @return vector<vector<int>>
 */

vector<vector<int>> topoSortASAP(int n, vector<vector<int>> &edges)
{
    vector<int> inDegree(n, 0);
    vector<vector<int>> graph(n, vector<int>());
    for (auto &e : edges)
    {
        inDegree[e[1]]++;
        graph[e[0]].push_back(e[1]);
    }
    queue<int> q;
    for (int i = 0; i < n; i++)
    {
        if (inDegree[i] == 0)
            q.push(i);
    }
    vector<vector<int>> res;
    while (!q.empty())
    {
        int size = q.size();
        vector<int> level;
        for (int i = 0; i < size; i++)
        {
            int cur = q.front();
            q.pop();
            level.push_back(cur);
            for (auto &next : graph[cur])
            {
                if (--inDegree[next] == 0)
                    q.push(next);
            }
        }
        res.push_back(level);
    }

    // debug
    cout << "levels size (ASAP): " << res.size() << endl;
    for (auto l = res.begin(); l != res.end(); l++)
    {
        cout << "[Level " << distance(res.begin(), l) << "]" << endl;
        for (auto i = l->begin(); i != l->end(); i++)
        {
            cout << setw(7) << * i;
        }
        cout << endl;
    }
    cout << endl;


    return res;
}

vector<vector<int>> topoSortALAP(int n, vector<vector<int>> &edges)
{
    vector<int> outDegree(n, 0);
    vector<vector<int>> graph(n, vector<int>());
    for (auto &e : edges)
    {
        outDegree[e[0]]++;
        graph[e[1]].push_back(e[0]);
    }
    queue<int> q;
    for (int i = 0; i < n; i++)
    {
        if (outDegree[i] == 0)
            q.push(i);
    }
    vector<vector<int>> res;
    while (!q.empty())
    {
        int size = q.size();
        vector<int> level;
        for (int i = 0; i < size; i++)
        {
            int cur = q.front();
            q.pop();
            level.push_back(cur);
            for (auto &next : graph[cur])
            {
                if (--outDegree[next] == 0)
                    q.push(next);
            }
        }
        res.push_back(level);
    }
    reverse(res.begin(), res.end());

    // debug
    cout << "levels size (ALAP): " << res.size() << endl;
    for (auto l = res.begin(); l != res.end(); l++)
    {
        cout << "[Level " << distance(res.begin(), l) << "] " << endl;
        for (auto i = l->begin(); i != l->end(); i++)
        {
            cout << setw(7) << * i;
        }
        cout << endl;
    }
    cout << endl;
    

    return res;
}