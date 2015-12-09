//
//  memory_pool.c
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "memory_pool.h"
#include "common_debug.h"

void memory_pool_initialize( memory_pool* pool, size_t block_size, uint32 block_num )
{
    pool->max_block_num = block_num;
    pool->current_block_num = 0;
    if ( block_size > sizeof(void*) )
    {
        pool->block_size = block_size;
    }
    else
    {
        pool->block_size = sizeof(void*);
    }
    pool->first_block = NULL;
}

void memory_pool_destroy( memory_pool* pool )
{
    void* block = pool->first_block;
    while ( NULL != block )
    {
        pool->first_block = *(void**)block;
        free( block );
        block = pool->first_block;
    }
    pool->current_block_num = 0;
}

void* memory_pool_alloc( memory_pool* pool )
{
    void* block = pool->first_block;
    if ( NULL != block )
    {
        pool->first_block = *(void**)block;
        --pool->current_block_num;
    }
    else
    {
        ASSERT( pool->current_block_num == 0 );
        block = malloc( pool->block_size );
    }
    return block;
}

void memory_pool_free( memory_pool* pool, void* block )
{
    if ( pool->current_block_num == pool->max_block_num )
    {
        free( block );
    }
    else
    {
        *(void**)block = pool->first_block;
        pool->first_block = block;
        ++pool->current_block_num;
    }
}