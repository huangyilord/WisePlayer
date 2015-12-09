#include "task_processor.h"

#include "atomic.h"
#include "common_debug.h"

boolean task_processor_initialize( task_processor* processor )
{
    memory_pool_initialize( &processor->mempool, 512, 4 );
    if ( !hash_map_initialize( &processor->task_queues, sizeof(event_group_element_type), sizeof(task_queue*), &processor->mempool ) )
    {
        memory_pool_destroy( &processor->mempool );
        return FALSE;
    }
    processor->event_group = synchronize_create_event_group();
    if ( NULL == processor->event_group )
    {
        hash_map_destroy( &processor->task_queues );
        memory_pool_destroy( &processor->mempool );
        return FALSE;
    }
    processor->mutex = synchronize_create_mutex();
    if ( NULL == processor->mutex )
    {
        synchronize_destroy_event_group( processor->event_group );
        hash_map_destroy( &processor->task_queues );
        memory_pool_destroy( &processor->mempool );
        return FALSE;
    }
    return TRUE;
}

void task_processor_destroy( task_processor* processor )
{
    task_processor_clear_task_queue( processor );

    synchronize_destroy_mutex( processor->mutex );
    synchronize_destroy_event_group( processor->event_group );
    hash_map_destroy( &processor->task_queues );
    memory_pool_destroy( &processor->mempool );
}

boolean task_processor_add_task_queue( task_processor* processor, task_queue* queue )
{
    boolean ret = FALSE;
    event_group_element_type group_element;
    ASSERT( NULL != queue );
    synchronize_lock_mutex( processor->mutex );
    do
    {
        group_element = synchronize_event_group_add_element( processor->event_group );
        if ( NULL == group_element )
        {
            ret = FALSE;
            break;
        }
        ret = hash_map_insert( &processor->task_queues, &group_element, &queue );
        if ( !ret )
        {
            synchronize_event_group_remove_element( processor->event_group, group_element );
            break;
        }
        synchronize_lock_mutex( queue->mutex );
        ret = list_push_back( &queue->event_list, &group_element );
        synchronize_unlock_mutex( queue->mutex );
        if ( !ret )
        {
            hash_map_remove( &processor->task_queues, &group_element );
            synchronize_event_group_remove_element( processor->event_group, group_element );
            break;
        }
    } while (FALSE);
    synchronize_unlock_mutex( processor->mutex );
    return ret;
}

void task_processor_remove_task_queue( task_processor* processor, task_queue* queue )
{
    hash_map_iterator iter;
    event_group_element_type group_element = NULL;
    synchronize_lock_mutex( processor->mutex );
    HASH_MAP_ITERATOR_START( iter, &processor->task_queues );
    while ( HASH_MAP_ITERATOR_IS_VALID( iter ) )
    {
        if ( queue == *(task_queue**)hash_map_get( &processor->task_queues, HASH_MAP_ITERATOR_GET( iter ) ) )
        {
            hash_map_remove( &processor->task_queues, HASH_MAP_ITERATOR_GET( iter ) );
            synchronize_lock_mutex( queue->mutex );
            LIST_ITERATOR_START( iter, &queue->event_list );
            while ( LIST_ITERATOR_IS_VALID( iter ) )
            {
                group_element = *(event_group_element_type*)LIST_ITERATOR_GET( iter );
                if ( synchronize_event_group_get_group_by_element( group_element ) == processor->event_group )
                {
                    list_remove( &queue->event_list, iter );
                    break;
                }
                LIST_ITERATOR_NEXT( iter );
            }
            synchronize_unlock_mutex( queue->mutex );
            ASSERT( synchronize_event_group_get_group_by_element( group_element ) == processor->event_group );
            synchronize_event_group_remove_element( processor->event_group, group_element );
            break;
        }
        HASH_MAP_ITERATOR_NEXT( iter );
    }
    synchronize_unlock_mutex( processor->mutex );
}

void task_processor_clear_task_queue( task_processor* processor )
{
    hash_map_iterator iter_queue;
    list_iterator iter_event;
    event_group_element_type group_element = NULL;
    task_queue* queue = NULL;
    synchronize_lock_mutex( processor->mutex );
    HASH_MAP_ITERATOR_START( iter_queue, &processor->task_queues );
    while ( HASH_MAP_ITERATOR_IS_VALID( iter_queue ) )
    {
        group_element = *(event_group_element_type*)HASH_MAP_ITERATOR_GET( iter_queue );
        queue = *(task_queue**)hash_map_get( &processor->task_queues, &group_element );
        synchronize_lock_mutex( queue->mutex );
        LIST_ITERATOR_START( iter_event, &queue->event_list );
        while ( LIST_ITERATOR_IS_VALID( iter_event ) )
        {
            if ( group_element == *(event_group_element_type*)LIST_ITERATOR_GET( iter_event ) )
            {
                list_remove( &queue->event_list, iter_event );
                break;
            }
            LIST_ITERATOR_NEXT( iter_event );
        }
        synchronize_unlock_mutex( queue->mutex );
        ASSERT( synchronize_event_group_get_group_by_element( group_element ) == processor->event_group );
        synchronize_event_group_remove_element( processor->event_group, group_element );
        HASH_MAP_ITERATOR_NEXT( iter_queue );
    }
    hash_map_clear( &processor->task_queues );
    synchronize_unlock_mutex( processor->mutex );
}

void task_processor_process_task( task_processor* processor, memory_pool* mempool )
{
    event_group_element_type element = NULL;
    task_queue** queue = NULL;
    task* ptask = NULL;
    do
    {
        element = synchronize_event_group_wait_for_any( processor->event_group );
        if ( NULL == element )
        {
            return;
        }
        synchronize_lock_mutex( processor->mutex );
        queue = hash_map_get( &processor->task_queues, &element );
        synchronize_unlock_mutex( processor->mutex );
        // queue may have been removed
        if ( NULL == queue )
        {
            return;
        }
        ptask = task_queue_pop_task( *queue );
    } while ( NULL == ptask );
    ptask->status = task_status_processing;
    ptask->return_value = ptask->func( ptask->param, mempool );
    ptask->status = task_status_done;
}

