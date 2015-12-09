//
//  common_typedefs.h
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _COMMON_TYPEDEFS_H_ )
#define _COMMON_TYPEDEFS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

typedef uint64_t        uint64;
typedef int64_t         int64;
typedef unsigned        uint32;
typedef int             int32;
typedef unsigned short  uint16;
typedef short           int16;
typedef unsigned char   uint8;
typedef char            int8;
typedef float           float32;
typedef double          float64;

typedef uint8           byte;
typedef uint16          word;
typedef uint32          dword;

typedef uint32          boolean;

#define TRUE    1
#define FALSE   0

typedef pthread_t       thread_type;
typedef void*           event_type;
typedef void*           event_group_type;
typedef void*           event_group_element_type;
typedef void*           mutex_type;

#endif
