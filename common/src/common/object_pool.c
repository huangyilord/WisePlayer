//
//  object_pool.c
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "object_pool.h"
#include "common_debug.h"

boolean object_pool_initialize( object_pool* pool, memory_pool* mem_pool, size_t element_size )
{
    size_t header_size = (uint32)(sizeof(object_pool_block_info) / sizeof(void*)) * sizeof(void*);
    header_size += header_size < sizeof(object_pool_block_info) ? sizeof(void*) : 0;
    pool->block_element_offset = header_size;
    pool->element_size = element_size;
    element_size = (uint32)(pool->element_size / sizeof(void*)) * sizeof(void*);
    element_size += element_size < pool->element_size ? sizeof(void*) : 0;
    pool->element_size = element_size + sizeof(void*);
    pool->block_pool = mem_pool;
    pool->first_full_block = pool->last_full_block = NULL;
    pool->first_free_block = NULL;
    if ( mem_pool->block_size < (pool->element_size + header_size) )
    {
        return FALSE;
    }
    pool->elment_count_each_block = (uint32)((mem_pool->block_size - header_size) / pool->element_size);
    return TRUE;
}

void object_pool_destroy( object_pool* pool )
{
    object_pool_clear( pool );
    memset( pool, 0, sizeof(object_pool) );
}

void* object_pool_alloc( object_pool* pool )
{
    object_pool_block_info* block = pool->first_free_block;
    void** pelem = NULL;
    size_t i = 0;
    if ( NULL == block )
    {
        // create a new block
        block = memory_pool_alloc( pool->block_pool );
        if ( NULL == block )
        {
            return NULL;
        }
        memset( block, 0, sizeof(object_pool_block_info));
        // format the new block
        block->first_free = (byte*)block + pool->block_element_offset;
        pelem = (void**)block->first_free;
        for ( i = 1; i < pool->elment_count_each_block; ++i )
        {
            pelem = *pelem = (byte*)pelem + pool->element_size;
        }
        *pelem = NULL;
        pool->first_free_block = block;
        block->free_count = pool->elment_count_each_block;
    }
    // get an obj
    pelem = (void**)block->first_free;
    block->first_free = *pelem;
    --block->free_count;
    if ( 0 == block->free_count )
    {
        ASSERT( NULL == block->first_free );
        // move to the full list
        if ( NULL == block->prev )
        {
            ASSERT( pool->first_free_block == block );
            pool->first_free_block = block->next;
        }
        else
        {
            block->prev->next = block->next;
        }
        if ( NULL != block->next )
        {
            block->next->prev = block->prev;
        }
        block->prev = pool->last_full_block;
        block->next = NULL;
        if ( NULL == pool->last_full_block )
        {
            pool->first_full_block = block;
        }
        else
        {
            pool->last_full_block->next = block;
        }
        pool->last_full_block = block;
    }
    *pelem = block;
    return (byte*)pelem + sizeof(void*);
}

void object_pool_free( object_pool* pool, void* obj )
{
    void** pelem = (void**)((byte*)obj - sizeof(void*));
    object_pool_block_info* block = (object_pool_block_info*)(*pelem);
    if ( 0 == block->free_count )
    {
        // move to the free list
        if ( NULL == block->prev )
        {
            ASSERT( pool->first_full_block == block );
            pool->first_full_block = block->next;
        }
        else
        {
            block->prev->next = block->next;
        }
        if ( NULL == block->next )
        {
            ASSERT( pool->last_full_block == block );
            pool->last_full_block = block->prev;
        }
        else
        {
            block->next->prev = block->prev;
        }
        block->next = pool->first_free_block;
        block->prev = NULL;
        if ( NULL != pool->first_free_block )
        {
            pool->first_free_block->prev = block;
        }
        pool->first_free_block = block;
    }
    ++ block->free_count;
    *pelem = block->first_free;
    block->first_free = pelem;
    if ( pool->elment_count_each_block == block->free_count )
    {
        // free the block
        if ( NULL == block->prev )
        {
            ASSERT( pool->first_free_block == block );
            pool->first_free_block = block->next;
        }
        else
        {
            block->prev->next = block->next;
        }
        if ( NULL != block->next )
        {
            block->next->prev = block->prev;
        }
        memory_pool_free( pool->block_pool, block );
    }
}

void object_pool_clear( object_pool* pool )
{
    object_pool_block_info* current = pool->first_free_block;
    object_pool_block_info* next = NULL;
    // clear free list
    while ( NULL != current )
    {
        next = current->next;
        memory_pool_free( pool->block_pool, current );
        current = next;
    }
    // clear full list
    current = pool->first_full_block;
    while ( NULL != current )
    {
        next = current->next;
        memory_pool_free( pool->block_pool, current );
        current = next;
    }
    pool->first_full_block = pool->last_full_block = NULL;
    pool->first_free_block = NULL;
}
