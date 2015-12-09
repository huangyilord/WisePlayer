//
//  common_typedefs.h
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _COMMON_TYPEDEFS_H_ )
#define _COMMON_TYPEDEFS_H_

#include <windows.h>

typedef UINT64          uint64;
typedef INT64           int64;
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

typedef HANDLE          thread_type;
typedef HANDLE          event_type;
typedef void*           event_group_type;
typedef void*           event_group_element_type;
typedef HANDLE          mutex_type;

#endif
