//
//  object_pool.h
//  common
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _OBJECT_POOL_H_ )
#define _OBJECT_POOL_H_

#include "memory_pool.h"

typedef struct object_pool_block_info
{
    struct object_pool_block_info*      prev;
    struct object_pool_block_info*      next;
    void*                               first_free;
    size_t                              free_count;
} object_pool_block_info;

typedef struct object_pool
{
    memory_pool*                        block_pool;
    object_pool_block_info*             first_full_block;
    object_pool_block_info*             last_full_block;
    object_pool_block_info*             first_free_block;
    size_t                              element_size;
    size_t                              block_element_offset;
    size_t                              elment_count_each_block;
} object_pool;

/** Initialize an object_pool instance.
 *
 *  @param pool:            The pool instance.
 *  @param mem_pool:        An initialized memory_pool instance.
 *  @return:                Returns TRUE if success.
 */
boolean object_pool_initialize( object_pool* pool, memory_pool* mem_pool, size_t element_size );

/** Destroy an object_pool instance.
 *
 *  @param pool:            The pool instance.
 *  @return:                None.
 */
void object_pool_destroy( object_pool* pool );

/** Allocate an object from the pool.
 *
 *  @param pool:            The pool instance.
 *  @return:                Returns the pointer of the object.
 *                          Returns NULL if failed.
 */
void* object_pool_alloc( object_pool* pool );

/** Put an object into the pool.
 *
 *  @param pool:            The pool instance.
 *  @param obj:             The pointer to the object.
 *  @return:                None.
 */
void object_pool_free( object_pool* pool, void* obj );

/** Destroy all objects allocated by this pool.
 *
 *  @param pool:            The pool instance.
 *  @return:                None.
 */
void object_pool_clear( object_pool* pool );

#endif // !defined(_OBJECT_pool_H_)
