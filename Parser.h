#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "VCDTypes.h"

#ifndef Parser_H
#define Parser_H

using namespace std;

/**
 * @brief Parse netlist files (.v) generated by yosys
 **/
class Parser {
    public:
        friend class inter;

        //! Instance a new Parser Class
        Parser();

        //! Destructor
        ~Parser();

        vector<std::string> Split(const std::string &str, const std::string &delim);
        string HextoBinary_x(string HexDigit);

        void parse_v(std::string& v_path);

        // func    
        map<std::string, Pin> get_pins() {return pins;}
        map<string, string> get_pin_bits() {return pin_bits;}       // pin_bit_name : pin_bit_type
        map<int, LutType> get_luts() {return luts;}
        map<int, DffType> get_dffs() {return dffs;}
        map<std::string, TimedValues*> get_pinbitValues() {return pinbitValues;}
        map<std::string, std::string> get_assign_pairs() {return assign_pairs;}

        // param
        std::string top_module_name;
        map<std::string, Pin> pins;                   // name : Pin
        // vector<std::string> pin_bits;
        map<string, string> pin_bits;                 // name : PinType
        map<int, LutType> luts;                       // num : LutType
        map<int, DffType> dffs;                       // num : DffType
        vector<std::string> dff_outs;                 // outputs from dffs
        map<string, vector<int>> net_for_id;
        map<string, int> net_from_id;
        map<string, vector<int>> net_for_id_dff;
        map<string, int> net_from_id_dff;
        map<std::string, TimedValues*> pinbitValues;  // name : TimedValues(PinBitValues), mainly for simulator, and here is for process 'assign'
        map<std::string, std::string> assign_pairs;   // assign_left : assign_right
};

#endif