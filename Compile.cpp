#include <iostream>
#include <algorithm>
#include <chrono>

#include "compile.h"
#include "ArchDefine.h"
#include "Parser.h"
#include "VCDParser.h"
#include "VCDTypes.h"
#include "PartGraph.h"
#include "TopoSort.h"
#include "PreProcess.h"
#include "ListSchedule.h"
#include "InstrGen.h"
#include "COEGen.h"

using namespace std;


int main(int argc, char const *argv[])
{
    std::string v_path, vcdFilePath, instr_out, out_dir; 
    if (argc == 5)
    {
        v_path = argv[1];
        vcdFilePath = argv[2]; 
        instr_out = argv[3];
        out_dir = argv[4];
    }
    else
    {
        std::cout << "[USAGE] ./compile v_path vcd_path [instr_path] [outdir_path]" << endl;
        exit(-1);
    }

    auto start_total = std::chrono::steady_clock::now();

    /****************************************************************************************************/
    /*                                          NETLIST PARSING                                         */
    /****************************************************************************************************/
    std::cout << "1. Executing netlist file parsing." << endl;
    auto start_v = std::chrono::steady_clock::now();

    Parser _parser;
    _parser.parse_v(v_path);

    auto end_v = std::chrono::steady_clock::now();
    long duration_v = std::chrono::duration_cast<std::chrono::milliseconds>(end_v - start_v).count();
    std::cout << "Successfully finished netlist file parsing. (" << duration_v << "ms) " << endl;
    std::cout << endl;

    map<int, LutType> luts = _parser.get_luts();
    map<int, DffType> dffs = _parser.get_dffs();
    int nNode = luts.size() + dffs.size();
    map<string, string> pin_bits = _parser.get_pin_bits();
    map<string, TimedValues *> pinbitValues = _parser.get_pinbitValues();
    map<string, string> assign_pairs = _parser.get_assign_pairs();
    map<string, vector<int>> net_for_id = _parser.net_for_id;
    map<string, int> net_from_id = _parser.net_from_id;
    map<string, int> net_from_id_dff = _parser.net_from_id_dff;

    /****************************************************************************************************/
    /*                                          VCD FILE PARSING                                        */
    /****************************************************************************************************/
    std::cout << "2. Executing vcd file parsing." << endl;
    auto start_vcd = std::chrono::steady_clock::now();

    VCDParser _vcdparser;
    _vcdparser.parse(vcdFilePath);

    auto end_vcd = std::chrono::steady_clock::now();
    long duration_vcd = std::chrono::duration_cast<std::chrono::milliseconds>(end_vcd - start_vcd).count();
    std::cout << "Successfully finished vcd file parsing. (" << duration_vcd << "ms)" << endl;
    std::cout << endl;

    std::unordered_map<std::string, std::vector<unsigned int> *> vcd_times = _vcdparser.times;
    std::unordered_map<std::string, std::vector<short> *> vcd_values = _vcdparser.values;

    /****************************************************************************************************/
    /*                                             PARTITION                                            */
    /****************************************************************************************************/
    std::cout << "3. Executing graph partition." << endl;
    auto start_par = std::chrono::steady_clock::now();

    vector<idx_t> part;
    vector<vector<int>> edges;
    int nParts = (luts.size() + dffs.size() + N_PROCESSORS_PER_CLUSTER * N_LUTS_PER_PROCESSOR - 1) / (N_PROCESSORS_PER_CLUSTER * N_LUTS_PER_PROCESSOR);
    Part _part;
    _part.Partition(luts, dffs, net_from_id, net_from_id_dff, assign_pairs, part, edges);

    auto end_par = std::chrono::steady_clock::now();
    long duration_par = std::chrono::duration_cast<std::chrono::milliseconds>(end_par - start_par).count();
    std::cout << "Successfully finished graph partition. (" << duration_par << "ms)" << endl;
    std::cout << endl;

    /****************************************************************************************************/
    /*                                            LEVELIZATION                                          */
    /****************************************************************************************************/
    cout << "4. Executing levelization." << endl;
    auto start_level = std::chrono::steady_clock::now();

    vector<vector<int>> levels_ASAP = topoSortASAP(nNode, edges);
    vector<vector<int>> levels_ALAP = topoSortALAP(nNode, edges);

    auto end_level = std::chrono::steady_clock::now();
    long duration_level = std::chrono::duration_cast<std::chrono::milliseconds>(end_level - start_level).count();
    cout << "Successfully finished levelization. (" << duration_level << "ms)" << endl;
    cout << endl;

    /****************************************************************************************************/
    /*                                           PREPROCESSING                                          */
    /****************************************************************************************************/
    std::cout << "5. Executing pre-processing." << endl;
    auto start_pre = std::chrono::steady_clock::now();

    PreProcess(part, luts, dffs, nNode, net_from_id, net_from_id_dff, pinbitValues, assign_pairs, vcd_values);

    auto end_pre = std::chrono::steady_clock::now();
    long duration_pre = std::chrono::duration_cast<std::chrono::milliseconds>(end_pre - start_pre).count();
    std::cout << "Successfully finished pre-processing. (" << duration_pre << "ms)" << endl;
    std::cout << endl;

    /****************************************************************************************************/
    /*                                            SCHEDULING                                            */
    /****************************************************************************************************/
    cout << "6. Executing scheduling." << endl;
    auto start_sch = std::chrono::steady_clock::now();

    ListSch _listsch;
    vector<vector<int>> SchList = _listsch.MLS(levels_ASAP, levels_ALAP, edges, luts, dffs, nNode, nParts, part);

    auto end_sch = std::chrono::steady_clock::now();
    long duration_sch = std::chrono::duration_cast<std::chrono::milliseconds>(end_sch - start_sch).count();
    cout << "Successfully finished scheduling. (" << duration_sch << "ms)" << endl;
    cout << endl;

    /****************************************************************************************************/
    /*                                      INSTRUCTION GENERATION                                      */
    /****************************************************************************************************/
    cout << "7. Executing instruction generation." << endl;
    auto start_ins = std::chrono::steady_clock::now();

    vector<vector<Instr>> tt_instr_mem = InstrGen(SchList, luts, dffs, net_for_id);

    auto end_ins = std::chrono::steady_clock::now();
    long duration_ins = std::chrono::duration_cast<std::chrono::milliseconds>(end_ins - start_ins).count();
    cout << "Successfully finished instruction generation. (" << duration_ins << "ms)" << endl;
    cout << endl;

    /****************************************************************************************************/
    /*                                          COE GENERATION                                          */
    /****************************************************************************************************/
    std::cout << "8. Executing output files generation." << endl;
    auto start_out = std::chrono::steady_clock::now();

    COEGen(tt_instr_mem, instr_out, out_dir, luts, dffs, pin_bits, assign_pairs);

    auto end_out = std::chrono::steady_clock::now();
    long duration_out = std::chrono::duration_cast<std::chrono::milliseconds>(end_out - start_out).count();
    std::cout << "Successfully finished output files generation. (" << duration_out << "ms)" << endl;
    std::cout << endl;



    auto end_total = std::chrono::steady_clock::now();
    long duration_total = std::chrono::duration_cast<std::chrono::milliseconds>(end_total - start_total).count();
    std::cout << "COMPILING DONE!    TIME:" << duration_total << "ms" << endl;
    std::cout << endl;
}

