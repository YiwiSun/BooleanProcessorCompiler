#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <bitset>
#include <bits/stdc++.h>

#include "COEGen.h"
#include "Compile.h"

void COEGen(string &instr_out, string &out_dir, map<int, Processor> &processors,
            map<int, LutType> &luts, map<int, DffType> &dffs, map<string, string> &pin_bits, map<string, string> &assign_pairs)
{
    for (auto it = processors.begin(); it != processors.end(); it++)
    {
        auto i = it->first;
        int cluster_num = (i / N_PROCESSORS_PER_CLUSTER) + 1;
        int bp_num = (i % N_PROCESSORS_PER_CLUSTER) + 1;
        string cur_instr_out = instr_out + "instrmem_" + to_string(cluster_num) + "_" + to_string(bp_num) + ".coe";
        ofstream outinstr(cur_instr_out);
        outinstr << "MEMORY_INITIALIZATION_RADIX = " << MEMORY_INITIALIZATION_RADIX << ";" << endl;
        outinstr << "MEMORY_INITIALIZATION_VECTOR =" << endl;
        vector<string> instrs = it->second.instr_mem;
        assert(instrs.size() < N_INS_PER_PROCESSOR);
        for (vector<string>::iterator iter = instrs.begin(); iter != instrs.end(); iter++)
        {
            if (iter == instrs.end() - 1)
                outinstr << *iter << ";" << endl;
            else
                outinstr << *iter << "," << endl;
        }
        outinstr.close();
    }

    ofstream outdir(out_dir);
    for (map<string, string>::iterator i = pin_bits.begin(); i != pin_bits.end(); i++)
    {
        if (i->second == "output")
        {
            for (map<int, LutType>::iterator it = luts.begin(); it != luts.end(); it++)
            {
                if (it->second.out_ports == i->first)
                {
                    outdir << "Pin: ";
                    outdir.setf(ios::left);
                    outdir << setw(30) << i->first;
                    outdir << "LUT: ";
                    outdir << setw(10) << it->first;
                    outdir << "Clutser: ";
                    outdir << setw(10) << it->second.node_addr[0];
                    outdir << "Processor: ";
                    outdir << setw(10) << it->second.node_addr[1];
                    outdir << "Addr: ";
                    outdir << setw(10) << it->second.res_pos_at_mem << endl;
                }
            }
            for (auto it = dffs.begin(); it != dffs.end(); it++)
            {
                if (it->second.dff_out == i->first)
                {
                    outdir << "Pin: ";
                    outdir.setf(ios::left);
                    outdir << setw(30) << i->first;
                    outdir << "DFF: ";
                    outdir << setw(10) << it->first;
                    outdir << "Clutser: ";
                    outdir << setw(10) << it->second.node_addr[0];
                    outdir << "Processor: ";
                    outdir << setw(10) << it->second.node_addr[1];
                    outdir << "Addr: ";
                    outdir << setw(10) << it->second.res_pos_at_mem << endl;
                }
            }
            if (assign_pairs.find(i->first) != assign_pairs.end())
            {
                string tmp = assign_pairs[i->first];
                for (map<int, LutType>::iterator it = luts.begin(); it != luts.end(); it++)
                {
                    if (it->second.out_ports == tmp)
                    {
                        outdir << "Pin: ";
                        outdir.setf(ios::left);
                        outdir << setw(30) << i->first;
                        outdir << "LUT: ";
                        outdir << setw(10) << it->first;
                        outdir << "Clutser: ";
                        outdir << setw(10) << it->second.node_addr[0];
                        outdir << "Processor: ";
                        outdir << setw(10) << it->second.node_addr[1];
                        outdir << "Addr: ";
                        outdir << setw(10) << it->second.res_pos_at_mem << endl;
                    }
                }
                for (auto it = dffs.begin(); it != dffs.end(); it++)
                {
                    if (it->second.dff_out == tmp)
                    {
                        outdir << "Pin: ";
                        outdir.setf(ios::left);
                        outdir << setw(30) << i->first;
                        outdir << "DFF: ";
                        outdir << setw(10) << it->first;
                        outdir << "Clutser: ";
                        outdir << setw(10) << it->second.node_addr[0];
                        outdir << "Processor: ";
                        outdir << setw(10) << it->second.node_addr[1];
                        outdir << "Addr: ";
                        outdir << setw(10) << it->second.res_pos_at_mem << endl;
                    }
                }
            }
        }
    }
    outdir.close();
}

string InstrCat(Instr &instr);
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