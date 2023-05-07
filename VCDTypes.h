#include <map>
#include <unordered_map>
#include <utility>
#include <string>
#include <cstring>
#include <vector>
#include <deque>
#include <assert.h>
#include <cstdlib>
#include <stdlib.h>
#include <cstddef>

/**
 * @brief A file for common types and data structures used by compiler.
 **/

#ifndef VCDTypes_H
#define VCDTypes_H

using namespace std;

//! Friendly name for a signal
typedef std::string VCDSignalReference;

//! Friendly name for a scope
typedef std::string VCDScopeName;

//! Compressed hash representation of a signal.
typedef std::string VCDSignalHash;

//! Represents a single instant in time in a trace
typedef unsigned long long VCDTime;

//! Specifies the timing resoloution along with VCDTimeUnit
typedef unsigned VCDTimeRes;

//! Width in bits of a signal.
typedef unsigned VCDSignalSize;

//! Represents the four-state signal values of a VCD file.
typedef enum {
    VCD_0 = 0,  //!< Logic zero
    VCD_X = 1,  //!< Unknown / Undefined
    VCD_Z = 2,  //!< High Impedence
    VCD_1 = 3   //!< Logic one.
} VCDBit;


//! A vector of VCDBit values.
typedef std::vector<VCDBit> VCDBitVector;

//! Typedef to identify a real number as stored in a VCD.
typedef double VCDReal;


//! Describes how a signal value is represented in the VCD trace.
typedef enum {
    VCD_SCALAR, //!< Single VCDBit
    VCD_VECTOR, //!< Vector of VCDBit
    VCD_REAL    //!< IEEE Floating point (64bit).
} VCDValueType;


// Forward declaration of class.
class VCDValue;


//! A signal value tagged with times.
typedef struct {
    VCDTime     time;
    VCDValue  * value;
} VCDTimedValue;


//! A vector of tagged time/value pairs, sorted by time values.
typedef std::deque<VCDTimedValue*> VCDSignalValues;


//! Variable types of a signal in a VCD file.
typedef std::string VCDVarType;


//! Represents the possible time units a VCD file is specified in.
typedef enum {
    TIME_S,     //!< Seconds
    TIME_MS,    //!< Milliseconds
    TIME_US,    //!< Microseconds
    TIME_NS,    //!< Nanoseconds
    TIME_PS,    //!< Picoseconds
} VCDTimeUnit;


//! Represents the type of SV construct who's scope we are in.
typedef std::string VCDScopeType;


// Typedef over vcdscope to make it available to VCDSignal struct.
typedef struct vcdscope VCDScope;

//! Represents a single signal reference within a VCD file
typedef struct {
    VCDSignalHash       hash;
    VCDSignalReference  reference;   //port name
    VCDScope          * scope;
    VCDSignalSize       size;
    VCDVarType          type;
    int                 lindex; // -1 if no brackets, otherwise [lindex] or [lindex:rindex]
    int                 rindex; // -1 if not [lindex:rindex]
} VCDSignal;


// Pin : ModuleType's port/wire OR instance's port/wire declare
typedef struct {
    std::string         name;
    std::string         type;   //input/output/wire/reg/inout
    unsigned            size;
    int                 lindex; // -1 if no brackets, otherwise [lindex] or [lindex:rindex]
    int                 rindex; // -1 if not [lindex:rindex]
} Pin;

// TimedValue: value at each time
typedef struct 
{
    VCDTime t;
    VCDBit  value;
} TimedValue;

typedef std::deque<TimedValue> TimedValues;

// For Simulate
typedef struct 
{
    VCDTime time;
    std::map<std::string, VCDBit> values;
} TimedValueSim;

//! Represents a scope type, scope name pair and all of it's child signals.
struct vcdscope
{
    VCDScopeName name;                //!< The short name of the scope
    VCDScopeType type;                //!< Construct type
    VCDScope *parent;                 //!< Parent scope object
    std::vector<VCDScope *> children; //!< Child scope objects.
    std::vector<VCDSignal *> signals; //!< Signals in this scope.
};

// LutType: structure of Look-up table
typedef struct
{
    unsigned long long      num;
    vector<string>          in_ports;
    string                  out_ports;
    string                  out_ports_type;
    string                  lut_res;

    pair<int, int>          node_addr; 
    int                     res_pos_at_mem;

    vector<int>             in_net_from_type;       // 0: input from LUTs; 1: input from DFFs; 2: input from PinbitValues; 3: input from Initials; 4: input from Interface
    vector<int>             in_net_from_id;         // ID of LUTs/DFFs; Value of PinbitValues/Initials (-1: Value_1; -2: Value_0); -3 if Interface
    vector<string>          in_net_from_info;       // blank of LUTs/DFFs; INFO of PinbitValues/Initials/Interface
    vector<int>             in_net_from_part;       // Part of LUTs/DFFs (-3 if the same part); -1 if PinbitValues/Initials/Interface
    vector<pair<int, int>>  in_net_from_addr;       // pair<int, int> (type of input from, address of MEM)
    vector<int>             in_net_from_ready;      // 1: ready; 0: not ready 

} LutType;

// DffType: structure of Flip-flops
typedef struct
{
    unsigned long long      num;
    int                     type;                                           // 0: unconditional judgment; 1: if; 2: if...else...; 3: if...else if...; 4: if...if...else...
    vector<pair<string, string>> sens_edge;                                 // <sensitive signals, edge>
    vector<pair<string, vector<pair<string, int>>>> assignsig_condsig;      // <assignment signal, <condition signal, condition>>
    vector<string>          dff_in_ports;
    string                  dff_out;
    string                  dff_out_ports_type;

    pair<int, int>          node_addr; 
    int                     FF_Datamem_Addr;
    int                     res_pos_at_mem;

    vector<int>             in_net_from_type;       // 0: input from LUTs; 1: input from DFFs; 2: input from PinbitValues; 3: input from Initials; 4: input from interface
    vector<int>             in_net_from_id;         // ID of LUTs/DFFs; Value of PinbitValues/Initials (-1: Value_1; -2: Value_0); -3 if Interface
    vector<string>          in_net_from_info;       // blank of LUTs/DFFs; INFO of PinbitValues/Initials/Interface
    vector<int>             in_net_from_part;       // Part of LUTs/DFFs (-3 if the same part); -1 if PinbitValues/Initials/Interface
    vector<pair<int, int>>  in_net_from_addr;       // pair<int, int> (type of input from, address of MEM)
    vector<int>             in_net_from_ready;      // 1: ready; 0: not ready

} DffType;

#endif
