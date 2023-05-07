#include <iostream>
#include <algorithm>
#include <metis.h>

#include "PreProcess.h"
#include "ArchDefine.h"
#include "InstrGen.h"
#include "VCDTypes.h"
#include "Compile.h"

using namespace std;

/**
 * @brief Generate 'Input INFO List' for Scheduling and Instruction Generation(IG)
 *
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
                map<string, pair<int, int>> &interface_addr)
{
    for (auto n = 0; n < nNodes; n++)
    {
        if (n < luts.size())
        {
            LutType cur_lut = luts[n];
            std::vector<string> _in_net = cur_lut.in_ports;
            for (unsigned j = 0; j < _in_net.size(); ++j)
            {
                string cur_in = _in_net[j];
                if (net_from_id.find(cur_in) != net_from_id.end())
                {
                    int _id = net_from_id[cur_in];
                    luts[n].in_net_from_type.push_back(0);
                    luts[n].in_net_from_id.push_back(_id);
                    luts[n].in_net_from_info.push_back(" ");
                    if (part[_id] == part[n])
                        luts[n].in_net_from_part.push_back(-3);
                    else
                        luts[n].in_net_from_part.push_back(part[_id]);
                    luts[n].in_net_from_addr.push_back(make_pair(-1,-1));
                    luts[n].in_net_from_ready.push_back(0);
                }
                else if (net_from_id_dff.find(cur_in) != net_from_id_dff.end())
                {
                    int _id = net_from_id_dff[cur_in] + luts.size();
                    luts[n].in_net_from_type.push_back(1);
                    luts[n].in_net_from_id.push_back(_id);
                    luts[n].in_net_from_info.push_back(" ");
                    if (part[_id] == part[n])
                        luts[n].in_net_from_part.push_back(-3);                  
                    else                  
                        luts[n].in_net_from_part.push_back(part[_id]);
                    luts[n].in_net_from_addr.push_back(make_pair(-1, -1));
                    luts[n].in_net_from_ready.push_back(0);
                }
                else if (pinbitValues.find(cur_in) != pinbitValues.end())
                {
                    TimedValues *tvs = pinbitValues[cur_in];
                    int cur_val = (*(tvs->begin())).value;
                    // at this case, 'in net from id' means value
                    luts[n].in_net_from_type.push_back(2);
                    luts[n].in_net_from_id.push_back(cur_val == 0 ? -2 : -1);
                    luts[n].in_net_from_info.push_back(cur_in);
                    luts[n].in_net_from_part.push_back(-1);
                    if (cur_val == 0)
                        luts[n].in_net_from_addr.push_back(make_pair(Inter_Datamem, MEM_DEPTH - 1));
                    else
                        luts[n].in_net_from_addr.push_back(make_pair(Exter_Datamem_0, MEM_DEPTH - 1));
                    luts[n].in_net_from_ready.push_back(1);
                }
                else if (interface_addr.find(cur_in) != interface_addr.end())
                {
                    luts[n].in_net_from_type.push_back(4);
                    luts[n].in_net_from_id.push_back(-3);
                    luts[n].in_net_from_info.push_back(cur_in);
                    luts[n].in_net_from_part.push_back(-1);
                    luts[n].in_net_from_addr.push_back(interface_addr[cur_in]);
                    luts[n].in_net_from_ready.push_back(1);
                }
                else if (assign_pairs.find(cur_in) != assign_pairs.end())
                {
                    string temp = assign_pairs[cur_in];
                    if (net_from_id.find(temp) != net_from_id.end())
                    {
                        int _id = net_from_id[temp];
                        luts[n].in_net_from_type.push_back(0);
                        luts[n].in_net_from_id.push_back(_id);
                        luts[n].in_net_from_info.push_back(" ");
                        if (part[_id] == part[n])
                            luts[n].in_net_from_part.push_back(-3);
                        else
                            luts[n].in_net_from_part.push_back(part[_id]);
                        luts[n].in_net_from_addr.push_back(make_pair(-1, -1));
                        luts[n].in_net_from_ready.push_back(0);
                    }
                    else if (net_from_id_dff.find(temp) != net_from_id_dff.end())
                    {
                        int _id = net_from_id_dff[temp] + luts.size();
                        luts[n].in_net_from_type.push_back(1);
                        luts[n].in_net_from_id.push_back(_id);
                        luts[n].in_net_from_info.push_back(" ");
                        if (part[_id] == part[n])
                            luts[n].in_net_from_part.push_back(-3);
                        else
                            luts[n].in_net_from_part.push_back(part[_id]);
                        luts[n].in_net_from_addr.push_back(make_pair(-1, -1));
                        luts[n].in_net_from_ready.push_back(0);
                    }
                    else if (pinbitValues.find(temp) != pinbitValues.end())
                    {
                        TimedValues *tvs = pinbitValues[temp];
                        int cur_val = (*(tvs->begin())).value;
                        // at this case, 'in net from id' means value
                        luts[n].in_net_from_type.push_back(2);
                        luts[n].in_net_from_id.push_back(cur_val == 0 ? -2 : -1);
                        luts[n].in_net_from_info.push_back(temp);
                        luts[n].in_net_from_part.push_back(-1);
                        if (cur_val == 0)
                            luts[n].in_net_from_addr.push_back(make_pair(Inter_Datamem, MEM_DEPTH - 1));
                        else
                            luts[n].in_net_from_addr.push_back(make_pair(Exter_Datamem_0, MEM_DEPTH - 1));
                        luts[n].in_net_from_ready.push_back(1);
                    }
                    else if (interface_addr.find(temp) != interface_addr.end())
                    {
                        luts[n].in_net_from_type.push_back(4);
                        luts[n].in_net_from_id.push_back(-3);
                        luts[n].in_net_from_info.push_back(temp);
                        luts[n].in_net_from_part.push_back(-1);
                        luts[n].in_net_from_addr.push_back(interface_addr[temp]);
                        luts[n].in_net_from_ready.push_back(1);
                    }
                    else if (vcd_values.find(temp) != vcd_values.end())
                    {
                        vector<short> *tvs_val = vcd_values[temp];
                        auto cur_val = *(tvs_val->begin());
                        // at this case, 'in net from id' means value
                        luts[n].in_net_from_type.push_back(3);
                        luts[n].in_net_from_id.push_back(cur_val == 0 ? -2 : -1);
                        luts[n].in_net_from_info.push_back(temp);
                        luts[n].in_net_from_part.push_back(-1);
                        if (cur_val == 0)
                        {
                            if (temp == INITIAL_JUMP_SIG)
                                luts[n].in_net_from_addr.push_back(make_pair(Inter_Datamem, INITIAL_JUMP_ADDR));
                            else
                                luts[n].in_net_from_addr.push_back(make_pair(Inter_Datamem, MEM_DEPTH - 1));
                        }
                        else
                        {
                            if (temp == INITIAL_JUMP_SIG)
                                luts[n].in_net_from_addr.push_back(make_pair(Exter_Datamem_0, INITIAL_JUMP_ADDR));
                            else
                                luts[n].in_net_from_addr.push_back(make_pair(Exter_Datamem_0, MEM_DEPTH - 1));
                        }
                        luts[n].in_net_from_ready.push_back(1);
                    }
                    else
                    {
                        cout << "ERROR: No INFO of Input Signal " << temp << "! (LUT " << n << ")" << endl;
                    }
                }
                else if (vcd_values.find(cur_in) != vcd_values.end())
                {
                    vector<short> *tvs_val = vcd_values[cur_in];
                    auto cur_val = *(tvs_val->begin());
                    // at this case, 'in net from id' means value
                    luts[n].in_net_from_type.push_back(3);
                    luts[n].in_net_from_id.push_back(cur_val == 0 ? -2 : -1);
                    luts[n].in_net_from_info.push_back(cur_in);
                    luts[n].in_net_from_part.push_back(-1);
                    if (cur_val == 0)
                    {
                        if (cur_in == INITIAL_JUMP_SIG)
                            luts[n].in_net_from_addr.push_back(make_pair(Inter_Datamem, INITIAL_JUMP_ADDR));
                        else
                            luts[n].in_net_from_addr.push_back(make_pair(Inter_Datamem, MEM_DEPTH - 1));
                    }
                    else
                    {
                        if (cur_in == INITIAL_JUMP_SIG)
                            luts[n].in_net_from_addr.push_back(make_pair(Exter_Datamem_0, INITIAL_JUMP_ADDR));
                        else
                            luts[n].in_net_from_addr.push_back(make_pair(Exter_Datamem_0, MEM_DEPTH - 1));
                    }
                    luts[n].in_net_from_ready.push_back(1);
                }
                else
                {
                    cout << "ERROR: No INFO of Input Signal " << cur_in << "! (LUT " << n << ")" << endl;
                }
            }
        }
        else
        {
            int dff_num = n - luts.size();
            DffType cur_dff = dffs[dff_num];
            vector<string> _in_net = cur_dff.dff_in_ports;
            for (unsigned j = 0; j < _in_net.size(); ++j)
            {
                string cur_in = _in_net[j];
                if (net_from_id.find(cur_in) != net_from_id.end())
                {
                    int _id = net_from_id[cur_in];
                    dffs[dff_num].in_net_from_type.push_back(0);
                    dffs[dff_num].in_net_from_id.push_back(_id);
                    dffs[dff_num].in_net_from_info.push_back(" ");
                    if (part[_id] == part[n])
                        dffs[dff_num].in_net_from_part.push_back(-3);
                    else
                        dffs[dff_num].in_net_from_part.push_back(part[_id]);
                    dffs[dff_num].in_net_from_addr.push_back(make_pair(-1, -1));
                    dffs[dff_num].in_net_from_ready.push_back(0);
                }
                else if (net_from_id_dff.find(cur_in) != net_from_id_dff.end())
                {
                    int _id = net_from_id_dff[cur_in] + luts.size();
                    dffs[dff_num].in_net_from_type.push_back(1);
                    dffs[dff_num].in_net_from_id.push_back(_id);
                    dffs[dff_num].in_net_from_info.push_back(" ");
                    if (part[_id] == part[n])
                        dffs[dff_num].in_net_from_part.push_back(-3);
                    else
                        dffs[dff_num].in_net_from_part.push_back(part[_id]);
                    dffs[dff_num].in_net_from_addr.push_back(make_pair(-1, -1));
                    dffs[dff_num].in_net_from_ready.push_back(0);
                }
                else if (pinbitValues.find(cur_in) != pinbitValues.end())
                {
                    TimedValues *tvs = pinbitValues[cur_in];
                    int cur_val = (*(tvs->begin())).value;
                    // at this case, 'in net from id' means value
                    dffs[dff_num].in_net_from_type.push_back(2);
                    dffs[dff_num].in_net_from_id.push_back(cur_val == 0 ? -2 : -1);
                    dffs[dff_num].in_net_from_info.push_back(cur_in);
                    dffs[dff_num].in_net_from_part.push_back(-1);
                    if (cur_val == 0)
                        dffs[dff_num].in_net_from_addr.push_back(make_pair(Inter_Datamem, MEM_DEPTH - 1));
                    else
                        dffs[dff_num].in_net_from_addr.push_back(make_pair(Exter_Datamem_0, MEM_DEPTH - 1));
                    dffs[dff_num].in_net_from_ready.push_back(1);
                }
                else if (interface_addr.find(cur_in) != interface_addr.end())
                {
                    dffs[dff_num].in_net_from_type.push_back(4);
                    dffs[dff_num].in_net_from_id.push_back(-3);
                    dffs[dff_num].in_net_from_info.push_back(cur_in);
                    dffs[dff_num].in_net_from_part.push_back(-1);
                    dffs[dff_num].in_net_from_addr.push_back(interface_addr[cur_in]);
                    dffs[dff_num].in_net_from_ready.push_back(1);
                }
                else if (assign_pairs.find(cur_in) != assign_pairs.end())
                {
                    string temp = assign_pairs[cur_in];
                    if (net_from_id.find(temp) != net_from_id.end())
                    {
                        int _id = net_from_id[temp];
                        dffs[dff_num].in_net_from_type.push_back(0);
                        dffs[dff_num].in_net_from_id.push_back(_id);
                        dffs[dff_num].in_net_from_info.push_back(" ");
                        if (part[_id] == part[n])
                            dffs[dff_num].in_net_from_part.push_back(-3);
                        else
                            dffs[dff_num].in_net_from_part.push_back(part[_id]);
                        dffs[dff_num].in_net_from_addr.push_back(make_pair(-1, -1));
                        dffs[dff_num].in_net_from_ready.push_back(0);
                    }
                    else if (net_from_id_dff.find(temp) != net_from_id_dff.end())
                    {
                        int _id = net_from_id_dff[temp] + luts.size();
                        dffs[dff_num].in_net_from_type.push_back(1);
                        dffs[dff_num].in_net_from_id.push_back(_id);
                        dffs[dff_num].in_net_from_info.push_back(" ");
                        if (part[_id] == part[n])
                            dffs[dff_num].in_net_from_part.push_back(-3);
                        else
                            dffs[dff_num].in_net_from_part.push_back(part[_id]);
                        dffs[dff_num].in_net_from_addr.push_back(make_pair(-1, -1));
                        dffs[dff_num].in_net_from_ready.push_back(0);
                    }
                    else if (pinbitValues.find(temp) != pinbitValues.end())
                    {
                        TimedValues *tvs = pinbitValues[temp];
                        int cur_val = (*(tvs->begin())).value;
                        // at this case, 'in net from id' means value
                        dffs[dff_num].in_net_from_type.push_back(2);
                        dffs[dff_num].in_net_from_id.push_back(cur_val == 0 ? -2 :-1);
                        dffs[dff_num].in_net_from_info.push_back(temp);
                        dffs[dff_num].in_net_from_part.push_back(-2);
                        if (cur_val == 0)
                            dffs[dff_num].in_net_from_addr.push_back(make_pair(Inter_Datamem, MEM_DEPTH - 1));
                        else
                            dffs[dff_num].in_net_from_addr.push_back(make_pair(Exter_Datamem_0, MEM_DEPTH - 1));
                        dffs[dff_num].in_net_from_ready.push_back(1);
                    }
                    else if (interface_addr.find(temp) != interface_addr.end())
                    {
                        dffs[dff_num].in_net_from_type.push_back(4);
                        dffs[dff_num].in_net_from_id.push_back(-3);
                        dffs[dff_num].in_net_from_info.push_back(temp);
                        dffs[dff_num].in_net_from_part.push_back(-1);
                        dffs[dff_num].in_net_from_addr.push_back(interface_addr[temp]);
                        dffs[dff_num].in_net_from_ready.push_back(1);
                    }
                    else if (vcd_values.find(temp) != vcd_values.end())
                    {
                        vector<short> *tvs_val = vcd_values[temp];
                        auto cur_val = *(tvs_val->begin());
                        // at this case, 'in net from id' means value
                        dffs[dff_num].in_net_from_type.push_back(3);
                        dffs[dff_num].in_net_from_id.push_back(cur_val == 0 ? -2 : -1);
                        dffs[dff_num].in_net_from_info.push_back(temp);
                        dffs[dff_num].in_net_from_part.push_back(-1);
                        if (cur_val == 0)
                        {
                            if (temp == INITIAL_JUMP_SIG)
                                dffs[dff_num].in_net_from_addr.push_back(make_pair(Inter_Datamem, INITIAL_JUMP_ADDR));
                            else
                                dffs[dff_num].in_net_from_addr.push_back(make_pair(Inter_Datamem, MEM_DEPTH - 1));
                        }
                        else
                        {
                            if (temp == INITIAL_JUMP_SIG)
                                dffs[dff_num].in_net_from_addr.push_back(make_pair(Exter_Datamem_0, INITIAL_JUMP_ADDR));
                            else
                                dffs[dff_num].in_net_from_addr.push_back(make_pair(Exter_Datamem_0, MEM_DEPTH - 1));
                        }
                        dffs[dff_num].in_net_from_ready.push_back(1);
                    }
                    else
                    {
                        cout << "ERROR: No INFO of Input Signal " << temp << "! (DFF " << dff_num << ")" << endl;
                    }
                }
                else if (vcd_values.find(cur_in) != vcd_values.end())
                {
                    vector<short> *tvs_val = vcd_values[cur_in];
                    auto cur_val = *(tvs_val->begin());
                    // at this case, 'in net from id' means value
                    dffs[dff_num].in_net_from_type.push_back(3);
                    dffs[dff_num].in_net_from_id.push_back(cur_val == 0 ? -2 : -1);
                    dffs[dff_num].in_net_from_info.push_back(cur_in);
                    dffs[dff_num].in_net_from_part.push_back(-1);
                    if (cur_val == 0)
                    {
                        if (cur_in == INITIAL_JUMP_SIG)
                            dffs[dff_num].in_net_from_addr.push_back(make_pair(Inter_Datamem, INITIAL_JUMP_ADDR));
                        else
                            dffs[dff_num].in_net_from_addr.push_back(make_pair(Inter_Datamem, MEM_DEPTH - 1));
                    }
                    else
                    {
                        if (cur_in == INITIAL_JUMP_SIG)
                            dffs[dff_num].in_net_from_addr.push_back(make_pair(Exter_Datamem_0, INITIAL_JUMP_ADDR));
                        else
                            dffs[dff_num].in_net_from_addr.push_back(make_pair(Exter_Datamem_0, MEM_DEPTH - 1));
                    }     
                    dffs[dff_num].in_net_from_ready.push_back(1);
                }
                else
                {
                    cout << "ERROR: No INFO of Input Signal " << cur_in << "! (DFF " << dff_num << ")" << endl;
                }
            }
        }
    }
}
