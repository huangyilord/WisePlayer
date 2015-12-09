#if !defined ( _PIPELINE_PROCEDURE_TASK_H_ )
#define _PIPELINE_PROCEDURE_TASK_H_

#include "task.h"
#include "hash_map.h"

typedef boolean (*procedure_task_function)( void* context );

typedef struct pipeline_procedure pipeline_procedure;

typedef struct pipeline_procedure_task
{
    task                        taskbase;
    procedure_task_function     function;
    void*                       context;
    event_type                  task_start_event;
    event_type                  task_end_event;
    volatile boolean            is_running;
    uint32                      reserved;
} pipeline_procedure_task;

boolean pipeline_procedure_task_initialize( pipeline_procedure_task* task, procedure_task_function func, void* context );

void pipeline_procedure_task_destroy( pipeline_procedure_task* task );

void pipeline_procedure_task_reset( pipeline_procedure_task* task );

boolean pipeline_procedure_task_start( pipeline_procedure_task* task );

void pipeline_procedure_task_stop( pipeline_procedure_task* task );

boolean pipeline_procedure_task_is_running( pipeline_procedure_task* task );

#endif /* defined(_PIPELINE_PROCEDURE_TASK_H_) */
