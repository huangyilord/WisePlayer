//
//  atomic.c
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "atomic.h"
#include <libkern/OSAtomic.h>

void* atomic_exchange_ptr( void* volatile* target, void* src )
{
    void* oldValue = *target;
    while (!OSAtomicCompareAndSwapPtr( oldValue, src, target ))
    {
        oldValue = *target;
    }
    return oldValue;
}

uint32 atomic_exchange_32( uint32 volatile* target, uint32 src )
{
    uint32 oldValue = *target;
    while (!OSAtomicCompareAndSwap32( oldValue, src, (volatile int32_t*)target ))
    {
        oldValue = *target;
    }
    return oldValue;
}

uint64 atomic_exchange_u64( uint64 volatile* target, uint64 src )
{
    uint64 oldValue = *target;
    while (!OSAtomicCompareAndSwap64( oldValue, src, (volatile int64_t*)target ))
    {
        oldValue = *target;
    }
    return oldValue;
}
