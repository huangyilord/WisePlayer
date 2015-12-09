#if !defined ( _TASK_H_ )
#define _TASK_H_

#include "memory_pool.h"

typedef uint32 (*task_function)( void* user_data, memory_pool* pool );

typedef enum task_status
{
    task_status_done    = 0
    , task_status_scheduling
    , task_status_processing
    , task_status_error
} task_status;

typedef struct task
{
    task_function   func;
    void*           param;
    task_status     status;
    uint32          return_value;
    uint32          reserved;
} task;

/**
 *  Create a new task instance
 *
 *  @param ptask:   The task instance to be initialize
 *  @param func:    The task function
 *  @param param:   The task parameter
 *  @return:        Returns TRUE if success
 */
boolean task_initialize( task* ptask, task_function func, void* param );

/**
 *  Destroy a task instance
 *
 *  @param ptask:   The task instance to be destroyed
 */
void task_destroy( task* ptask );

#endif /* defined(_TASK_H_) */
