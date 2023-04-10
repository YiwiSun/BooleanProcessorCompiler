#include <iostream>
#include <algorithm>
#include <string>
#include <bitset>
#include <bits/stdc++.h>

#include "InstrGen.h"
#include "ArchDefine.h"
#include "VCDTypes.h"

using namespace std;

vector<vector<Instr>> InstrGen(vector<vector<int>> &SchList, map<int, LutType> &luts, map<int, DffType> &dffs,
                               map<string, vector<int>> &net_for_id, map<string, vector<int>> &net_for_id_dff)
{

    // Transmit FF data in advance
    for (int n_dff = 0; n_dff < dffs.size(); n_dff++)
    {
        DffType cur_dff = dffs[n_dff];
        auto cur_node_addr = cur_dff.node_addr;
        auto tt_instr_mem_index = cur_node_addr.first * N_PROCESSORS_PER_CLUSTER + cur_node_addr.second;
        auto cur_instr_mem_cnt  = tt_instr_mem_cnt[tt_instr_mem_index];
        Instr new_instr;
        new_instr.Opcode = STATIC_CONFIG;
        new_instr.Jump   = 00;
        new_instr.Datamem_Sel.push_back(FF_Datamem);
        new_instr.Operand_Addr.push_back(dffs[n_dff].FF_Datamem_Addr);
        tt_instr_mem[tt_instr_mem_index][cur_instr_mem_cnt] = new_instr;
        tt_instr_mem_cnt[tt_instr_mem_index] += 1;

        auto cur_node_for_id = net_for_id[cur_dff.dff_out];
        for (auto i = cur_node_for_id.begin(); i != cur_node_for_id.end(); i++)
        {
            if (*i < luts.size())
            {
                auto for_node_addr = luts[*i].node_addr;
                auto for_node_addr_num = for_node_addr.first * N_PROCESSORS_PER_CLUSTER + for_node_addr.second;
                if (for_node_addr == cur_node_addr)
                {
                    auto iter = find(luts[*i].in_net_from_id.begin(), luts[*i].in_net_from_id.end(), n_dff + luts.size());
                    luts[*i].in_net_from_addr[distance(luts[*i].in_net_from_id.begin(), iter)] = make_pair(FF_Datamem, dffs[n_dff].FF_Datamem_Addr);
                    luts[*i].in_net_from_ready[distance(luts[*i].in_net_from_id.begin(), iter)] = 1;
                }
                else
                {
                    if (tt_instr_mem[for_node_addr_num][cur_instr_mem_cnt].Node_Addr.size() < 4)
                    {
                        tt_instr_mem[for_node_addr_num][cur_instr_mem_cnt].Node_Addr.push_back(tt_instr_mem_index);
                        int n = tt_instr_mem[for_node_addr_num][cur_instr_mem_cnt].Node_Addr.size();
                        auto iter = find(luts[*i].in_net_from_id.begin(), luts[*i].in_net_from_id.end(), n_dff + luts.size());
                        luts[*i].in_net_from_addr[distance(luts[*i].in_net_from_id.begin(), iter)] = make_pair(Sel_Exter_Datamem(n), cur_instr_mem_cnt);
                        luts[*i].in_net_from_ready[distance(luts[*i].in_net_from_id.begin(), iter)] = 1;
                    }
                    else
                    {
                        for (int offset = 1;; offset++)
                        {
                            if (tt_instr_mem[for_node_addr_num][cur_instr_mem_cnt + offset].Node_Addr.size() < 4)
                            {
                                tt_instr_mem[for_node_addr_num][cur_instr_mem_cnt + offset].Node_Addr.push_back(tt_instr_mem_index);
                                int n = tt_instr_mem[for_node_addr_num][cur_instr_mem_cnt + offset].Node_Addr.size();
                                auto iter = find(luts[*i].in_net_from_id.begin(), luts[*i].in_net_from_id.end(), n_dff + luts.size());
                                luts[*i].in_net_from_addr[distance(luts[*i].in_net_from_id.begin(), iter)] = make_pair(Sel_Exter_Datamem(n), cur_instr_mem_cnt + offset);
                                luts[*i].in_net_from_ready[distance(luts[*i].in_net_from_id.begin(), iter)] = 1;
                                tt_instr_mem_cnt[tt_instr_mem_index] += offset;
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                auto for_node_addr = dffs[*i - luts.size()].node_addr;
                auto for_node_addr_num = for_node_addr.first * N_PROCESSORS_PER_CLUSTER + for_node_addr.second;
                if (for_node_addr == cur_node_addr)
                {
                    auto iter = find(dffs[*i - luts.size()].in_net_from_id.begin(), dffs[*i - luts.size()].in_net_from_id.end(), n_dff + luts.size());
                    dffs[*i - luts.size()].in_net_from_addr[distance(dffs[*i - luts.size()].in_net_from_id.begin(), iter)] = make_pair(Inter_Datamem, cur_instr_mem_cnt);
                    dffs[*i - luts.size()].in_net_from_ready[distance(dffs[*i - luts.size()].in_net_from_id.begin(), iter)] = 1;
                }
                else
                {
                    if (tt_instr_mem[for_node_addr_num][cur_instr_mem_cnt].Node_Addr.size() < 4)
                    {
                        tt_instr_mem[for_node_addr_num][cur_instr_mem_cnt].Node_Addr.push_back(tt_instr_mem_index);
                        int n = tt_instr_mem[for_node_addr_num][cur_instr_mem_cnt].Node_Addr.size();
                        auto iter = find(dffs[*i - luts.size()].in_net_from_id.begin(), dffs[*i - luts.size()].in_net_from_id.end(), n_dff + luts.size());
                        dffs[*i - luts.size()].in_net_from_addr[distance(dffs[*i - luts.size()].in_net_from_id.begin(), iter)] = make_pair(Sel_Exter_Datamem(n), cur_instr_mem_cnt);
                        dffs[*i - luts.size()].in_net_from_ready[distance(dffs[*i - luts.size()].in_net_from_id.begin(), iter)] = 1;
                    }
                    else
                    {
                        for (int offset = 1;; offset++)
                        {
                            if (tt_instr_mem[for_node_addr_num][cur_instr_mem_cnt + offset].Node_Addr.size() < 4)
                            {
                                tt_instr_mem[for_node_addr_num][cur_instr_mem_cnt + offset].Node_Addr.push_back(tt_instr_mem_index);
                                int n = tt_instr_mem[for_node_addr_num][cur_instr_mem_cnt + offset].Node_Addr.size();
                                auto iter = find(dffs[*i - luts.size()].in_net_from_id.begin(), dffs[*i - luts.size()].in_net_from_id.end(), n_dff + luts.size());
                                dffs[*i - luts.size()].in_net_from_addr[distance(dffs[*i - luts.size()].in_net_from_id.begin(), iter)] = make_pair(Sel_Exter_Datamem(n), cur_instr_mem_cnt + offset);
                                dffs[*i - luts.size()].in_net_from_ready[distance(dffs[*i - luts.size()].in_net_from_id.begin(), iter)] = 1;
                                tt_instr_mem_cnt[tt_instr_mem_index] += offset;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // Generate for nodes simulation
    for (auto level = SchList.begin(); level != SchList.end(); level++)
    {
        for (auto i = level->begin(); i != level->end(); i++)
        {
            auto node_num = *i;
            if (node_num < luts.size())
            {
                LutType cur_lut = luts[node_num];
                if (find(cur_lut.in_net_from_ready.begin(), cur_lut.in_net_from_ready.end(), 0) != cur_lut.in_net_from_ready.end()) 
                {
                    cout << "ERROR: Input signal not ready " << "(LUT " << node_num << ")" << ", instruction generation failed!" << endl;
                    exit(-1);
                }
                pair<int, int> cur_node_addr = cur_lut.node_addr;
                int tt_instr_mem_index = cur_node_addr.first * N_PROCESSORS_PER_CLUSTER + cur_node_addr.second;
                int cur_instr_mem_cnt  = tt_instr_mem_cnt[tt_instr_mem_index];
                int pushaddr = cur_instr_mem_cnt;
                Instr new_instr;
                string lut_res_binary = HextoBinary(cur_lut.lut_res);
                new_instr.Opcode    = LUT_CONFIG;
                new_instr.Jump      = 00;
                new_instr.Value_Data.push_back(stoi(lut_res_binary));
                for (auto i = cur_lut.in_net_from_addr.begin(); i != cur_lut.in_net_from_addr.end(); i++)
                {
                    new_instr.Datamem_Sel.push_back(i->first);
                }
                for (auto i = cur_lut.in_net_from_addr.begin(); i != cur_lut.in_net_from_addr.end(); i++)
                {
                    new_instr.Operand_Addr.push_back(i->second);
                    if (i->second != MEM_DEPTH - 1 && i->second >= cur_instr_mem_cnt)
                        pushaddr = ((i->second + 1) > pushaddr) ? i->second : pushaddr;
                }
                tt_instr_mem[tt_instr_mem_index][pushaddr] = new_instr;
                tt_instr_mem_cnt[tt_instr_mem_index] = pushaddr + 1;

                auto cur_node_for_id = net_for_id[cur_lut.out_ports];
                for (auto i = cur_node_for_id.begin(); i != cur_node_for_id.end(); i++)
                {
                    if (*i < luts.size())
                    {
                        auto for_node_addr = luts[*i].node_addr;
                        auto for_node_addr_num = for_node_addr.first * N_PROCESSORS_PER_CLUSTER + for_node_addr.second;
                        if (for_node_addr == cur_node_addr)
                        {
                            auto iter = find(luts[*i].in_net_from_id.begin(), luts[*i].in_net_from_id.end(), node_num);
                            luts[*i].in_net_from_addr[distance(luts[*i].in_net_from_id.begin(), iter)] = make_pair(Inter_Datamem, pushaddr);
                            luts[*i].in_net_from_ready[distance(luts[*i].in_net_from_id.begin(), iter)] = 1;
                        }
                        else
                        {
                            if (tt_instr_mem[for_node_addr_num][pushaddr].Node_Addr.size() < 4)
                            {
                                tt_instr_mem[for_node_addr_num][pushaddr].Node_Addr.push_back(tt_instr_mem_index);
                                int n = tt_instr_mem[for_node_addr_num][pushaddr].Node_Addr.size();
                                auto iter = find(luts[*i].in_net_from_id.begin(), luts[*i].in_net_from_id.end(), node_num);
                                luts[*i].in_net_from_addr[distance(luts[*i].in_net_from_id.begin(), iter)] = make_pair(Sel_Exter_Datamem(n), pushaddr);
                                luts[*i].in_net_from_ready[distance(luts[*i].in_net_from_id.begin(), iter)] = 1;
                            }
                            else
                            {
                                for (int offset = 1; ; offset++)
                                {
                                    if (tt_instr_mem[for_node_addr_num][pushaddr + offset].Node_Addr.size() < 4)
                                    {
                                        tt_instr_mem[for_node_addr_num][pushaddr + offset].Node_Addr.push_back(tt_instr_mem_index);
                                        int n = tt_instr_mem[for_node_addr_num][pushaddr + offset].Node_Addr.size();
                                        auto iter = find(luts[*i].in_net_from_id.begin(), luts[*i].in_net_from_id.end(), node_num);
                                        luts[*i].in_net_from_addr[distance(luts[*i].in_net_from_id.begin(), iter)] = make_pair(Sel_Exter_Datamem(n), pushaddr + offset);
                                        luts[*i].in_net_from_ready[distance(luts[*i].in_net_from_id.begin(), iter)] = 1;
                                        tt_instr_mem_cnt[tt_instr_mem_index] += offset;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        auto for_node_addr = dffs[*i - luts.size()].node_addr;
                        auto for_node_addr_num = for_node_addr.first * N_PROCESSORS_PER_CLUSTER + for_node_addr.second;
                        if (for_node_addr == cur_node_addr)
                        {
                            auto iter = find(dffs[*i - luts.size()].in_net_from_id.begin(), dffs[*i - luts.size()].in_net_from_id.end(), node_num);
                            dffs[*i - luts.size()].in_net_from_addr[distance(dffs[*i - luts.size()].in_net_from_id.begin(), iter)] = make_pair(Inter_Datamem, pushaddr);
                            dffs[*i - luts.size()].in_net_from_ready[distance(dffs[*i - luts.size()].in_net_from_id.begin(), iter)] = 1;
                        }
                        else
                        {
                            if (tt_instr_mem[for_node_addr_num][pushaddr].Node_Addr.size() < 4)
                            {
                                tt_instr_mem[for_node_addr_num][pushaddr].Node_Addr.push_back(tt_instr_mem_index);
                                int n = tt_instr_mem[for_node_addr_num][pushaddr].Node_Addr.size();
                                auto iter = find(dffs[*i - luts.size()].in_net_from_id.begin(), dffs[*i - luts.size()].in_net_from_id.end(), node_num);
                                dffs[*i - luts.size()].in_net_from_addr[distance(dffs[*i - luts.size()].in_net_from_id.begin(), iter)] = make_pair(Sel_Exter_Datamem(n), pushaddr);
                                dffs[*i - luts.size()].in_net_from_ready[distance(dffs[*i - luts.size()].in_net_from_id.begin(), iter)] = 1;
                            }
                            else
                            {
                                for (int offset = 1;  ; offset++)
                                {
                                    if (tt_instr_mem[for_node_addr_num][pushaddr + offset].Node_Addr.size() < 4)
                                    {
                                        tt_instr_mem[for_node_addr_num][pushaddr + offset].Node_Addr.push_back(tt_instr_mem_index);
                                        int n = tt_instr_mem[for_node_addr_num][pushaddr + offset].Node_Addr.size();
                                        auto iter = find(dffs[*i - luts.size()].in_net_from_id.begin(), dffs[*i - luts.size()].in_net_from_id.end(), node_num);
                                        dffs[*i - luts.size()].in_net_from_addr[distance(dffs[*i - luts.size()].in_net_from_id.begin(), iter)] = make_pair(Sel_Exter_Datamem(n), pushaddr + offset);
                                        dffs[*i - luts.size()].in_net_from_ready[distance(dffs[*i - luts.size()].in_net_from_id.begin(), iter)] = 1;
                                        tt_instr_mem_cnt[tt_instr_mem_index] += offset;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                DffType cur_dff = dffs[node_num - luts.size()];
                if (find(cur_dff.in_net_from_ready.begin(), cur_dff.in_net_from_ready.end(), 0) != cur_dff.in_net_from_ready.end())
                {
                    cout << "ERROR: Input signal not ready " << "(DFF " << node_num - luts.size() << ")" << ", instruction generation failed!" << endl;
                    exit(-1);
                }
                // pair<int, int> cur_node_addr = cur_dff.node_addr;
                // int tt_instr_mem_index = cur_node_addr.first * N_PROCESSORS_PER_CLUSTER + cur_node_addr.second;
                // int cur_instr_mem_cnt = tt_instr_mem_cnt[tt_instr_mem_index];
                // int pushaddr = cur_instr_mem_cnt;
                // Instr new_instr;
                // string lut_res_binary = HextoBinary(cur_dff.lut_res);
                // new_instr.Opcode = LUT_CONFIG;
                // new_instr.Jump = 00;
                // new_instr.Value_Data.push_back(stoi(lut_res_binary));
                // for (auto i = cur_dff.in_net_from_addr.begin(); i != cur_dff.in_net_from_addr.end(); i++)
                // {
                //     new_instr.Datamem_Sel.push_back(i->first);
                // }
                // for (auto i = cur_dff.in_net_from_addr.begin(); i != cur_dff.in_net_from_addr.end(); i++)
                // {
                //     new_instr.Operand_Addr.push_back(i->second);
                //     if (i->second != MEM_DEPTH - 1 && i->second >= cur_instr_mem_cnt)
                //         pushaddr = ((i->second + 1) > pushaddr) ? i->second : pushaddr;
                // }
                // tt_instr_mem[tt_instr_mem_index][pushaddr] = new_instr;
                // tt_instr_mem_cnt[tt_instr_mem_index] = pushaddr + 1;
           }
        }
    }

    // Generating Instructions for DFFs
    for (vector<vector<int>>::reverse_iterator l = levels_dff.rbegin(); l != levels_dff.rend(); l++)
    {
        for (auto d = l->begin(); d != l->end(); d++)
        {
            int dff_num = *d;
            DffType cur_dff = dffs[dff_num];
            int cur_processor_id = N_PROCESSORS_PER_CLUSTER * cur_dff.node_addr[0] + cur_dff.node_addr[1];
            vector<int> cur_in_net_from_id = cur_dff.in_net_from_id;
            vector<int> cur_in_net_from_part = cur_dff.in_net_from_part;
            for (int it = 0; it < cur_in_net_from_id.size();)
            {
                if (cur_in_net_from_id[it] == -1 || cur_in_net_from_id[it] == -2 ||
                    (cur_in_net_from_id[it] < luts.size() && luts[cur_in_net_from_id[it]].node_addr == cur_dff.node_addr) ||
                    (cur_in_net_from_id[it] >= luts.size()))
                {
                    cur_in_net_from_id.erase(cur_in_net_from_id.begin() + it);
                    cur_in_net_from_part.erase(cur_in_net_from_part.begin() + it);
                }
                else
                    it++;
            }

            vector<int> config_instr_pos;
            map<int, int> from_id_from_part; // <in_from_id, id_from_part>
            map<int, int> from_id_opr_addr;  // <in_from_id, operand_addr>
            if (cur_in_net_from_id.size() >= 1)
            {
                for (int in = 0; in < cur_in_net_from_id.size(); in++)
                {
                    LutType cur_in_from_lut = luts[cur_in_net_from_id[in]];
                    int from_processor_id = N_PROCESSORS_PER_CLUSTER * cur_in_from_lut.node_addr[0] + cur_in_from_lut.node_addr[1];
                    from_id_from_part.insert(pair<int, int>(cur_in_net_from_id[in], cur_in_net_from_part[in]));
                    int get_res_addr;
                    if (processors[from_processor_id].id_outaddr.first == cur_in_net_from_id[in] && processors[from_processor_id].id_outaddr.second > 0 && processors[from_processor_id].id_outaddr.second < INS_DEPTH)
                    {
                        get_res_addr = max(processors[from_processor_id].id_outaddr.second, int(processors[cur_processor_id].instr_mem.size()));
                    }
                    else
                    {
                        Instr_2 instr_2;
                        instr_2.PC_Jump = 0;
                        instr_2.BM_Jump = 0;
                        instr_2.Node_Addr = cur_in_from_lut.node_addr;
                        instr_2.Data_Mem_Select = "0";
                        instr_2.Operand_Addr = cur_in_from_lut.res_pos_at_mem;
                        string lut_instr_2 = cat_instr_2(instr_2);
                        processors[from_processor_id].instr_mem.push_back(lut_instr_2);
                        processors[from_processor_id].id_outaddr = pair<int, int>(cur_in_net_from_id[in], int(processors[from_processor_id].instr_mem.size()) - 1);
                        get_res_addr = max(processors[from_processor_id].id_outaddr.second, int(processors[cur_processor_id].instr_mem.size()));
                    }
                    from_id_opr_addr.insert(pair<int, int>(cur_in_net_from_id[in], get_res_addr));
                    if (from_id_from_part[cur_in_net_from_id[in]] == -3) // current part
                        config_instr_pos.push_back(get_res_addr + INTRA_CLUSTER_CLK - 1);
                    else // other part
                        config_instr_pos.push_back(get_res_addr + INTER_CLUSTER_CLK - 1);
                    int cur_lut_idle_num = get_res_addr - (int(processors[cur_processor_id].instr_mem.size()) - 1);
                    int from_lut_idle_num = ((get_res_addr - (int(processors[from_processor_id].instr_mem.size()) - 1)) > 0) ? (get_res_addr - int(processors[from_processor_id].instr_mem.size()) + 1) : 0;
                    Instr_4 instr_4;
                    instr_4.PC_Jump = 0;
                    instr_4.BM_Jump = 0;
                    instr_4.Node_Addr = cur_in_from_lut.node_addr;
                    string lut_instr_4 = cat_instr_4(instr_4);
                    // current lut
                    processors[cur_processor_id].instr_mem.insert(processors[cur_processor_id].instr_mem.end(), cur_lut_idle_num, lut_instr_4);
                    // in from lut
                    processors[from_processor_id].instr_mem.insert(processors[from_processor_id].instr_mem.end(), from_lut_idle_num, lut_instr_4);
                }
                auto max_config_addr = max_element(config_instr_pos.begin(), config_instr_pos.end());
                Instr_4 instr_4;
                instr_4.PC_Jump = 0;
                instr_4.BM_Jump = 0;
                instr_4.Node_Addr = cur_dff.node_addr;
                string lut_instr_4 = cat_instr_4(instr_4);
                processors[cur_processor_id].instr_mem.insert(processors[cur_processor_id].instr_mem.end(), *max_config_addr - int(processors[cur_processor_id].instr_mem.size()), lut_instr_4);
            }

            if (cur_dff.type == 0)
            {
                assert(cur_dff.assignsig_condsig.size() == 1);
                if (cur_dff.assignsig_condsig.begin()->first == "0" || cur_dff.assignsig_condsig.begin()->first == "1")
                {
                    Instr_9 instr_9;
                    instr_9.PC_Jump = 0;
                    instr_9.BM_Jump = 0;
                    instr_9.Node_Addr = cur_dff.node_addr;
                    instr_9.im = atoi((cur_dff.assignsig_condsig.begin()->first).c_str());
                    string dff_instr = cat_instr_9(instr_9);
                    processors[cur_processor_id].instr_mem.push_back(dff_instr);
                    dffs[dff_num].res_pos_at_mem = int(processors[cur_processor_id].instr_mem.size()) - 1;
                    processors[cur_processor_id].id_outaddr = pair<int, int>(dff_num + luts.size(), int(processors[cur_processor_id].instr_mem.size()) - 1);
                }
                else
                {
                    Instr_10 instr_10;
                    instr_10.PC_Jump = 0;
                    instr_10.BM_Jump = 0;
                    instr_10.Node_Addr = cur_dff.node_addr;
                    assert(cur_dff.in_net_from_id.size() == 1);
                    if (cur_dff.in_net_from_id[0] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[0]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[0]];
                            if (*(tvs_val->begin()) == 0)
                                instr_10.data_mem_select = 0;
                            else
                                instr_10.data_mem_select = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[0] == INITIAL_JUMP_SIG)
                                instr_10.Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_10.Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[0] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[0] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[0] == 0)
                            instr_10.data_mem_select = 0;
                        else
                            instr_10.data_mem_select = 1;
                        instr_10.Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[0] < luts.size() && luts[cur_dff.in_net_from_id[0]].node_addr == cur_dff.node_addr)
                    {
                        instr_10.data_mem_select = 0;
                        instr_10.Addr = luts[*(cur_dff.in_net_from_id.begin())].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[0] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[0] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_10.data_mem_select = 0;
                        else
                            instr_10.data_mem_select = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[0] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(8, 0);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_10.Addr = dffs[cur_dff.in_net_from_id[0] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_10.data_mem_select = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[0]);
                        instr_10.Addr = it->second;
                    }
                    string dff_instr = cat_instr_10(instr_10);
                    processors[cur_processor_id].instr_mem.push_back(dff_instr);
                    dffs[dff_num].res_pos_at_mem = int(processors[cur_processor_id].instr_mem.size()) - 1;
                    processors[cur_processor_id].id_outaddr = pair<int, int>(dff_num + luts.size(), int(processors[cur_processor_id].instr_mem.size()) - 1);
                }
            }
            else if (cur_dff.type == 1)
            {
                assert(cur_dff.assignsig_condsig.size() == 1);
                if (cur_dff.assignsig_condsig.begin()->first == "0" || cur_dff.assignsig_condsig.begin()->first == "1")
                {
                    Instr_11 instr_11;
                    instr_11.PC_Jump = 0;
                    instr_11.BM_Jump = 0;
                    instr_11.Node_Addr = cur_dff.node_addr;
                    assert(cur_dff.in_net_from_id.size() == 1);
                    if (cur_dff.in_net_from_id[0] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[0]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[0]];
                            if (*(tvs_val->begin()) == 0)
                                instr_11.data_mem_select = 0;
                            else
                                instr_11.data_mem_select = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[0] == INITIAL_JUMP_SIG)
                                instr_11.Operand_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_11.Operand_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[0] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[0] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[0] == 0)
                            instr_11.data_mem_select = 0;
                        else
                            instr_11.data_mem_select = 1;
                        instr_11.Operand_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[0] < luts.size() && luts[cur_dff.in_net_from_id[0]].node_addr == cur_dff.node_addr)
                    {
                        instr_11.data_mem_select = 0;
                        instr_11.Operand_Addr = luts[*(cur_dff.in_net_from_id.begin())].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[0] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[0] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_11.data_mem_select = 0;
                        else
                            instr_11.data_mem_select = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[0] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(17, 9);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_11.Operand_Addr = dffs[cur_dff.in_net_from_id[0] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_11.data_mem_select = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[0]);
                        instr_11.Operand_Addr = it->second;
                    }
                    instr_11.value = cur_dff.assignsig_condsig.begin()->second.second;
                    instr_11.T = atoi(cur_dff.assignsig_condsig.begin()->first.c_str());
                    string dff_instr = cat_instr_11(instr_11);
                    processors[cur_processor_id].instr_mem.push_back(dff_instr);
                    dffs[dff_num].res_pos_at_mem = int(processors[cur_processor_id].instr_mem.size()) - 1;
                    processors[cur_processor_id].id_outaddr = pair<int, int>(dff_num + luts.size(), int(processors[cur_processor_id].instr_mem.size()) - 1);
                }
                else
                {
                    Instr_12 instr_12;
                    instr_12.PC_Jump = 0;
                    instr_12.BM_Jump = 0;
                    instr_12.Node_Addr = cur_dff.node_addr;
                    assert(cur_dff.in_net_from_id.size() == 2);
                    if (cur_dff.in_net_from_id[1] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[1]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[1]];
                            if (*(tvs_val->begin()) == 0)
                                instr_12.data_mem_select1 = 0;
                            else
                                instr_12.data_mem_select1 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[1] == INITIAL_JUMP_SIG)
                                instr_12.Operand_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_12.Operand_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[1] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[1] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[1] == 0)
                            instr_12.data_mem_select1 = 0;
                        else
                            instr_12.data_mem_select1 = 1;
                        instr_12.Operand_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[1] < luts.size() && luts[cur_dff.in_net_from_id[1]].node_addr == cur_dff.node_addr)
                    {
                        instr_12.data_mem_select1 = 0;
                        instr_12.Operand_Addr = luts[cur_dff.in_net_from_id[1]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[1] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[1] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_12.data_mem_select1 = 0;
                        else
                            instr_12.data_mem_select1 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[1] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(17, 9);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_12.Operand_Addr = dffs[cur_dff.in_net_from_id[1] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_12.data_mem_select1 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[1]);
                        instr_12.Operand_Addr = it->second;
                    }
                    instr_12.value = cur_dff.assignsig_condsig.begin()->second.second;
                    if (cur_dff.in_net_from_id[0] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[0]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[0]];
                            if (*(tvs_val->begin()) == 0)
                                instr_12.data_mem_select2 = 0;
                            else
                                instr_12.data_mem_select2 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[0] == INITIAL_JUMP_SIG)
                                instr_12.T_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_12.T_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[0] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[0] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[0] == 0)
                            instr_12.data_mem_select2 = 0;
                        else
                            instr_12.data_mem_select2 = 1;
                        instr_12.T_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[0] < luts.size() && luts[cur_dff.in_net_from_id[0]].node_addr == cur_dff.node_addr)
                    {
                        instr_12.data_mem_select2 = 0;
                        instr_12.T_Addr = luts[cur_dff.in_net_from_id[0]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[0] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[0] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_12.data_mem_select2 = 0;
                        else
                            instr_12.data_mem_select2 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[0] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(8, 0);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_12.T_Addr = dffs[cur_dff.in_net_from_id[0] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_12.data_mem_select2 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[0]);
                        instr_12.T_Addr = it->second;
                    }
                    string dff_instr = cat_instr_12(instr_12);
                    processors[cur_processor_id].instr_mem.push_back(dff_instr);
                    dffs[dff_num].res_pos_at_mem = int(processors[cur_processor_id].instr_mem.size()) - 1;
                    processors[cur_processor_id].id_outaddr = pair<int, int>(dff_num + luts.size(), int(processors[cur_processor_id].instr_mem.size()) - 1);
                }
            }
            else if (cur_dff.type == 2)
            {
                assert(cur_dff.assignsig_condsig.size() == 2);
                if ((cur_dff.assignsig_condsig.begin()->first == "0" || cur_dff.assignsig_condsig.begin()->first == "1") && ((cur_dff.assignsig_condsig.begin() + 1)->first == "0" || (cur_dff.assignsig_condsig.begin() + 1)->first == "1"))
                {
                    Instr_5 instr_5;
                    instr_5.PC_Jump = 0;
                    instr_5.BM_Jump = 0;
                    instr_5.Node_Addr = cur_dff.node_addr;
                    assert(cur_dff.in_net_from_id.size() == 1);
                    if (cur_dff.in_net_from_id[0] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[0]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[0]];
                            if (*(tvs_val->begin()) == 0)
                                instr_5.data_mem_select = 0;
                            else
                                instr_5.data_mem_select = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[0] == INITIAL_JUMP_SIG)
                                instr_5.Operand_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_5.Operand_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[0] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[0] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[0] == 0)
                            instr_5.data_mem_select = 0;
                        else
                            instr_5.data_mem_select = 1;
                        instr_5.Operand_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[0] < luts.size() && luts[cur_dff.in_net_from_id[0]].node_addr == cur_dff.node_addr)
                    {
                        instr_5.data_mem_select = 0;
                        instr_5.Operand_Addr = luts[*(cur_dff.in_net_from_id.begin())].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[0] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[0] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_5.data_mem_select = 0;
                        else
                            instr_5.data_mem_select = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[0] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(17, 9);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_5.Operand_Addr = dffs[cur_dff.in_net_from_id[0] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_5.data_mem_select = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[0]);
                        instr_5.Operand_Addr = it->second;
                    }

                    instr_5.value = cur_dff.assignsig_condsig.begin()->second.second;
                    instr_5.T = stoi(cur_dff.assignsig_condsig.begin()->first.c_str());
                    instr_5.F = stoi((cur_dff.assignsig_condsig.begin() + 1)->first.c_str());
                    string dff_instr = cat_instr_5(instr_5);
                    processors[cur_processor_id].instr_mem.push_back(dff_instr);
                    dffs[dff_num].res_pos_at_mem = int(processors[cur_processor_id].instr_mem.size()) - 1;
                    processors[cur_processor_id].id_outaddr = pair<int, int>(dff_num + luts.size(), int(processors[cur_processor_id].instr_mem.size()) - 1);
                }
                else if ((cur_dff.assignsig_condsig.begin()->first == "0" || cur_dff.assignsig_condsig.begin()->first == "1") && ((cur_dff.assignsig_condsig.begin() + 1)->first != "0" && (cur_dff.assignsig_condsig.begin() + 1)->first != "1"))
                {
                    Instr_6 instr_6;
                    instr_6.PC_Jump = 0;
                    instr_6.BM_Jump = 0;
                    instr_6.Node_Addr = cur_dff.node_addr;
                    assert(cur_dff.in_net_from_id.size() == 2);
                    if (cur_dff.in_net_from_id[0] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[0]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[0]];
                            if (*(tvs_val->begin()) == 0)
                                instr_6.data_mem_select1 = 0;
                            else
                                instr_6.data_mem_select1 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[0] == INITIAL_JUMP_SIG)
                                instr_6.Operand_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_6.Operand_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[0] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[0] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[0] == 0)
                            instr_6.data_mem_select1 = 0;
                        else
                            instr_6.data_mem_select1 = 1;
                        instr_6.Operand_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[0] < luts.size() && luts[cur_dff.in_net_from_id[0]].node_addr == cur_dff.node_addr)
                    {
                        instr_6.data_mem_select1 = 0;
                        instr_6.Operand_Addr = luts[cur_dff.in_net_from_id[0]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[0] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[0] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_6.data_mem_select1 = 0;
                        else
                            instr_6.data_mem_select1 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[0] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(17, 9);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_6.Operand_Addr = dffs[cur_dff.in_net_from_id[0] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_6.data_mem_select1 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[0]);
                        instr_6.Operand_Addr = it->second;
                    }
                    instr_6.value = cur_dff.assignsig_condsig.begin()->second.second;
                    instr_6.T = stoi(cur_dff.assignsig_condsig.begin()->first.c_str());
                    if (cur_dff.in_net_from_id[1] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[1]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[1]];
                            if (*(tvs_val->begin()) == 0)
                                instr_6.data_mem_select2 = 0;
                            else
                                instr_6.data_mem_select2 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[1] == INITIAL_JUMP_SIG)
                                instr_6.F_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_6.F_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[1] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[1] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[1] == 0)
                            instr_6.data_mem_select2 = 0;
                        else
                            instr_6.data_mem_select2 = 1;
                        instr_6.F_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[1] < luts.size() && luts[cur_dff.in_net_from_id[1]].node_addr == cur_dff.node_addr)
                    {
                        instr_6.data_mem_select2 = 0;
                        instr_6.F_Addr = luts[cur_dff.in_net_from_id[1]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[1] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[1] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_6.data_mem_select2 = 0;
                        else
                            instr_6.data_mem_select2 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[1] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(26, 18);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_6.F_Addr = dffs[cur_dff.in_net_from_id[1] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_6.data_mem_select2 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[1]);
                        instr_6.F_Addr = it->second;
                    }
                    string dff_instr = cat_instr_6(instr_6);
                    processors[cur_processor_id].instr_mem.push_back(dff_instr);
                    dffs[dff_num].res_pos_at_mem = int(processors[cur_processor_id].instr_mem.size()) - 1;
                    processors[cur_processor_id].id_outaddr = pair<int, int>(dff_num + luts.size(), int(processors[cur_processor_id].instr_mem.size()) - 1);
                }
                else if ((cur_dff.assignsig_condsig.begin()->first != "0" && cur_dff.assignsig_condsig.begin()->first != "1") && ((cur_dff.assignsig_condsig.begin() + 1)->first != "0" && (cur_dff.assignsig_condsig.begin() + 1)->first != "1"))
                {
                    Instr_7 instr_7;
                    instr_7.PC_Jump = 0;
                    instr_7.BM_Jump = 0;
                    instr_7.Node_Addr = cur_dff.node_addr;
                    assert(cur_dff.in_net_from_id.size() == 3);
                    if (cur_dff.in_net_from_id[1] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[1]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[1]];
                            if (*(tvs_val->begin()) == 0)
                                instr_7.data_mem_select1 = 0;
                            else
                                instr_7.data_mem_select1 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[1] == INITIAL_JUMP_SIG)
                                instr_7.Operand_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_7.Operand_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[1] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[1] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[1] == 0)
                            instr_7.data_mem_select1 = 0;
                        else
                            instr_7.data_mem_select1 = 1;
                        instr_7.Operand_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[1] < luts.size() && luts[cur_dff.in_net_from_id[1]].node_addr == cur_dff.node_addr)
                    {
                        instr_7.data_mem_select1 = 0;
                        instr_7.Operand_Addr = luts[cur_dff.in_net_from_id[1]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[1] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[1] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_7.data_mem_select1 = 0;
                        else
                            instr_7.data_mem_select1 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[1] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(17, 9);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_7.Operand_Addr = dffs[cur_dff.in_net_from_id[1] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_7.data_mem_select1 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[1]);
                        instr_7.Operand_Addr = it->second;
                    }
                    instr_7.value = cur_dff.assignsig_condsig.begin()->second.second;
                    if (cur_dff.in_net_from_id[0] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[0]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[0]];
                            if (*(tvs_val->begin()) == 0)
                                instr_7.data_mem_select2 = 0;
                            else
                                instr_7.data_mem_select2 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[0] == INITIAL_JUMP_SIG)
                                instr_7.T_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_7.T_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[0] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[0] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[0] == 0)
                            instr_7.data_mem_select2 = 0;
                        else
                            instr_7.data_mem_select2 = 1;
                        instr_7.T_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[0] < luts.size() && luts[cur_dff.in_net_from_id[0]].node_addr == cur_dff.node_addr)
                    {
                        instr_7.data_mem_select2 = 0;
                        instr_7.T_Addr = luts[cur_dff.in_net_from_id[0]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[0] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[0] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_7.data_mem_select2 = 0;
                        else
                            instr_7.data_mem_select2 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[0] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(8, 0);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_7.T_Addr = dffs[cur_dff.in_net_from_id[0] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_7.data_mem_select2 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[0]);
                        instr_7.T_Addr = it->second;
                    }
                    if (cur_dff.in_net_from_id[2] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[2]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[2]];
                            if (*(tvs_val->begin()) == 0)
                                instr_7.data_mem_select3 = 0;
                            else
                                instr_7.data_mem_select3 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[2] == INITIAL_JUMP_SIG)
                                instr_7.F_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_7.F_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[2] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[2] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[2] == 0)
                            instr_7.data_mem_select3 = 0;
                        else
                            instr_7.data_mem_select3 = 1;
                        instr_7.F_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[2] < luts.size() && luts[cur_dff.in_net_from_id[2]].node_addr == cur_dff.node_addr)
                    {
                        instr_7.data_mem_select3 = 0;
                        instr_7.F_Addr = luts[cur_dff.in_net_from_id[2]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[2] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[2] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_7.data_mem_select3 = 0;
                        else
                            instr_7.data_mem_select3 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[2] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(26, 18);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_7.F_Addr = dffs[cur_dff.in_net_from_id[2] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_7.data_mem_select3 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[2]);
                        instr_7.F_Addr = it->second;
                    }
                    string dff_instr = cat_instr_7(instr_7);
                    processors[cur_processor_id].instr_mem.push_back(dff_instr);
                    dffs[dff_num].res_pos_at_mem = int(processors[cur_processor_id].instr_mem.size()) - 1;
                    processors[cur_processor_id].id_outaddr = pair<int, int>(dff_num + luts.size(), int(processors[cur_processor_id].instr_mem.size()) - 1);
                }
                else if ((cur_dff.assignsig_condsig.begin()->first != "0" && cur_dff.assignsig_condsig.begin()->first != "1") && ((cur_dff.assignsig_condsig.begin() + 1)->first == "0" || (cur_dff.assignsig_condsig.begin() + 1)->first == "1"))
                {
                    Instr_8 instr_8;
                    instr_8.PC_Jump = 0;
                    instr_8.BM_Jump = 0;
                    instr_8.Node_Addr = cur_dff.node_addr;
                    assert(cur_dff.in_net_from_id.size() == 2);
                    if (cur_dff.in_net_from_id[1] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[1]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[1]];
                            if (*(tvs_val->begin()) == 0)
                                instr_8.data_mem_select1 = 0;
                            else
                                instr_8.data_mem_select1 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[1] == INITIAL_JUMP_SIG)
                                instr_8.Operand_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_8.Operand_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[1] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[1] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[1] == 0)
                            instr_8.data_mem_select1 = 0;
                        else
                            instr_8.data_mem_select1 = 1;
                        instr_8.Operand_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[1] < luts.size() && luts[cur_dff.in_net_from_id[1]].node_addr == cur_dff.node_addr)
                    {
                        instr_8.data_mem_select1 = 0;
                        instr_8.Operand_Addr = luts[cur_dff.in_net_from_id[1]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[1] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[1] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_8.data_mem_select1 = 0;
                        else
                            instr_8.data_mem_select1 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[1] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(17, 9);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_8.Operand_Addr = dffs[cur_dff.in_net_from_id[1] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_8.data_mem_select1 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[1]);
                        instr_8.Operand_Addr = it->second;
                    }
                    instr_8.value = cur_dff.assignsig_condsig.begin()->second.second;
                    if (cur_dff.in_net_from_id[0] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[0]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[0]];
                            if (*(tvs_val->begin()) == 0)
                                instr_8.data_mem_select2 = 0;
                            else
                                instr_8.data_mem_select2 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[0] == INITIAL_JUMP_SIG)
                                instr_8.T_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_8.T_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[0] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[0] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[0] == 0)
                            instr_8.data_mem_select2 = 0;
                        else
                            instr_8.data_mem_select2 = 1;
                        instr_8.T_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[0] < luts.size() && luts[cur_dff.in_net_from_id[0]].node_addr == cur_dff.node_addr)
                    {
                        instr_8.data_mem_select2 = 0;
                        instr_8.T_Addr = luts[cur_dff.in_net_from_id[0]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[0] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[0] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_8.data_mem_select2 = 0;
                        else
                            instr_8.data_mem_select2 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[0] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(8, 0);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_8.T_Addr = dffs[cur_dff.in_net_from_id[0] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_8.data_mem_select2 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[0]);
                        instr_8.T_Addr = it->second;
                    }
                    instr_8.F = atoi((cur_dff.assignsig_condsig.begin() + 1)->first.c_str());
                    string dff_instr = cat_instr_8(instr_8);
                    processors[cur_processor_id].instr_mem.push_back(dff_instr);
                    dffs[dff_num].res_pos_at_mem = int(processors[cur_processor_id].instr_mem.size()) - 1;
                    processors[cur_processor_id].id_outaddr = pair<int, int>(dff_num + luts.size(), int(processors[cur_processor_id].instr_mem.size()) - 1);
                }
            }
            else if (cur_dff.type == 3)
            {
                assert(cur_dff.assignsig_condsig.size() == 2);
                if ((cur_dff.assignsig_condsig.begin()->first == "0" || cur_dff.assignsig_condsig.begin()->first == "1") && ((cur_dff.assignsig_condsig.begin() + 1)->first == "0" || (cur_dff.assignsig_condsig.begin() + 1)->first == "1"))
                {
                    Instr_13 instr_13;
                    instr_13.PC_Jump = 0;
                    instr_13.BM_Jump = 0;
                    instr_13.Node_Addr = cur_dff.node_addr;
                    assert(cur_dff.in_net_from_id.size() == 2);
                    if (cur_dff.in_net_from_id[0] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[0]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[0]];
                            if (*(tvs_val->begin()) == 0)
                                instr_13.data_mem_select1 = 0;
                            else
                                instr_13.data_mem_select1 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[0] == INITIAL_JUMP_SIG)
                                instr_13.Operand_Addr1 = INITIAL_JUMP_ADDR;
                            else
                                instr_13.Operand_Addr1 = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[0] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[0] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[0] == 0)
                            instr_13.data_mem_select1 = 0;
                        else
                            instr_13.data_mem_select1 = 1;
                        instr_13.Operand_Addr1 = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[0] < luts.size() && luts[cur_dff.in_net_from_id[0]].node_addr == cur_dff.node_addr)
                    {
                        instr_13.data_mem_select1 = 0;
                        instr_13.Operand_Addr1 = luts[*(cur_dff.in_net_from_id.begin())].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[0] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[0] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_13.data_mem_select1 = 0;
                        else
                            instr_13.data_mem_select1 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[0] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(17, 9);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_13.Operand_Addr1 = dffs[cur_dff.in_net_from_id[0] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_13.data_mem_select1 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[0]);
                        instr_13.Operand_Addr1 = it->second;
                    }
                    instr_13.value1 = cur_dff.assignsig_condsig.begin()->second.second;
                    instr_13.T1 = stoi(cur_dff.assignsig_condsig.begin()->first.c_str());
                    if (cur_dff.in_net_from_id[1] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[1]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[1]];
                            if (*(tvs_val->begin()) == 0)
                                instr_13.data_mem_select2 = 0;
                            else
                                instr_13.data_mem_select2 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[1] == INITIAL_JUMP_SIG)
                                instr_13.Operand_Addr2 = INITIAL_JUMP_ADDR;
                            else
                                instr_13.Operand_Addr2 = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[1] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[1] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[1] == 0)
                            instr_13.data_mem_select2 = 0;
                        else
                            instr_13.data_mem_select2 = 1;
                        instr_13.Operand_Addr2 = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[1] < luts.size() && luts[cur_dff.in_net_from_id[1]].node_addr == cur_dff.node_addr)
                    {
                        instr_13.data_mem_select2 = 0;
                        instr_13.Operand_Addr2 = luts[*(cur_dff.in_net_from_id.begin() + 1)].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[1] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[1] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_13.data_mem_select2 = 0;
                        else
                            instr_13.data_mem_select2 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[1] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(35, 27);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_13.Operand_Addr2 = dffs[cur_dff.in_net_from_id[1] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_13.data_mem_select2 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[1]);
                        instr_13.Operand_Addr2 = it->second;
                    }
                    instr_13.value2 = (cur_dff.assignsig_condsig.begin() + 1)->second.second;
                    instr_13.T2 = stoi((cur_dff.assignsig_condsig.begin() + 1)->first.c_str());
                    string dff_instr = cat_instr_13(instr_13);
                    processors[cur_processor_id].instr_mem.push_back(dff_instr);
                    dffs[dff_num].res_pos_at_mem = int(processors[cur_processor_id].instr_mem.size()) - 1;
                    processors[cur_processor_id].id_outaddr = pair<int, int>(dff_num + luts.size(), int(processors[cur_processor_id].instr_mem.size()) - 1);
                }
                else if ((cur_dff.assignsig_condsig.begin()->first == "0" || cur_dff.assignsig_condsig.begin()->first == "1") && ((cur_dff.assignsig_condsig.begin() + 1)->first != "0" && (cur_dff.assignsig_condsig.begin() + 1)->first != "1"))
                {
                    Instr_14 instr_14;
                    instr_14.PC_Jump = 0;
                    instr_14.BM_Jump = 0;
                    instr_14.Node_Addr = cur_dff.node_addr;
                    assert(cur_dff.in_net_from_id.size() == 3);
                    if (cur_dff.in_net_from_id[0] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[0]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[0]];
                            if (*(tvs_val->begin()) == 0)
                                instr_14.data_mem_select1 = 0;
                            else
                                instr_14.data_mem_select1 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[0] == INITIAL_JUMP_SIG)
                                instr_14.Operand_Addr1 = INITIAL_JUMP_ADDR;
                            else
                                instr_14.Operand_Addr1 = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[0] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[0] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[0] == 0)
                            instr_14.data_mem_select1 = 0;
                        else
                            instr_14.data_mem_select1 = 1;
                        instr_14.Operand_Addr1 = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[0] < luts.size() && luts[cur_dff.in_net_from_id[0]].node_addr == cur_dff.node_addr)
                    {
                        instr_14.data_mem_select1 = 0;
                        instr_14.Operand_Addr1 = luts[cur_dff.in_net_from_id[0]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[0] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[0] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_14.data_mem_select1 = 0;
                        else
                            instr_14.data_mem_select1 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[0] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(17, 9);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_14.Operand_Addr1 = dffs[cur_dff.in_net_from_id[0] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_14.data_mem_select1 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[0]);
                        instr_14.Operand_Addr1 = it->second;
                    }
                    instr_14.value1 = cur_dff.assignsig_condsig.begin()->second.second;
                    instr_14.T1 = stoi(cur_dff.assignsig_condsig.begin()->first.c_str());
                    if (cur_dff.in_net_from_id[2] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[2]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[2]];
                            if (*(tvs_val->begin()) == 0)
                                instr_14.data_mem_select2 = 0;
                            else
                                instr_14.data_mem_select2 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[2] == INITIAL_JUMP_SIG)
                                instr_14.Operand_Addr2 = INITIAL_JUMP_ADDR;
                            else
                                instr_14.Operand_Addr2 = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[2] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[2] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[2] == 0)
                            instr_14.data_mem_select2 = 0;
                        else
                            instr_14.data_mem_select2 = 1;
                        instr_14.Operand_Addr2 = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[2] < luts.size() && luts[cur_dff.in_net_from_id[2]].node_addr == cur_dff.node_addr)
                    {
                        instr_14.data_mem_select2 = 0;
                        instr_14.Operand_Addr2 = luts[cur_dff.in_net_from_id[2]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[2] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[2] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_14.data_mem_select2 = 0;
                        else
                            instr_14.data_mem_select2 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[2] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(35, 27);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_14.Operand_Addr2 = dffs[cur_dff.in_net_from_id[2] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_14.data_mem_select2 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[2]);
                        instr_14.Operand_Addr2 = it->second;
                    }
                    instr_14.value2 = (cur_dff.assignsig_condsig.begin() + 1)->second.second;
                    if (cur_dff.in_net_from_id[1] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[1]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[1]];
                            if (*(tvs_val->begin()) == 0)
                                instr_14.data_mem_select3 = 0;
                            else
                                instr_14.data_mem_select3 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[1] == INITIAL_JUMP_SIG)
                                instr_14.T2_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_14.T2_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[1] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[1] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[1] == 0)
                            instr_14.data_mem_select3 = 0;
                        else
                            instr_14.data_mem_select3 = 1;
                        instr_14.T2_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[1] < luts.size() && luts[cur_dff.in_net_from_id[1]].node_addr == cur_dff.node_addr)
                    {
                        instr_14.data_mem_select3 = 0;
                        instr_14.T2_Addr = luts[cur_dff.in_net_from_id[1]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[1] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[1] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_14.data_mem_select3 = 0;
                        else
                            instr_14.data_mem_select3 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[1] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(26, 18);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_14.T2_Addr = dffs[cur_dff.in_net_from_id[1] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_14.data_mem_select3 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[1]);
                        instr_14.T2_Addr = it->second;
                    }
                    string dff_instr = cat_instr_14(instr_14);
                    processors[cur_processor_id].instr_mem.push_back(dff_instr);
                    dffs[dff_num].res_pos_at_mem = int(processors[cur_processor_id].instr_mem.size()) - 1;
                    processors[cur_processor_id].id_outaddr = pair<int, int>(dff_num + luts.size(), int(processors[cur_processor_id].instr_mem.size()) - 1);
                }
                else if ((cur_dff.assignsig_condsig.begin()->first != "0" && cur_dff.assignsig_condsig.begin()->first != "1") && ((cur_dff.assignsig_condsig.begin() + 1)->first == "0" || (cur_dff.assignsig_condsig.begin() + 1)->first == "1"))
                {
                    Instr_15 instr_15;
                    instr_15.PC_Jump = 0;
                    instr_15.BM_Jump = 0;
                    instr_15.Node_Addr = cur_dff.node_addr;
                    assert(cur_dff.in_net_from_id.size() == 3);
                    if (cur_dff.in_net_from_id[1] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[1]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[1]];
                            if (*(tvs_val->begin()) == 0)
                                instr_15.data_mem_select1 = 0;
                            else
                                instr_15.data_mem_select1 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[1] == INITIAL_JUMP_SIG)
                                instr_15.Operand_Addr1 = INITIAL_JUMP_ADDR;
                            else
                                instr_15.Operand_Addr1 = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[1] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[1] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[1] == 0)
                            instr_15.data_mem_select1 = 0;
                        else
                            instr_15.data_mem_select1 = 1;
                        instr_15.Operand_Addr1 = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[1] < luts.size() && luts[cur_dff.in_net_from_id[1]].node_addr == cur_dff.node_addr)
                    {
                        instr_15.data_mem_select1 = 0;
                        instr_15.Operand_Addr1 = luts[cur_dff.in_net_from_id[1]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[1] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[1] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_15.data_mem_select1 = 0;
                        else
                            instr_15.data_mem_select1 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[1] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(17, 9);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_15.Operand_Addr1 = dffs[cur_dff.in_net_from_id[1] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_15.data_mem_select1 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[1]);
                        instr_15.Operand_Addr1 = it->second;
                    }
                    instr_15.value1 = cur_dff.assignsig_condsig.begin()->second.second;
                    if (cur_dff.in_net_from_id[0] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[0]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[0]];
                            if (*(tvs_val->begin()) == 0)
                                instr_15.data_mem_select2 = 0;
                            else
                                instr_15.data_mem_select2 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[0] == INITIAL_JUMP_SIG)
                                instr_15.T1_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_15.T1_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[0] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[0] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[0] == 0)
                            instr_15.data_mem_select2 = 0;
                        else
                            instr_15.data_mem_select2 = 1;
                        instr_15.T1_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[0] < luts.size() && luts[cur_dff.in_net_from_id[0]].node_addr == cur_dff.node_addr)
                    {
                        instr_15.data_mem_select2 = 0;
                        instr_15.T1_Addr = luts[cur_dff.in_net_from_id[0]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[0] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[0] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_15.data_mem_select2 = 0;
                        else
                            instr_15.data_mem_select2 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[0] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(8, 0);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_15.T1_Addr = dffs[cur_dff.in_net_from_id[0] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_15.data_mem_select2 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[0]);
                        instr_15.T1_Addr = it->second;
                    }
                    if (cur_dff.in_net_from_id[2] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[2]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[2]];
                            if (*(tvs_val->begin()) == 0)
                                instr_15.data_mem_select3 = 0;
                            else
                                instr_15.data_mem_select3 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[2] == INITIAL_JUMP_SIG)
                                instr_15.Operand_Addr2 = INITIAL_JUMP_ADDR;
                            else
                                instr_15.Operand_Addr2 = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[2] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[2] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[2] == 0)
                            instr_15.data_mem_select3 = 0;
                        else
                            instr_15.data_mem_select3 = 1;
                        instr_15.Operand_Addr2 = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[2] < luts.size() && luts[cur_dff.in_net_from_id[2]].node_addr == cur_dff.node_addr)
                    {
                        instr_15.data_mem_select3 = 0;
                        instr_15.Operand_Addr2 = luts[cur_dff.in_net_from_id[2]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[2] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[2] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_15.data_mem_select3 = 0;
                        else
                            instr_15.data_mem_select3 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[2] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(35, 27);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_15.Operand_Addr2 = dffs[cur_dff.in_net_from_id[2] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_15.data_mem_select3 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[2]);
                        instr_15.Operand_Addr2 = it->second;
                    }
                    instr_15.value2 = (cur_dff.assignsig_condsig.begin() + 1)->second.second;
                    instr_15.T2 = stoi((cur_dff.assignsig_condsig.begin() + 1)->first.c_str());
                    string dff_instr = cat_instr_15(instr_15);
                    processors[cur_processor_id].instr_mem.push_back(dff_instr);
                    dffs[dff_num].res_pos_at_mem = int(processors[cur_processor_id].instr_mem.size()) - 1;
                    processors[cur_processor_id].id_outaddr = pair<int, int>(dff_num + luts.size(), int(processors[cur_processor_id].instr_mem.size()) - 1);
                }
                else if ((cur_dff.assignsig_condsig.begin()->first != "0" && cur_dff.assignsig_condsig.begin()->first != "1") && ((cur_dff.assignsig_condsig.begin() + 1)->first != "0" && (cur_dff.assignsig_condsig.begin() + 1)->first != "1"))
                {
                    Instr_16 instr_16;
                    instr_16.PC_Jump = 0;
                    instr_16.BM_Jump = 0;
                    instr_16.Node_Addr = cur_dff.node_addr;
                    assert(cur_dff.in_net_from_id.size() == 4);
                    if (cur_dff.in_net_from_id[1] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[1]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[1]];
                            if (*(tvs_val->begin()) == 0)
                                instr_16.data_mem_select1 = 0;
                            else
                                instr_16.data_mem_select1 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[1] == INITIAL_JUMP_SIG)
                                instr_16.Operand_Addr1 = INITIAL_JUMP_ADDR;
                            else
                                instr_16.Operand_Addr1 = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[1] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[1] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[1] == 0)
                            instr_16.data_mem_select1 = 0;
                        else
                            instr_16.data_mem_select1 = 1;
                        instr_16.Operand_Addr1 = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[1] < luts.size() && luts[cur_dff.in_net_from_id[1]].node_addr == cur_dff.node_addr)
                    {
                        instr_16.data_mem_select1 = 0;
                        instr_16.Operand_Addr1 = luts[cur_dff.in_net_from_id[1]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[1] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[1] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_16.data_mem_select1 = 0;
                        else
                            instr_16.data_mem_select1 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[1] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(17, 9);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_16.Operand_Addr1 = dffs[cur_dff.in_net_from_id[1] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_16.data_mem_select1 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[1]);
                        instr_16.Operand_Addr1 = it->second;
                    }
                    instr_16.value1 = cur_dff.assignsig_condsig.begin()->second.second;
                    if (cur_dff.in_net_from_id[0] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[0]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[0]];
                            if (*(tvs_val->begin()) == 0)
                                instr_16.data_mem_select2 = 0;
                            else
                                instr_16.data_mem_select2 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[0] == INITIAL_JUMP_SIG)
                                instr_16.T1_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_16.T1_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[0] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[0] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[0] == 0)
                            instr_16.data_mem_select2 = 0;
                        else
                            instr_16.data_mem_select2 = 1;
                        instr_16.T1_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[0] < luts.size() && luts[cur_dff.in_net_from_id[0]].node_addr == cur_dff.node_addr)
                    {
                        instr_16.data_mem_select2 = 0;
                        instr_16.T1_Addr = luts[cur_dff.in_net_from_id[0]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[0] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[0] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_16.data_mem_select2 = 0;
                        else
                            instr_16.data_mem_select2 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[0] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(8, 0);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_16.T1_Addr = dffs[cur_dff.in_net_from_id[0] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_16.data_mem_select2 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[0]);
                        instr_16.T1_Addr = it->second;
                    }
                    if (cur_dff.in_net_from_id[3] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[3]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[3]];
                            if (*(tvs_val->begin()) == 0)
                                instr_16.data_mem_select3 = 0;
                            else
                                instr_16.data_mem_select3 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[3] == INITIAL_JUMP_SIG)
                                instr_16.Operand_Addr2 = INITIAL_JUMP_ADDR;
                            else
                                instr_16.Operand_Addr2 = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[3] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[3] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[3] == 0)
                            instr_16.data_mem_select3 = 0;
                        else
                            instr_16.data_mem_select3 = 1;
                        instr_16.Operand_Addr2 = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[3] < luts.size() && luts[cur_dff.in_net_from_id[3]].node_addr == cur_dff.node_addr)
                    {
                        instr_16.data_mem_select3 = 0;
                        instr_16.Operand_Addr2 = luts[cur_dff.in_net_from_id[3]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[3] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[3] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_16.data_mem_select3 = 0;
                        else
                            instr_16.data_mem_select3 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[3] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(35, 27);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_16.Operand_Addr2 = dffs[cur_dff.in_net_from_id[3] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_16.data_mem_select3 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[3]);
                        instr_16.Operand_Addr2 = it->second;
                    }
                    instr_16.value2 = (cur_dff.assignsig_condsig.begin() + 1)->second.second;
                    if (cur_dff.in_net_from_id[2] == -1)
                    {
                        if (vcd_values.find(cur_dff.in_net_from_info[2]) != vcd_values.end())
                        {
                            vector<short> *tvs_val = vcd_values[cur_dff.in_net_from_info[2]];
                            if (*(tvs_val->begin()) == 0)
                                instr_16.data_mem_select4 = 0;
                            else
                                instr_16.data_mem_select4 = 1;
                            // for input initial signal INITIAL_JUMP_SIG changing
                            if (cur_dff.in_net_from_info[2] == INITIAL_JUMP_SIG)
                                instr_16.T2_Addr = INITIAL_JUMP_ADDR;
                            else
                                instr_16.T2_Addr = MEM_DEPTH - 1;
                        }
                        else
                        {
                            std::cout << "ERROR: No initial info of signal " << cur_dff.in_net_from_info[2] << " (DFF " << dff_num << ")"
                                      << " in vcd file!" << endl;
                        }
                    }
                    else if (cur_dff.in_net_from_id[2] == -2)
                    {
                        if (cur_dff.in_net_from_pos_at_level[2] == 0)
                            instr_16.data_mem_select4 = 0;
                        else
                            instr_16.data_mem_select4 = 1;
                        instr_16.T2_Addr = MEM_DEPTH - 1;
                    }
                    else if (cur_dff.in_net_from_id[2] < luts.size() && luts[cur_dff.in_net_from_id[2]].node_addr == cur_dff.node_addr)
                    {
                        instr_16.data_mem_select4 = 0;
                        instr_16.T2_Addr = luts[cur_dff.in_net_from_id[2]].res_pos_at_mem;
                    }
                    else if (cur_dff.in_net_from_id[2] >= luts.size())
                    {
                        if (dffs[cur_dff.in_net_from_id[2] - luts.size()].node_addr == cur_dff.node_addr)
                            instr_16.data_mem_select4 = 0;
                        else
                            instr_16.data_mem_select4 = 1;
                        blank_addr ba;
                        ba.dff_num = cur_dff.in_net_from_id[2] - luts.size();
                        ba.Node_Node_Addr = cur_dff.node_addr;
                        ba.Node_Instr_Addr = processors[cur_processor_id].instr_mem.size();
                        ba.Replaced_Instr_Range = pair<int, int>(26, 18);
                        blank_addrs[dff_num + luts.size()].push_back(ba);
                        instr_16.T2_Addr = dffs[cur_dff.in_net_from_id[2] - luts.size()].res_pos_at_mem;
                    }
                    else
                    {
                        instr_16.data_mem_select4 = 1;
                        auto it = from_id_opr_addr.find(cur_dff.in_net_from_id[2]);
                        instr_16.T2_Addr = it->second;
                    }
                    string dff_instr = cat_instr_16(instr_16);
                    processors[cur_processor_id].instr_mem.push_back(dff_instr);
                    dffs[dff_num].res_pos_at_mem = int(processors[cur_processor_id].instr_mem.size()) - 1;
                    processors[cur_processor_id].id_outaddr = pair<int, int>(dff_num + luts.size(), int(processors[cur_processor_id].instr_mem.size()) - 1);
                }
            }
        }
    }


    // Instruction for DFF Value Updating



    // Instruction for Jumping
    for (auto it = processors.begin(); it != processors.end(); it++)
    {
        Instr_3 instr_3;
        instr_3.PC_Jump = 1;
        instr_3.BM_Jump = 0;
        instr_3.Node_Addr.push_back(it->first / N_PROCESSORS_PER_CLUSTER);
        instr_3.Node_Addr.push_back(it->first % N_PROCESSORS_PER_CLUSTER);
        instr_3.Static_Binary_Value = "0";
        string lut_instr_3 = cat_instr_3(instr_3);
        processors[it->first].instr_mem.insert(processors[it->first].instr_mem.end(), 1, lut_instr_3);
    }

    return tt_instr_mem;
}



string toBinary(int n)
{
    string r;
    while (n != 0)
    {
        r += (n % 2 == 0 ? "0" : "1");
        n /= 2;
    }
    reverse(r.begin(), r.end());
    return r;
}

string HextoBinary(string HexDigit)
{
    string BinDigit;
    for (int i = 0; i < HexDigit.length(); i++)
    {
        char e = HexDigit[i];
        if (e >= 'a' && e <= 'f')
        {
            int a = static_cast<int>(e - 'a' + 10);
            switch (a)
            {
            case 10:
                BinDigit += "1010";
                break;
            case 11:
                BinDigit += "1011";
                break;
            case 12:
                BinDigit += "1100";
                break;
            case 13:
                BinDigit += "1101";
                break;
            case 14:
                BinDigit += "1110";
                break;
            case 15:
                BinDigit += "1111";
                break;
            }
        }
        else if (isdigit(e))
        {
            int b = static_cast<int>(e - '0');
            switch (b)
            {
            case 0:
                BinDigit += "0000";
                break;
            case 1:
                BinDigit += "0001";
                break;
            case 2:
                BinDigit += "0010";
                break;
            case 3:
                BinDigit += "0011";
                break;
            case 4:
                BinDigit += "0100";
                break;
            case 5:
                BinDigit += "0101";
                break;
            case 6:
                BinDigit += "0110";
                break;
            case 7:
                BinDigit += "0111";
                break;
            case 8:
                BinDigit += "1000";
                break;
            case 9:
                BinDigit += "1001";
                break;
            }
        }
    }
    return BinDigit;
}

int Sel_Exter_Datamem(int &n)
{
    switch (n)
    {
    case 1:
        return Exter_Datamem_3;
        break;
    case 2:
        return Exter_Datamem_2;
        break;
    case 3:
        return Exter_Datamem_1;
        break;
    case 4:
        return Exter_Datamem_0;
        break;
    }
}

string cat_instr_1(Instr_1 &instr_1)
{
    string LUT_Value = HextoBinary(instr_1.LUT_Value);
    int cur_node_addr = instr_1.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_1.Node_Addr[1];
    int filling_num = 4 - instr_1.Operand_Addr.size();
    for (int i = 0; i < filling_num; i++)
    {
        instr_1.Operand_Addr.insert(instr_1.Operand_Addr.begin(), MEM_DEPTH - 1);
    }
    string cat_instr_1;
    stringstream ss;
    ss << "0000" << instr_1.PC_Jump << instr_1.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << "0" << bitset<16>(LUT_Value) << bitset<4>(instr_1.Data_Mem_Select)
       << bitset<9>(toBinary(instr_1.Operand_Addr[0])) << bitset<9>(toBinary(instr_1.Operand_Addr[1])) << bitset<9>(toBinary(instr_1.Operand_Addr[2])) << bitset<9>(toBinary(instr_1.Operand_Addr[3]));
    cat_instr_1 = ss.str();
    return cat_instr_1;
}

string cat_instr_2(Instr_2 &instr_2)
{
    int cur_node_addr = instr_2.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_2.Node_Addr[1];
    string cat_instr_2;
    stringstream ss;
    ss << "0001" << instr_2.PC_Jump << instr_2.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(20, '0') << instr_2.Data_Mem_Select << string(27, '0') << bitset<9>(toBinary(instr_2.Operand_Addr));
    cat_instr_2 = ss.str();
    return cat_instr_2;
}

string cat_instr_3(Instr_3 &instr_3)
{
    int cur_node_addr = instr_3.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_3.Node_Addr[1];
    string cat_instr_3;
    stringstream ss;
    ss << "0010" << instr_3.PC_Jump << instr_3.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << instr_3.Static_Binary_Value << string(56, '0');
    cat_instr_3 = ss.str();
    return cat_instr_3;
}

string cat_instr_4(Instr_4 &instr_4)
{
    int cur_node_addr = instr_4.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_4.Node_Addr[1];
    string cat_instr_4;
    stringstream ss;
    ss << "0011" << instr_4.PC_Jump << instr_4.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(57, '0');
    cat_instr_4 = ss.str();
    return cat_instr_4;
}

string cat_instr_5(Instr_5 &instr_5)
{
    int cur_node_addr = instr_5.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_5.Node_Addr[1];
    string cat_instr_5;
    stringstream ss;
    ss << "0100" << instr_5.PC_Jump << instr_5.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(13, '0') << "00" << instr_5.data_mem_select << "0" << instr_5.value << "0" << instr_5.F << instr_5.T << string(18, '0') << bitset<9>(toBinary(instr_5.Operand_Addr)) << string(9, '0');
    cat_instr_5 = ss.str();
    return cat_instr_5;
}

string cat_instr_6(Instr_6 &instr_6)
{
    int cur_node_addr = instr_6.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_6.Node_Addr[1];
    string cat_instr_6;
    stringstream ss;
    ss << "0101" << instr_6.PC_Jump << instr_6.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(13, '0') << "0" << instr_6.data_mem_select2 << instr_6.data_mem_select1 << "0" << instr_6.value << "00" << instr_6.T << string(9, '0') << bitset<9>(toBinary(instr_6.F_Addr)) << bitset<9>(toBinary(instr_6.Operand_Addr)) << string(9, '0');
    cat_instr_6 = ss.str();
    return cat_instr_6;
}

string cat_instr_7(Instr_7 &instr_7)
{
    int cur_node_addr = instr_7.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_7.Node_Addr[1];
    string cat_instr_7;
    stringstream ss;
    ss << "0110" << instr_7.PC_Jump << instr_7.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(13, '0') << "0" << instr_7.data_mem_select3 << instr_7.data_mem_select1 << instr_7.data_mem_select2 << instr_7.value << "000" << string(9, '0') << bitset<9>(toBinary(instr_7.F_Addr)) << bitset<9>(toBinary(instr_7.Operand_Addr)) << bitset<9>(toBinary(instr_7.T_Addr));
    cat_instr_7 = ss.str();
    return cat_instr_7;
}

string cat_instr_8(Instr_8 &instr_8)
{
    int cur_node_addr = instr_8.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_8.Node_Addr[1];
    string cat_instr_8;
    stringstream ss;
    ss << "0111" << instr_8.PC_Jump << instr_8.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(13, '0') << "00" << instr_8.data_mem_select1 << instr_8.data_mem_select2 << instr_8.value << "0" << instr_8.F << "0" << string(18, '0') << bitset<9>(toBinary(instr_8.Operand_Addr)) << bitset<9>(toBinary(instr_8.T_Addr));
    cat_instr_8 = ss.str();
    return cat_instr_8;
}

string cat_instr_9(Instr_9 &instr_9)
{
    int cur_node_addr = instr_9.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_9.Node_Addr[1];
    string cat_instr_9;
    stringstream ss;
    ss << "1000" << instr_9.PC_Jump << instr_9.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(20, '0') << instr_9.im << string(36, '0');
    cat_instr_9 = ss.str();
    return cat_instr_9;
}

string cat_instr_10(Instr_10 &instr_10)
{
    int cur_node_addr = instr_10.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_10.Node_Addr[1];
    string cat_instr_10;
    stringstream ss;
    ss << "1001" << instr_10.PC_Jump << instr_10.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(13, '0') << "000" << instr_10.data_mem_select << string(31, '0') << bitset<9>(toBinary(instr_10.Addr));
    cat_instr_10 = ss.str();
    return cat_instr_10;
}

string cat_instr_11(Instr_11 &instr_11)
{
    int cur_node_addr = instr_11.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_11.Node_Addr[1];
    string cat_instr_11;
    stringstream ss;
    ss << "1010" << instr_11.PC_Jump << instr_11.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(13, '0') << "00" << instr_11.data_mem_select << "0" << instr_11.value << "00" << instr_11.T << string(18, '0') << bitset<9>(toBinary(instr_11.Operand_Addr)) << string(9, '0');
    cat_instr_11 = ss.str();
    return cat_instr_11;
}

string cat_instr_12(Instr_12 &instr_12)
{
    int cur_node_addr = instr_12.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_12.Node_Addr[1];
    string cat_instr_12;
    stringstream ss;
    ss << "1011" << instr_12.PC_Jump << instr_12.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(13, '0') << "00" << instr_12.data_mem_select1 << instr_12.data_mem_select2 << instr_12.value << "000" << string(18, '0') << bitset<9>(toBinary(instr_12.Operand_Addr)) << bitset<9>(toBinary(instr_12.T_Addr));
    cat_instr_12 = ss.str();
    return cat_instr_12;
}

string cat_instr_13(Instr_13 &instr_13)
{
    int cur_node_addr = instr_13.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_13.Node_Addr[1];
    string cat_instr_13;
    stringstream ss;
    ss << "1100" << instr_13.PC_Jump << instr_13.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(13, '0') << instr_13.data_mem_select2 << "0" << instr_13.data_mem_select1 << "0" << instr_13.value1 << instr_13.value2 << instr_13.T2 << instr_13.T1 << bitset<9>(toBinary(instr_13.Operand_Addr2)) << string(9, '0') << bitset<9>(toBinary(instr_13.Operand_Addr1)) << string(9, '0');
    cat_instr_13 = ss.str();
    return cat_instr_13;
}

string cat_instr_14(Instr_14 &instr_14)
{
    int cur_node_addr = instr_14.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_14.Node_Addr[1];
    string cat_instr_14;
    stringstream ss;
    ss << "1101" << instr_14.PC_Jump << instr_14.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(13, '0') << instr_14.data_mem_select2 << instr_14.data_mem_select3 << instr_14.data_mem_select1 << "0" << instr_14.value1 << instr_14.value2 << "0" << instr_14.T1 << bitset<9>(toBinary(instr_14.Operand_Addr2)) << bitset<9>(toBinary(instr_14.T2_Addr)) << bitset<9>(toBinary(instr_14.Operand_Addr1)) << string(9, '0');
    cat_instr_14 = ss.str();
    return cat_instr_14;
}

string cat_instr_15(Instr_15 &instr_15)
{
    int cur_node_addr = instr_15.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_15.Node_Addr[1];
    string cat_instr_15;
    stringstream ss;
    ss << "1110" << instr_15.PC_Jump << instr_15.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(13, '0') << instr_15.data_mem_select3 << "0" << instr_15.data_mem_select1 << instr_15.data_mem_select2 << instr_15.value1 << instr_15.value2 << instr_15.T2 << "0" << bitset<9>(toBinary(instr_15.Operand_Addr2)) << string(9, '0') << bitset<9>(toBinary(instr_15.Operand_Addr1)) << bitset<9>(toBinary(instr_15.T1_Addr));
    cat_instr_15 = ss.str();
    return cat_instr_15;
}

string cat_instr_16(Instr_16 &instr_16)
{
    int cur_node_addr = instr_16.Node_Addr[0] * N_PROCESSORS_PER_CLUSTER + instr_16.Node_Addr[1];
    string cat_instr_16;
    stringstream ss;
    ss << "1111" << instr_16.PC_Jump << instr_16.BM_Jump << string(9, '0') << bitset<8>(toBinary(cur_node_addr)) << string(13, '0') << instr_16.data_mem_select3 << instr_16.data_mem_select4 << instr_16.data_mem_select1 << instr_16.data_mem_select2 << instr_16.value1 << instr_16.value2 << "00" << bitset<9>(toBinary(instr_16.Operand_Addr2)) << bitset<9>(toBinary(instr_16.T2_Addr)) << bitset<9>(toBinary(instr_16.Operand_Addr1)) << bitset<9>(toBinary(instr_16.T1_Addr));
    cat_instr_16 = ss.str();
    return cat_instr_16;
}
