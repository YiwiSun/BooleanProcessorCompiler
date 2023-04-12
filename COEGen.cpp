#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <bitset>
#include <bits/stdc++.h>

#include "COEGen.h"
#include "Compile.h"

void COEGen(vector<vector<Instr>> &tt_instr_mem, string &instr_out, string &out_dir,
            map<int, LutType> &luts, map<int, DffType> &dffs, map<string, string> &pin_bits, map<string, string> &assign_pairs)
{
    for (auto it = tt_instr_mem.begin(); it != tt_instr_mem.end(); it++)
    {
        auto i = distance(tt_instr_mem.begin(), it);
        int cluster_num = (i / N_PROCESSORS_PER_CLUSTER) + 1;
        int bp_num      = (i % N_PROCESSORS_PER_CLUSTER) + 1;
        string cur_instr_out = instr_out + "instrmem_" + to_string(cluster_num) + "_" + to_string(bp_num) + ".coe";
        ofstream outinstr(cur_instr_out);
        outinstr << "MEMORY_INITIALIZATION_RADIX = " << MEMORY_INITIALIZATION_RADIX << ";" << endl;
        outinstr << "MEMORY_INITIALIZATION_VECTOR =" << endl;
        for (auto iter = it->begin(); iter != it->end(); iter++)
        {
            auto cat_instr = InstrCat(*iter);
            if (iter == it->end() - 1)
                outinstr << cat_instr << ";" << endl;
            else
                outinstr << cat_instr << "," << endl;
        }
        outinstr.close();
    }

    ofstream outdir(out_dir);
    for (auto i = pin_bits.begin(); i != pin_bits.end(); i++)
    {
        if (i->second == "output")
        {
            for (auto it = luts.begin(); it != luts.end(); it++)
            {
                if (it->second.out_ports == i->first)
                {
                    outdir << "Pin: ";
                    outdir.setf(ios::left);
                    outdir << setw(30) << i->first;
                    outdir << "LUT: ";
                    outdir << setw(10) << it->first;
                    outdir << "Clutser: ";
                    outdir << setw(10) << it->second.node_addr.first;
                    outdir << "Processor: ";
                    outdir << setw(10) << it->second.node_addr.second;
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
                    outdir << setw(10) << it->second.node_addr.first;
                    outdir << "Processor: ";
                    outdir << setw(10) << it->second.node_addr.second;
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
                        outdir << setw(10) << it->second.node_addr.first;
                        outdir << "Processor: ";
                        outdir << setw(10) << it->second.node_addr.second;
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
                        outdir << setw(10) << it->second.node_addr.first;
                        outdir << "Processor: ";
                        outdir << setw(10) << it->second.node_addr.second;
                        outdir << "Addr: ";
                        outdir << setw(10) << it->second.res_pos_at_mem << endl;
                    }
                }
            }
        }
    }
    outdir.close();
}

string InstrCat(Instr &instr)
{
    string value_data;
    for (auto i = instr.Value_Data.begin(); i != instr.Value_Data.end(); i++)
        {value_data.append(toBinary(*i));}
    string cat_instr;
    stringstream ss;
    ss << bitset<4>(toBinary(instr.Opcode)) << instr.Jump[0] << instr.Jump[1]
       << bitset<8>(toBinary(instr.Node_Addr[0])) << bitset<8>(toBinary(instr.Node_Addr[1])) << bitset<8>(toBinary(instr.Node_Addr[2])) << bitset<8>(toBinary(instr.Node_Addr[3]))
       << bitset<17>(value_data)
       << bitset<3>(toBinary(instr.Datamem_Sel[0])) << bitset<3>(toBinary(instr.Datamem_Sel[1])) << bitset<3>(toBinary(instr.Datamem_Sel[2])) << bitset<3>(toBinary(instr.Datamem_Sel[3]))
       << bitset<9>(toBinary(instr.Operand_Addr[0])) << bitset<9>(toBinary(instr.Operand_Addr[1])) << bitset<9>(toBinary(instr.Operand_Addr[2])) << bitset<9>(toBinary(instr.Operand_Addr[3]));
    cat_instr = ss.str();
    return cat_instr;
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