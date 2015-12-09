//
//  array_list.c
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "common_debug.h"
#include "array_list.h"

#define GET_BLOCK_INFO( list, block )       (array_list_block_info*)((byte*)(block) + (list)->element_size * ((list)->elment_count_each_block - 1))

boolean array_list_initialize( array_list* list, size_t element_size, memory_pool* block_pool )
{
    list->block_pool = block_pool;
    if ( element_size < sizeof(array_list_block_info) )
    {
        list->element_size = sizeof(array_list_block_info);
    }
    else
    {
        list->element_size = element_size;
    }
    list->size = 0;
    list->first_block = NULL;
    list->last_block = NULL;
    list->elment_count_each_block = (uint32)(block_pool->block_size / list->element_size);
    if ( list->elment_count_each_block < 2 )
    {
        return FALSE;
    }
    return TRUE;
}

void array_list_destroy( array_list* list )
{
    array_list_clear( list );
}

void* array_list_push( array_list* list )
{
    void* block = NULL;
    array_list_block_info* block_info = NULL;
    if ( NULL == list->last_block )
    {
        ASSERT( list->size == 0 );
        ASSERT( list->first_block == NULL );
        block = memory_pool_alloc( list->block_pool );
        if ( NULL == block )
        {
            return NULL;
        }
        list->first_block = list->last_block = block;
        block_info = GET_BLOCK_INFO(list, block);
        block_info->next_block = NULL;
        block_info->prev_block = NULL;
        block_info->used_count = 1;
        ++list->size;
        return block;
    }

    ASSERT( list->size > 0 );
    block_info = GET_BLOCK_INFO(list, list->last_block);
    if ( block_info->used_count < (list->elment_count_each_block - 1) )
    {
        block = (byte*)list->last_block + list->element_size * block_info->used_count;
        ++block_info->used_count;
        ++list->size;
        return block;
    }

    block = memory_pool_alloc( list->block_pool );
    if ( NULL == block )
    {
        return NULL;
    }
    block_info->next_block = block;
    block_info = GET_BLOCK_INFO(list, block);
    block_info->next_block = NULL;
    block_info->prev_block = list->last_block;
    list->last_block = block;
    block_info->used_count = 1;
    ++list->size;
    return block;
}

void array_list_pop( array_list* list )
{
    void* block = NULL;
    array_list_block_info* block_info = NULL;
    if ( NULL != list->last_block )
    {
        block = list->last_block;
        block_info = GET_BLOCK_INFO(list, block);
        ASSERT( block_info->used_count > 0 );
        -- block_info->used_count;
        -- list->size;
        if ( 0 == block_info->used_count )
        {
            list->last_block = block_info->prev_block;
            memory_pool_free( list->block_pool, block );
            if ( NULL == list->last_block )
            {
                ASSERT( 0 == list->size );
                list->first_block = NULL;
            }
            else
            {
                block_info = GET_BLOCK_INFO(list, list->last_block);
                block_info->next_block = NULL;
            }
        }
    }
}

void* array_list_get( array_list* list, uint32 index )
{
    void* block = list->first_block;
    array_list_block_info* block_info = NULL;
    if ( index < list->size )
    {
        ASSERT( NULL != block );
        block_info = GET_BLOCK_INFO(list, block);
        while ( block_info->used_count <= index )
        {
            index -= block_info->used_count;
            block = block_info->next_block;
            ASSERT( NULL != block );
            block_info = GET_BLOCK_INFO(list, block);
        }
        return (byte*)block + list->element_size * index;
    }
    return NULL;
}

void array_list_clear( array_list* list )
{
    void* block = list->first_block;
    void* next_block = NULL;
    array_list_block_info* block_info = NULL;
    while ( NULL != block )
    {
        block_info = GET_BLOCK_INFO(list, block);
        next_block = block_info->next_block;
        memory_pool_free( list->block_pool, block );
        block = next_block;
    }
    list->first_block = NULL;
    list->last_block = NULL;
    list->size = 0;
}

void* array_list_first( array_list* list )
{
    return list->first_block;
}

void* array_list_last( array_list* list )
{
    array_list_block_info* block_info = GET_BLOCK_INFO( list, list->last_block );
    return (byte*)list->last_block + list->element_size * (block_info->used_count - 1);
}

void array_list_iterator_start( array_list_iterator* iter, array_list* list )
{
    iter->list = list;
    iter->block = list->first_block;
    iter->block_info = GET_BLOCK_INFO( list, iter->block );
    iter->current_index = 0;
}

boolean array_list_iterator_is_valid( array_list_iterator* iter )
{
    if ( NULL == iter->block )
    {
        return FALSE;
    }
    return iter->current_index < iter->block_info->used_count;
}

void array_list_iterator_next( array_list_iterator* iter )
{
    ++ iter->current_index;
    if ( iter->current_index < iter->block_info->used_count )
    {
        return;
    }
    iter->block = iter->block_info->next_block;
    if ( NULL != iter->block )
    {
        iter->block_info = GET_BLOCK_INFO( iter->list, iter->block );
    }
    iter->current_index = 0;
}
