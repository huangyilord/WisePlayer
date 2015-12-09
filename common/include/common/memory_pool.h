//
//  memory_pool.h
//  common
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _MEMORY_POOL_H_ )
#define _MEMORY_POOL_H_

#include "common_typedefs.h"

typedef struct memory_pool
{
    size_t      max_block_num;
    size_t      current_block_num;
    size_t      block_size;
    void*       first_block;
} memory_pool;

/** Initialize a pool instance.
 *
 *  @param pool:            The pool instance.
 *  @param block_size:      Size of each block in bytes.
 *  @param block_num:       Max block num save in this pool.
 *  @return:                None.
 */
void memory_pool_initialize( memory_pool* pool, size_t block_size, uint32 block_num );

/** Destroy a memory pool instance.
 *
 *  @param pool:            The pool instance.
 *  @return:                None.
 */
void memory_pool_destroy( memory_pool* pool );

/** Get a memory block from the pool.
 *
 *  @param pool:            The pool instance.
 *  @return:                Returns the pointer of the memory block.
 *                          Returns NULL if failed.
 */
void* memory_pool_alloc( memory_pool* pool );

/** Put a memory block into the pool.
 *
 *  @param pool:            The pool instance.
 *  @param block:           The memory block.
 *  @return:                None.
 */
void memory_pool_free( memory_pool* pool, void* block );

#endif
