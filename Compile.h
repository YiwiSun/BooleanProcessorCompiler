#ifndef Compile_H
#define Compile_H

// For input initial signal changing
//! des64_src
// #define INITIAL_JUMP_SIG        "rst_n"
// #define INITIAL_JUMP_ADDR       33
// #define INITIAL_STATIC_VALUE    1
// Jump {1, 0}

//! AES-FPGA-master
// #define INITIAL_JUMP_SIG        "ld"
// #define INITIAL_JUMP_ADDR       53
// #define INITIAL_STATIC_VALUE    0
// Jump {1, 1}

//! SHACORE_UNROLLED_PIPELINED
#define INITIAL_JUMP_SIG        " "
#define INITIAL_JUMP_ADDR       MEM_DEPTH - 1
#define INITIAL_STATIC_VALUE    0
// Jump {1, 0}

#endif





















