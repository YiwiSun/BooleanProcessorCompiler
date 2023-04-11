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

//! Multi-cluster
#define MAX_CLUSTERS                4
#define N_PROCESSORS_PER_CLUSTER    64
#define INTRA_CLUSTER_CLK           1
#define INTER_CLUSTER_CLK           2
//! Full-connection
#define N_PROCESSORS                256
#define DATA_TRANS_CLK              1

#define INS_DEPTH                   512
#define MEM_DEPTH                   512
#define N_LUTS_PER_PROCESSOR        50

#define MEMORY_INITIALIZATION_RADIX 2


#endif