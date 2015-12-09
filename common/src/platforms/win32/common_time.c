//
//  common_time.c
//  common
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "common_time.h"
#include <windows.h>

uint32 time_get_tick_count()
{
    return GetTickCount();
}
