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

#define LUT_CONFIG              0000
#define MEM_ACCESS              0001
#define STATIC_CONFIG           0010
#define IDLE_MODE               0011
#define FF_CONFIG               0100


#define DFF_If_Else_0           0000
#define DFF_If_Else_1           0001
#define DFF_If_Else_2           0010
#define DFF_If_Else_3           0011
#define DFF_Direct_Assign_0     0100
#define DFF_Direct_Assign_1     0101
#define DFF_If_0                0110
#define DFF_If_1                0111
#define DFF_If_Elseif_0         1000
#define DFF_If_Elseif_1         1001
#define DFF_If_Elseif_2         1010
#define DFF_If_Elseif_3         1011
#define DFF_If_Ifelse_0         1100
#define DFF_If_Ifelse_1         1101
#define DFF_If_Ifelse_2         1110
#define DFF_If_Ifelse_3         1111


#define Exter_Datamem_0         000
#define Exter_Datamem_1         001
#define Exter_Datamem_2         010
#define Exter_Datamem_3         011
#define Inter_Datamem           100
#define FF_Datamem              101

   
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


vector<vector<Instr>>   InstrGen(vector<vector<int>> &SchList, map<int, LutType> &luts, map<int, DffType> &dffs,
                               map<string, vector<int>> &net_for_id, map<string, vector<int>> &net_for_id_dff);
int                     Sel_Exter_Datamem(int &n);
string                  HextoBinary(string HexDigit);

#endif