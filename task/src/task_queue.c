//
//  task_queue.c
//  task
//
//  Created by Yi Huang on 7/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "task_queue.h"

boolean task_queue_initialize( task_queue* queue )
{
    memory_pool_initialize( &queue->mempool, 1024, 1 );
    if ( !list_initialize( &queue->task_list, sizeof(task*), &queue->mempool ) )
    {
        memory_pool_destroy( &queue->mempool );
        return FALSE;
    }
    if ( !list_initialize( &queue->event_list, sizeof(event_group_element_type), &queue->mempool ) )
    {
        list_destroy( &queue->task_list );
        memory_pool_destroy( &queue->mempool );
        return FALSE;
    }
    queue->mutex = synchronize_create_mutex();
    if ( NULL == queue->mutex )
    {
        list_destroy( &queue->event_list );
        list_destroy( &queue->task_list );
        memory_pool_destroy( &queue->mempool );
        return FALSE;
    }
    return TRUE;
}

void task_queue_destroy( task_queue* queue )
{
    task_queue_clear_task( queue );
    synchronize_destroy_mutex( queue->mutex );
    list_destroy( &queue->event_list );
    list_destroy( &queue->task_list );
    memory_pool_destroy( &queue->mempool );
    memset( queue, 0, sizeof(task_queue) );
}

boolean task_queue_add_task( task_queue* queue, task* ptask )
{
    boolean ret = FALSE;
    event_group_element_type element;
    list_iterator iter;
    synchronize_lock_mutex( queue->mutex );
    ret = list_push_back( &queue->task_list, &ptask );
    if ( ret )
    {
        ptask->status = task_status_scheduling;
        LIST_ITERATOR_START( iter, &queue->event_list );
        while ( LIST_ITERATOR_IS_VALID( iter ) )
        {
            element = *(event_group_element_type*)LIST_ITERATOR_GET( iter );
            synchronize_event_group_set_element( element );
            LIST_ITERATOR_NEXT( iter );
        }
    }
    synchronize_unlock_mutex( queue->mutex );
    return ret;
}

task* task_queue_pop_task( task_queue* queue )
{
    task* ptask = NULL;
    list_iterator iter;
    synchronize_lock_mutex( queue->mutex );
    if ( !LIST_IS_EMPTY( &queue->task_list ) )
    {
        LIST_ITERATOR_START( iter, &queue->task_list );
        ptask = *(task**)LIST_ITERATOR_GET( iter );
        list_remove( &queue->task_list, iter );
    }
    synchronize_unlock_mutex( queue->mutex );
    return ptask;
}

void task_queue_clear_task( task_queue* queue )
{
    synchronize_lock_mutex( queue->mutex );
    list_clear( &queue->task_list );
    synchronize_unlock_mutex( queue->mutex );
}
