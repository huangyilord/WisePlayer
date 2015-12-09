//
//  atomic.c
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "atomic.h"
#include <windows.h>

void* atomic_exchange_ptr( void* volatile* target, void* src )
{
	return InterlockedExchangePointer( target, src );
}

uint32 atomic_exchange_32( uint32 volatile* target, uint32 src )
{
    return InterlockedExchange( target, src );
}

uint64 atomic_exchange_u64( uint64 volatile* target, uint64 src )
{
    return InterlockedExchange64( (LONGLONG volatile*)target, src );
}
