//
//  atomic.h
//  common
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _ATOMIC_H_ )
#define _ATOMIC_H_

#include "common_typedefs.h"

/** Set the target with the src atomically.
 *
 *  @param target:      The target var.
 *  @param src:         The value to set.
 *  @return:            Returns the original value of the target.
 */
void* atomic_exchange_ptr( void* volatile* target, void* src );

/** Set the target with the src atomically.
 *
 *  @param target:      The target var.
 *  @param src:         The value to set.
 *  @return:            Returns the original value of the target.
 */
uint32 atomic_exchange_u32( uint32 volatile* target, uint32 src );

/** Set the target with the src atomically.
 *
 *  @param target:      The target var.
 *  @param src:         The value to set.
 *  @return:            Returns the original value of the target.
 */
uint64 atomic_exchange_u64( uint64 volatile* target, uint64 src );

#endif
