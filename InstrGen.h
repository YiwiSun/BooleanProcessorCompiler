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

#define LUT_CONFIG              0b0000
#define MEM_ACCESS              0b0001
#define STATIC_CONFIG           0b0010
#define IDLE_MODE               0b0011
#define FF_CONFIG               0b0100


#define DFF_If_Else_0           0b0000
#define DFF_If_Else_1           0b0001
#define DFF_If_Else_2           0b0010
#define DFF_If_Else_3           0b0011
#define DFF_Direct_Assign_0     0b0100
#define DFF_Direct_Assign_1     0b0101
#define DFF_If_0                0b0110
#define DFF_If_1                0b0111
#define DFF_If_Elseif_0         0b1000
#define DFF_If_Elseif_1         0b1001
#define DFF_If_Elseif_2         0b1010
#define DFF_If_Elseif_3         0b1011
#define DFF_If_Ifelse_0         0b1100
#define DFF_If_Ifelse_1         0b1101
#define DFF_If_Ifelse_2         0b1110
#define DFF_If_Ifelse_3         0b1111


#define Exter_Datamem_0         0b000
#define Exter_Datamem_1         0b001
#define Exter_Datamem_2         0b010
#define Exter_Datamem_3         0b011
#define Inter_Datamem           0b100
#define FF_Datamem              0b101

#define LUT_Size                4

   
//! Struction of Instruction
typedef struct
{
    int Opcode;
    vector<int> Jump;        
    vector<int> Node_Addr;
    vector<int> Value_Data;
    vector<int> Datamem_Sel;
    vector<int> Operand_Addr;
} Instr;


vector<vector<Instr>>   InstrGen(vector<vector<int>> &SchList, map<int, LutType> &luts, map<int, DffType> &dffs,
                               map<string, vector<int>> &net_for_id);
int                     Sel_Exter_Datamem(int &n);

#endif