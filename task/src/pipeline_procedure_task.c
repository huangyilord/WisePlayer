#include "pipeline_procedure_task.h"
#include "pipeline_procedure.h"
#include "thread.h"

static uint32 pipline_procedure_task_function( void* param, memory_pool* pool );

boolean pipeline_procedure_task_initialize( pipeline_procedure_task* task, procedure_task_function func, void* context )
{
    memset( task, 0, sizeof(pipeline_procedure_task) );
    task->function = func;
    task->context = context;
    if ( !task_initialize( &task->taskbase, pipline_procedure_task_function, task ) )
    {
        return FALSE;
    }
    task->task_start_event = synchronize_create_event();
    if ( NULL == task->task_start_event )
    {
        task_destroy( &task->taskbase );
        return FALSE;
    }
    task->task_end_event = synchronize_create_event();
    if ( NULL == task->task_end_event )
    {
        synchronize_destroy_event( task->task_start_event );
        task_destroy( &task->taskbase );
        return FALSE;
    }
    task->is_running = FALSE;
    return TRUE;
}

void pipeline_procedure_task_destroy( pipeline_procedure_task* task )
{
    synchronize_destroy_event( task->task_start_event );
    synchronize_destroy_event( task->task_end_event );
    task_destroy( &task->taskbase );
}

void pipeline_procedure_task_reset( pipeline_procedure_task* task )
{
    task->is_running = FALSE;
    synchronize_reset_event( task->task_end_event );
    synchronize_reset_event( task->task_start_event );
}

boolean pipeline_procedure_task_start( pipeline_procedure_task* task )
{
    task->is_running = TRUE;
    synchronize_wait_for_event( task->task_start_event );
    return TRUE;
}

void pipeline_procedure_task_stop( pipeline_procedure_task* task )
{
    task->is_running = FALSE;
    synchronize_wait_for_event( task->task_end_event );
}

boolean pipeline_procedure_task_is_running( pipeline_procedure_task* task )
{
    return task->is_running;
}

uint32 pipline_procedure_task_function( void* param, memory_pool* pool )
{
    pipeline_procedure_task* task = (pipeline_procedure_task*)param;
    synchronize_set_event( task->task_start_event );
    while ( task->is_running )
    {
        task->function( task->context );
    }
    synchronize_set_event( task->task_end_event );
    return 0;
}


