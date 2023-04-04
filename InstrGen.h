#include <iostream>
#include <algorithm>

#include "VCDTypes.h"
#include "ArchDefine.h"

#ifndef INSTRGEN_H
#define INSTRGEN_H

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////
// 102       99 98    97 96        65 64         48 47            36 35                  0
// ---------------------------------------------------------------------------------------
// |           |        |            |             |                |                    |
// |  Opcode   |  Jump  |  NodeAddr  |  ValueData  |   DatamemSel   |     OperandAddr    |
// |           |        |            |             |                |                    |
// ---------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////

#define LUT_CONFIG      0000
#define MEM_ACCESS      0001
#define STATIC_CONFIG   0010
#define IDLE_MODE       0011
#define FF_CONFIG       0100

#define Exter_Datamem_0 000
#define Exter_Datamem_1 001
#define Exter_Datamem_2 010
#define Exter_Datamem_3 011
#define Inter_Datamem   100
#define FF_Datamem      101

//! Struction of Instruction
typedef struct
{
    int Opcode;
    int Jump;        
    vector<int> Node_Addr;
    vector<int> Value_Data;
    vector<int> Datamem_Sel;
    vector<int> Operand_Addr;
} Instr;

vector<Instr> instr_mem(INS_DEPTH);
vector<vector<Instr>> tt_instr_mem(N_PROCESSORS, vector<Instr>(INS_DEPTH));

vector<vector<Instr>> InstrGen(vector<vector<int>> &SchList, map<int, LutType> &luts, map<int, DffType> &dffs);
string InstrCat(Instr &instr);
string toBinary(int n);
string HextoBinary(string HexDigit);


#endif