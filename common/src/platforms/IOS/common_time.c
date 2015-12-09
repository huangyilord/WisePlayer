//
//  common_time.c
//  common
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "common_time.h"
#include <sys/time.h>

uint32 time_get_tick_count()
{
    struct timeval tv = {0};
    gettimeofday( &tv, NULL );
    return (uint32)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
