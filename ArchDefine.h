/**
 * @file ArchDefine.h
 * @author YiweiSun 
 * @brief Architecture Parameter Define
 * @version 0.1
 * @date 2023-03-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef ARCHDEFINE_H
#define ARCHDEFINE_H

//! Full-connection
#define N_PROCESSORS                256
#define DATA_TRANS_CLK              1
//! Multi-cluster
#define MAX_CLUSTERS                4
#define N_PROCESSORS_PER_CLUSTER    64
#define INTRA_CLUSTER_CLK           1
#define INTER_CLUSTER_CLK           2

#define INS_DEPTH                   512
#define MEM_DEPTH                   512

#define LUT_SIZE                    4
#define FETCH_DATA_SIZE             4
#define N_LUTS_PER_PROCESSOR        70

#define PRE_DELAY                   5
#define POST_DELAY                  5
#define INTERFACE_PER_PROCESSOR     4

#define MEMORY_INITIALIZATION_RADIX 2


#endif