#include <iostream>
#include <algorithm>

#include "InstrGen.h"

#ifndef COEGEN_H
#define COEGEN_H

using namespace std;

void    COEGen(string &instr_out, string &out_dir, map<int, Processor> &processors,
            map<int, LutType> &luts, map<int, DffType> &dffs, map<string, string> &pin_bits, map<string, string> &assign_pairs);
string  InstrCat(Instr &instr);
string  toBinary(int n);

#endif