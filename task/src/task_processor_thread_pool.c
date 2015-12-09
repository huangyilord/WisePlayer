//
//  task_processor_thread_pool.c
//  task
//
//  Created by Yi Huang on 3/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "task_processor_thread_pool.h"
#include "common_debug.h"

boolean task_processor_thread_pool_initialize( task_processor_thread_pool* pool, uint32 max_size, memory_pool* mem_pool )
{
    pool->mutex = synchronize_create_mutex();
    if ( NULL == pool->mutex )
    {
        return FALSE;
    }
    if ( !object_pool_initialize( &pool->processor_thread_pool, mem_pool, sizeof(task_processor_thread)))
    {
        synchronize_destroy_mutex( pool->mutex );
        return FALSE;
    }
    if ( !array_list_initialize( &pool->threads, sizeof(task_processor*), mem_pool) )
    {
        object_pool_destroy( &pool->processor_thread_pool );
        synchronize_destroy_mutex( pool->mutex );
        return FALSE;
    }
    pool->max_size = max_size;
    return TRUE;
}

void task_processor_thread_pool_destroy( task_processor_thread_pool* pool )
{
    synchronize_lock_mutex( pool->mutex );
    array_list_destroy( &pool->threads );
    object_pool_destroy( &pool->processor_thread_pool );
    synchronize_unlock_mutex( pool->mutex );
    synchronize_destroy_mutex( pool->mutex );
}


task_processor_thread* task_processor_thread_pool_get( task_processor_thread_pool* pool )
{
    task_processor_thread* thread = NULL;
    synchronize_lock_mutex( pool->mutex );
    if ( ARRAY_LIST_IS_EMPTY( &pool->threads ))
    {
        thread = object_pool_alloc( &pool->processor_thread_pool );
        if ( NULL == thread )
        {
            synchronize_unlock_mutex( pool->mutex );
            return NULL;
        }
        if ( !task_processor_thread_initialize( thread ) )
        {
            object_pool_free( &pool->processor_thread_pool, thread );
            synchronize_unlock_mutex( pool->mutex );
            return NULL;
        }
    }
    else
    {
        thread = *(task_processor_thread**)array_list_last( &pool->threads );
        ASSERT( NULL != thread );
        array_list_pop( &pool->threads );
    }
    synchronize_unlock_mutex( pool->mutex );
    task_processor_thread_continue( thread );
    return thread;
}

void task_processor_thread_pool_put( task_processor_thread_pool* pool, task_processor_thread* thread )
{
    task_processor_thread** p = NULL;
    ASSERT( NULL != thread );
    task_processor_thread_pause( thread );
    synchronize_lock_mutex( pool->mutex );
    if ( ARRAY_LIST_SIZE( &pool->threads ) < pool->max_size )
    {
        p = (task_processor_thread**)array_list_push( &pool->threads );
        if ( NULL != p )
        {
            *p = thread;
            synchronize_unlock_mutex( pool->mutex );
            return;
        }
    }
    task_processor_thread_destroy( thread );
    object_pool_free( &pool->processor_thread_pool, thread );
    synchronize_unlock_mutex( pool->mutex );
}