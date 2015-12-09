//
//  task_processor_thread.c
//  task
//
//  Created by Yi Huang on 8/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "task_processor_thread.h"

#include "atomic.h"
#include "common_debug.h"

static uint32 process_task( void* user_data );

boolean task_processor_thread_initialize( task_processor_thread* thread )
{
    memory_pool_initialize( &thread->mempool, 1024, 4 );
    thread->is_running = TRUE;
    thread->is_paused = FALSE;
    if ( !task_processor_initialize( &thread->processor ) )
    {
        return FALSE;
    }
    thread->pause_status_changed_event = synchronize_create_event();
    if ( NULL == thread->pause_status_changed_event )
    {
        task_processor_destroy( &thread->processor );
        return FALSE;
    }
    if ( !thread_create_thread( &thread->thread, process_task, thread ) )
    {
        task_processor_destroy( &thread->processor );
        synchronize_destroy_event( thread->pause_status_changed_event );
        return FALSE;
    }
    return TRUE;
}

void task_processor_thread_destroy( task_processor_thread* thread )
{
    if ( thread->is_running )
    {
        thread->is_paused = FALSE;
        thread->is_running = FALSE;
        synchronize_set_event( thread->pause_status_changed_event );
        task_processor_clear_task_queue( &thread->processor );
        thread_wait_for_thread( &thread->thread );
        task_processor_destroy( &thread->processor );
        synchronize_destroy_event( thread->pause_status_changed_event );
        memory_pool_destroy( &thread->mempool );
    }
}

task_processor* task_processor_thread_get_processor( task_processor_thread* thread )
{
    return &thread->processor;
}

void task_processor_thread_pause( task_processor_thread* thread )
{
    thread->is_paused = TRUE;
}

void task_processor_thread_continue( task_processor_thread* thread )
{
    thread->is_paused = FALSE;
    synchronize_set_event( thread->pause_status_changed_event );
}

uint32 process_task( void* user_data )
{
    memory_pool mempool = {0};
    memory_pool_initialize( &mempool, 1024, 2 );
    task_processor_thread* thread = (task_processor_thread*)user_data;
    while ( thread->is_running )
    {
        while ( thread->is_paused )
        {
            synchronize_wait_for_event( thread->pause_status_changed_event );
        }
        task_processor_process_task( &thread->processor, &mempool );
    }
    memory_pool_destroy( &mempool );
    return 0;
}

