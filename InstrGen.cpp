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

    vector<vector<Instr>> tt_instr_mem(N_PROCESSORS, vector<Instr>(INS_DEPTH));
    vector<int> tt_instr_mem_cnt(N_PROCESSORS, 0);

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
                new_instr.Opcode      = LUT_CONFIG;
                new_instr.Jump        = 00;
                new_instr.Value_Data.push_back(stoi(lut_res_binary));
                for (auto i = cur_lut.in_net_from_addr.begin(); i != cur_lut.in_net_from_addr.end(); i++)
                {
                    new_instr.Datamem_Sel.push_back(i->first);
                }
                for (auto i = cur_lut.in_net_from_addr.begin(); i != cur_lut.in_net_from_addr.end(); i++)
                {
                    new_instr.Operand_Addr.push_back(i->second);
                    if (i->second != MEM_DEPTH - 1 && i->second >= cur_instr_mem_cnt)
                        pushaddr = ((i->second + 1) > pushaddr) ? i->second + 1 : pushaddr;
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
                pair<int, int> cur_node_addr = cur_dff.node_addr;
                int tt_instr_mem_index = cur_node_addr.first * N_PROCESSORS_PER_CLUSTER + cur_node_addr.second;
                int cur_instr_mem_cnt = tt_instr_mem_cnt[tt_instr_mem_index];
                int pushaddr = cur_instr_mem_cnt;
                auto assignsig_condsig = cur_dff.assignsig_condsig;
                Instr new_instr;
                new_instr.Opcode = FF_CONFIG;
                new_instr.Jump = 00;
                if (cur_dff.type == 0)
                {
                    if (assignsig_condsig[0].first == "0" || assignsig_condsig[0].first == "1")
                    {
                        // --------------------------------------------------------------------------------------------------------
                        // |                     pseudocode                        |                assignsig_condsig             |
                        // --------------------------------------------------------------------------------------------------------
                        // |                 dff_out <= "0"/"1"                    |   <"0"/"1"        , <"             ", -1 >>  |
                        // --------------------------------------------------------------------------------------------------------
                        new_instr.Value_Data   = {DFF_Direct_Assign_0, 0, 0, 0, stoi(assignsig_condsig[0].first)};
                        new_instr.Datamem_Sel  = {0, 0, 0, 0};
                        new_instr.Operand_Addr = {0, 0, 0, 0};
                    }
                    else
                    {
                        // --------------------------------------------------------------------------------------------------------
                        // |                     pseudocode                        |                assignsig_condsig             |
                        // --------------------------------------------------------------------------------------------------------
                        // |             dff_out <= dff_in_ports[0]                |   <dff_in_ports[0], <"             ", -1 >>  |
                        // --------------------------------------------------------------------------------------------------------
                        new_instr.Value_Data   = {DFF_Direct_Assign_1, 0, 0, 0, 0};
                        new_instr.Datamem_Sel  = {0, 0, 0, cur_dff.in_net_from_addr[0].first};
                        new_instr.Operand_Addr = {0, 0, 0, cur_dff.in_net_from_addr[0].second};
                    }
                }
                else if (cur_dff.type == 1)
                {
                    if (assignsig_condsig[0].first == "0" || assignsig_condsig[0].first == "1")
                    {
                        // --------------------------------------------------------------------------------------------------------
                        // |                     pseudocode                        |                assignsig_condsig             |
                        // --------------------------------------------------------------------------------------------------------
                        // | if (dff_in_ports[0])     dff_out <= "0"/"1"           |   <"0"/"1"        , <dff_in_ports[0], 0/1>>  |
                        // --------------------------------------------------------------------------------------------------------
                        new_instr.Value_Data   = {DFF_If_0, assignsig_condsig[0].second[0].second, 0, 0, stoi(assignsig_condsig[0].first)};
                        new_instr.Datamem_Sel  = {0, 0, cur_dff.in_net_from_addr[0].first, 0};
                        new_instr.Operand_Addr = {0, 0, cur_dff.in_net_from_addr[0].second, 0};
                    }
                    else
                    {
                        // --------------------------------------------------------------------------------------------------------
                        // |                     pseudocode                        |                assignsig_condsig             |
                        // --------------------------------------------------------------------------------------------------------
                        // | if (dff_in_ports[1])     dff_out <= dff_in_ports[0]   |   <dff_in_ports[0], <dff_in_ports[1], 0/1>>  |
                        // --------------------------------------------------------------------------------------------------------
                        new_instr.Value_Data   = {DFF_If_1, assignsig_condsig[0].second[0].second, 0, 0, 0};
                        new_instr.Datamem_Sel  = {0, 0, cur_dff.in_net_from_addr[1].first, cur_dff.in_net_from_addr[1].first};
                        new_instr.Operand_Addr = {0, 0, cur_dff.in_net_from_addr[0].second, cur_dff.in_net_from_addr[0].second};
                    }
                }
                else if (cur_dff.type == 2)
                {
                    if (assignsig_condsig[0].first == "0" || assignsig_condsig[0].first == "1")
                    {
                        if (assignsig_condsig[1].first == "0" || assignsig_condsig[1].first == "1")
                        {
                            // --------------------------------------------------------------------------------------------------------
                            // |                     pseudocode                        |                assignsig_condsig             |
                            // --------------------------------------------------------------------------------------------------------
                            // | if (dff_in_ports[0])     dff_out <= "0"/"1"           |   <"0"/"1"        , <dff_in_ports[0], 0/1>>  |
                            // | else                     dff_out <= "0"/"1"           |   <"0"/"1"        , <"             ", -1 >>  |
                            // --------------------------------------------------------------------------------------------------------
                            new_instr.Value_Data   = {DFF_If_Else_0, assignsig_condsig[0].second[0].second, 0, stoi(assignsig_condsig[1].first), stoi(assignsig_condsig[0].first)};
                            new_instr.Datamem_Sel  = {0, 0, cur_dff.in_net_from_addr[0].first, 0};
                            new_instr.Operand_Addr = {0, 0, cur_dff.in_net_from_addr[0].second, 0};
                        }
                        else
                        {
                            // --------------------------------------------------------------------------------------------------------
                            // |                     pseudocode                        |                assignsig_condsig             |
                            // --------------------------------------------------------------------------------------------------------
                            // | if (dff_in_ports[0])     dff_out <= "0"/"1"           |   <"0"/"1"        , <dff_in_ports[0], 0/1>>  |
                            // | else                     dff_out <= dff_in_ports[1]   |   <dff_in_ports[1], <"             ", -1 >>  |
                            // --------------------------------------------------------------------------------------------------------
                            new_instr.Value_Data   = {DFF_If_Else_1, assignsig_condsig[0].second[0].second, 0, 0, stoi(assignsig_condsig[0].first)};
                            new_instr.Datamem_Sel  = {0, cur_dff.in_net_from_addr[1].first, cur_dff.in_net_from_addr[0].first, 0};
                            new_instr.Operand_Addr = {0, cur_dff.in_net_from_addr[1].second, cur_dff.in_net_from_addr[0].second, 0};
                        }
                    }
                    else
                    {
                        if (assignsig_condsig[1].first == "0" || assignsig_condsig[1].first == "1")
                        {
                            // --------------------------------------------------------------------------------------------------------
                            // |                     pseudocode                        |                assignsig_condsig             |
                            // --------------------------------------------------------------------------------------------------------
                            // | if (dff_in_ports[1])     dff_out <= dff_in_ports[0]   |   <dff_in_ports[0], <dff_in_ports[1], 0/1>>  |
                            // | else                     dff_out <= "0"/"1"           |   <"0"/"1"        , <"             ", -1 >>  |
                            // --------------------------------------------------------------------------------------------------------
                            new_instr.Value_Data   = {DFF_If_Else_3, assignsig_condsig[0].second[0].second, 0, stoi(assignsig_condsig[1].first), 0};
                            new_instr.Datamem_Sel  = {0, 0, cur_dff.in_net_from_addr[1].first, cur_dff.in_net_from_addr[0].first};
                            new_instr.Operand_Addr = {0, 0, cur_dff.in_net_from_addr[1].second, cur_dff.in_net_from_addr[0].second};
                        }
                        else
                        {
                            // --------------------------------------------------------------------------------------------------------
                            // |                     pseudocode                        |                assignsig_condsig             |
                            // --------------------------------------------------------------------------------------------------------
                            // | if (dff_in_ports[1])     dff_out <= dff_in_ports[0]   |   <dff_in_ports[0], <dff_in_ports[1], 0/1>>  |
                            // | else                     dff_out <= dff_in_ports[2]   |   <dff_in_ports[2], <"             ", -1 >>  |
                            // --------------------------------------------------------------------------------------------------------
                            new_instr.Value_Data   = {DFF_If_Else_2, assignsig_condsig[0].second[0].second, 0, 0, 0};
                            new_instr.Datamem_Sel  = {0, cur_dff.in_net_from_addr[2].first, cur_dff.in_net_from_addr[1].first, cur_dff.in_net_from_addr[0].first};
                            new_instr.Operand_Addr = {0, cur_dff.in_net_from_addr[2].second, cur_dff.in_net_from_addr[1].second, cur_dff.in_net_from_addr[0].second};
                        }
                    }
                }
                else if (cur_dff.type == 3)
                {
                    if (assignsig_condsig[0].first == "0" || assignsig_condsig[0].first == "1")
                    {
                        if (assignsig_condsig[1].first == "0" || assignsig_condsig[1].first == "1")
                        {
                            // --------------------------------------------------------------------------------------------------------
                            // |                     pseudocode                        |                assignsig_condsig             |
                            // --------------------------------------------------------------------------------------------------------
                            // | if (dff_in_ports[0])     dff_out <= "0"/"1"           |   <"0"/"1"        , <dff_in_ports[0], 0/1>>  |
                            // | elseif (dff_in_ports[1]) dff_out <= "0"/"1"           |   <"0"/"1"        , <dff_in_ports[1], 0/1>>  |
                            // --------------------------------------------------------------------------------------------------------
                            new_instr.Value_Data   = {DFF_If_Elseif_0, assignsig_condsig[0].second[0].second, assignsig_condsig[1].second[0].second, stoi(assignsig_condsig[1].first), stoi(assignsig_condsig[0].first)};
                            new_instr.Datamem_Sel  = {cur_dff.in_net_from_addr[1].first, 0, cur_dff.in_net_from_addr[0].first, 0};
                            new_instr.Operand_Addr = {cur_dff.in_net_from_addr[1].second, 0, cur_dff.in_net_from_addr[0].second, 0};
                        }
                        else
                        {
                            // --------------------------------------------------------------------------------------------------------
                            // |                     pseudocode                        |                assignsig_condsig             |
                            // --------------------------------------------------------------------------------------------------------
                            // | if (dff_in_ports[0])     dff_out <= "0"/"1"           |   <"0"/"1"        , <dff_in_ports[0], 0/1>>  |
                            // | elseif (dff_in_ports[2]) dff_out <= dff_in_ports[1]   |   <dff_in_ports[1], <dff_in_ports[2], 0/1>>  |
                            // --------------------------------------------------------------------------------------------------------
                            new_instr.Value_Data   = {DFF_If_Elseif_1, assignsig_condsig[0].second[0].second, assignsig_condsig[1].second[0].second, 0, stoi(assignsig_condsig[0].first)};
                            new_instr.Datamem_Sel  = {cur_dff.in_net_from_addr[2].first, cur_dff.in_net_from_addr[1].first, cur_dff.in_net_from_addr[0].first, 0};
                            new_instr.Operand_Addr = {cur_dff.in_net_from_addr[2].second, cur_dff.in_net_from_addr[1].second, cur_dff.in_net_from_addr[0].second, 0};
                        }
                    }
                    else
                    {
                        if (assignsig_condsig[1].first == "0" || assignsig_condsig[1].first == "1")
                        {
                            // --------------------------------------------------------------------------------------------------------
                            // |                     pseudocode                        |                assignsig_condsig             |
                            // --------------------------------------------------------------------------------------------------------
                            // | if (dff_in_ports[1])     dff_out <= dff_in_ports[0]   |   <dff_in_ports[0], <dff_in_ports[1], 0/1>>  |
                            // | elseif (dff_in_ports[2]) dff_out <= "0"/"1"           |   <"0"/"1"        , <dff_in_ports[2], 0/1>>  |
                            // --------------------------------------------------------------------------------------------------------
                            new_instr.Value_Data   = {DFF_If_Elseif_2, assignsig_condsig[0].second[0].second, assignsig_condsig[1].second[0].second, stoi(assignsig_condsig[1].first), 0};
                            new_instr.Datamem_Sel  = {cur_dff.in_net_from_addr[2].first, 0, cur_dff.in_net_from_addr[1].first, cur_dff.in_net_from_addr[0].first};
                            new_instr.Operand_Addr = {cur_dff.in_net_from_addr[2].second, 0, cur_dff.in_net_from_addr[1].second, cur_dff.in_net_from_addr[0].second};
                        }
                        else
                        {
                            // --------------------------------------------------------------------------------------------------------
                            // |                     pseudocode                        |                assignsig_condsig             |
                            // --------------------------------------------------------------------------------------------------------
                            // | if (dff_in_ports[1])     dff_out <= dff_in_ports[0]   |   <dff_in_ports[0], <dff_in_ports[1], 0/1>>  |
                            // | elseif (dff_in_ports[3]) dff_out <= dff_in_ports[2]   |   <dff_in_ports[2], <dff_in_ports[3], 0/1>>  |
                            // --------------------------------------------------------------------------------------------------------
                            new_instr.Value_Data   = {DFF_If_Elseif_3, assignsig_condsig[0].second[0].second, assignsig_condsig[1].second[0].second, 0, 0};
                            new_instr.Datamem_Sel  = {cur_dff.in_net_from_addr[3].first, cur_dff.in_net_from_addr[2].first, cur_dff.in_net_from_addr[1].first, cur_dff.in_net_from_addr[0].first};
                            new_instr.Operand_Addr = {cur_dff.in_net_from_addr[3].second, cur_dff.in_net_from_addr[2].second, cur_dff.in_net_from_addr[1].second, cur_dff.in_net_from_addr[0].second};
                        }
                    }
                }
                else  // cur_dff.type == 4
                {
                    if (assignsig_condsig[0].first == "0" || assignsig_condsig[0].first == "1")
                    {
                        if (assignsig_condsig[1].first == "0" || assignsig_condsig[1].first == "1")
                        {
                            // --------------------------------------------------------------------------------------------------------
                            // |                     pseudocode                        |                assignsig_condsig             |
                            // --------------------------------------------------------------------------------------------------------
                            // | if (dff_in_ports[0])                                  |   <"0"/"1"        , <dff_in_ports[0], 0/1>   |
                            // | if (dff_in_ports[1])     dff_out <= "0"/"1"           |                     <dff_in_ports[1], 0/1>>  |
                            // | else                     dff_out <= "0"/"1"           |   <"0"/"1"        , <"             ", -1 >>  |
                            // --------------------------------------------------------------------------------------------------------
                            new_instr.Value_Data   = {DFF_If_Ifelse_0, assignsig_condsig[0].second[0].second, assignsig_condsig[0].second[1].second, stoi(assignsig_condsig[1].first), stoi(assignsig_condsig[0].first)};
                            new_instr.Datamem_Sel  = {cur_dff.in_net_from_addr[1].first, 0, cur_dff.in_net_from_addr[0].first, 0};
                            new_instr.Operand_Addr = {cur_dff.in_net_from_addr[1].second, 0, cur_dff.in_net_from_addr[0].second, 0};
                        }
                        else
                        {
                            // --------------------------------------------------------------------------------------------------------
                            // |                     pseudocode                        |                assignsig_condsig             |
                            // --------------------------------------------------------------------------------------------------------
                            // | if (dff_in_ports[0])                                  |   <"0"/"1"        , <dff_in_ports[0], 0/1>   |
                            // | if (dff_in_ports[1])     dff_out <= "0"/"1"           |                     <dff_in_ports[1], 0/1>>  |
                            // | else                     dff_out <= dff_in_ports[2]   |   <dff_in_ports[2], <"             ", -1 >>  |
                            // --------------------------------------------------------------------------------------------------------
                            new_instr.Value_Data   = {DFF_If_Ifelse_1, assignsig_condsig[0].second[0].second, assignsig_condsig[0].second[1].second, 0, stoi(assignsig_condsig[0].first)};
                            new_instr.Datamem_Sel  = {cur_dff.in_net_from_addr[1].first, cur_dff.in_net_from_addr[2].first, cur_dff.in_net_from_addr[0].first, 0};
                            new_instr.Operand_Addr = {cur_dff.in_net_from_addr[1].second, cur_dff.in_net_from_addr[2].second, cur_dff.in_net_from_addr[0].second, 0};
                        }
                    }
                    else
                    {
                        if (assignsig_condsig[1].first == "0" || assignsig_condsig[1].first == "1")
                        {
                            // --------------------------------------------------------------------------------------------------------
                            // |                     pseudocode                        |                assignsig_condsig             |
                            // --------------------------------------------------------------------------------------------------------
                            // | if (dff_in_ports[1])                                  |   <dff_in_ports[0], <dff_in_ports[1], 0/1>   |
                            // | if (dff_in_ports[2])     dff_out <= dff_in_ports[0]   |                     <dff_in_ports[2], 0/1>>  |
                            // | else                     dff_out <= "0"/"1"           |   <"0"/"1"        , <"             ", -1 >>  |
                            // --------------------------------------------------------------------------------------------------------
                            new_instr.Value_Data   = {DFF_If_Ifelse_2, assignsig_condsig[0].second[0].second, assignsig_condsig[0].second[1].second, stoi(assignsig_condsig[1].first), 0};
                            new_instr.Datamem_Sel  = {cur_dff.in_net_from_addr[2].first, 0, cur_dff.in_net_from_addr[1].first, cur_dff.in_net_from_addr[0].first};
                            new_instr.Operand_Addr = {cur_dff.in_net_from_addr[2].second, 0, cur_dff.in_net_from_addr[1].second, cur_dff.in_net_from_addr[0].second};
                        }
                        else
                        {
                            // --------------------------------------------------------------------------------------------------------
                            // |                     pseudocode                        |                assignsig_condsig             |
                            // --------------------------------------------------------------------------------------------------------
                            // | if (dff_in_ports[1])                                  |   <dff_in_ports[0], <dff_in_ports[1], 0/1>   |
                            // | if (dff_in_ports[2])     dff_out <= dff_in_ports[0]   |                     <dff_in_ports[2], 0/1>>  |
                            // | else                     dff_out <= dff_in_ports[3]   |   <dff_in_ports[3], <"             ", -1 >>  |
                            // --------------------------------------------------------------------------------------------------------
                            new_instr.Value_Data   = {DFF_If_Ifelse_3, assignsig_condsig[0].second[0].second, assignsig_condsig[0].second[1].second, 0, 0};
                            new_instr.Datamem_Sel  = {cur_dff.in_net_from_addr[2].first, cur_dff.in_net_from_addr[3].first, cur_dff.in_net_from_addr[1].first, cur_dff.in_net_from_addr[0].first};
                            new_instr.Operand_Addr = {cur_dff.in_net_from_addr[2].second, cur_dff.in_net_from_addr[3].second, cur_dff.in_net_from_addr[1].second, cur_dff.in_net_from_addr[0].second};
                        }
                    }
                }
                for (auto i = cur_dff.in_net_from_addr.begin(); i != cur_dff.in_net_from_addr.end(); i++)
                {
                    if (i->second != MEM_DEPTH - 1 && i->second >= cur_instr_mem_cnt)
                        pushaddr = ((i->second + 1) > pushaddr) ? i->second + 1 : pushaddr;
                }
                tt_instr_mem[tt_instr_mem_index][pushaddr] = new_instr;
                tt_instr_mem_cnt[tt_instr_mem_index] = pushaddr + 1;
           }
        }
    }

    // Instruction for Jumping
    Instr new_instr;
    new_instr.Opcode = STATIC_CONFIG;
    new_instr.Jump   = 10;
    new_instr.Value_Data.push_back(1);
    new_instr.Datamem_Sel  = {0, 0, 0, 0};
    new_instr.Operand_Addr = {0, 0, 0, 0};
    auto max = *max_element(tt_instr_mem_cnt.begin(), tt_instr_mem_cnt.end());
    for (auto n = tt_instr_mem.begin(); n != tt_instr_mem.end(); n++)
    {
        (*n)[max] = new_instr;
    }


    return tt_instr_mem;
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