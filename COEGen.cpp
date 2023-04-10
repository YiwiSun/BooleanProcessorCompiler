#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <bitset>
#include <bits/stdc++.h>

#include "COEGen.h"
#include "Compile.h"

// void COEGen(string &instr_out, string &out_dir, map<int, Processor> &processors,
//             map<int, LutType> &luts, map<int, DffType> &dffs, map<string, string> &pin_bits, map<string, string> &assign_pairs)
// {
//     for (auto it = processors.begin(); it != processors.end(); it++)
//     {
//         auto i = it->first;
//         int cluster_num = (i / N_PROCESSORS_PER_CLUSTER) + 1;
//         int bp_num = (i % N_PROCESSORS_PER_CLUSTER) + 1;
//         string cur_instr_out = instr_out + "instrmem_" + to_string(cluster_num) + "_" + to_string(bp_num) + ".coe";
//         ofstream outinstr(cur_instr_out);
//         outinstr << "MEMORY_INITIALIZATION_RADIX = " << MEMORY_INITIALIZATION_RADIX << ";" << endl;
//         outinstr << "MEMORY_INITIALIZATION_VECTOR =" << endl;
//         vector<string> instrs = it->second.instr_mem;
//         assert(instrs.size() < N_INS_PER_PROCESSOR);
//         for (vector<string>::iterator iter = instrs.begin(); iter != instrs.end(); iter++)
//         {
//             if (iter == instrs.end() - 1)
//                 outinstr << *iter << ";" << endl;
//             else
//                 outinstr << *iter << "," << endl;
//         }
//         outinstr.close();
//     }

//     ofstream outdir(out_dir);
//     for (map<string, string>::iterator i = pin_bits.begin(); i != pin_bits.end(); i++)
//     {
//         if (i->second == "output")
//         {
//             for (map<int, LutType>::iterator it = luts.begin(); it != luts.end(); it++)
//             {
//                 if (it->second.out_ports == i->first)
//                 {
//                     outdir << "Pin: ";
//                     outdir.setf(ios::left);
//                     outdir << setw(30) << i->first;
//                     outdir << "LUT: ";
//                     outdir << setw(10) << it->first;
//                     outdir << "Clutser: ";
//                     outdir << setw(10) << it->second.node_addr[0];
//                     outdir << "Processor: ";
//                     outdir << setw(10) << it->second.node_addr[1];
//                     outdir << "Addr: ";
//                     outdir << setw(10) << it->second.res_pos_at_mem << endl;
//                 }
//             }
//             for (auto it = dffs.begin(); it != dffs.end(); it++)
//             {
//                 if (it->second.dff_out == i->first)
//                 {
//                     outdir << "Pin: ";
//                     outdir.setf(ios::left);
//                     outdir << setw(30) << i->first;
//                     outdir << "DFF: ";
//                     outdir << setw(10) << it->first;
//                     outdir << "Clutser: ";
//                     outdir << setw(10) << it->second.node_addr[0];
//                     outdir << "Processor: ";
//                     outdir << setw(10) << it->second.node_addr[1];
//                     outdir << "Addr: ";
//                     outdir << setw(10) << it->second.res_pos_at_mem << endl;
//                 }
//             }
//             if (assign_pairs.find(i->first) != assign_pairs.end())
//             {
//                 string tmp = assign_pairs[i->first];
//                 for (map<int, LutType>::iterator it = luts.begin(); it != luts.end(); it++)
//                 {
//                     if (it->second.out_ports == tmp)
//                     {
//                         outdir << "Pin: ";
//                         outdir.setf(ios::left);
//                         outdir << setw(30) << i->first;
//                         outdir << "LUT: ";
//                         outdir << setw(10) << it->first;
//                         outdir << "Clutser: ";
//                         outdir << setw(10) << it->second.node_addr[0];
//                         outdir << "Processor: ";
//                         outdir << setw(10) << it->second.node_addr[1];
//                         outdir << "Addr: ";
//                         outdir << setw(10) << it->second.res_pos_at_mem << endl;
//                     }
//                 }
//                 for (auto it = dffs.begin(); it != dffs.end(); it++)
//                 {
//                     if (it->second.dff_out == tmp)
//                     {
//                         outdir << "Pin: ";
//                         outdir.setf(ios::left);
//                         outdir << setw(30) << i->first;
//                         outdir << "DFF: ";
//                         outdir << setw(10) << it->first;
//                         outdir << "Clutser: ";
//                         outdir << setw(10) << it->second.node_addr[0];
//                         outdir << "Processor: ";
//                         outdir << setw(10) << it->second.node_addr[1];
//                         outdir << "Addr: ";
//                         outdir << setw(10) << it->second.res_pos_at_mem << endl;
//                     }
//                 }
//             }
//         }
//     }
//     outdir.close();
// }
