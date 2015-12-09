#include "task.h"

#include "common_debug.h"

boolean task_initialize( task* ptask, task_function func, void* param )
{
    ASSERT( NULL != ptask );
    ASSERT( NULL != func );
    ptask->func = func;
    ptask->param = param;
    ptask->status = task_status_done;
    ptask->return_value = 0;
    return TRUE;
}

void task_destroy( task* ptask )
{
    memset( ptask, 0, sizeof(task) );
}
