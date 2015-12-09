//
//  task_queue.h
//  task
//
//  Created by Yi Huang on 3/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _TASK_QUEUE_H_ )
#define _TASK_QUEUE_H_

#include "task.h"
#include "synchronize.h"
#include "list.h"

typedef struct task_queue
{
    memory_pool                 mempool;
    list                        task_list;
    list                        event_list;
    mutex_type                  mutex;
} task_queue;

/**
 *  Initialize a task queue instance
 *
 *  @param queue:               The queue instance.
 *  @return:                    Returns TRUE if success.
 */
boolean task_queue_initialize( task_queue* queue );


/**
 *  Destroy a task queue instance
 *
 *  @param queue:               The queue instance to destroy.
 */
void task_queue_destroy( task_queue* queue );

/**
 *  Add a task at the tail of a task queue.
 *
 *  @param queue:               The queue instance.
 *  @param ptask:               The task instance to add.
 *  @return:                    Returns TRUE if success.
 */
boolean task_queue_add_task( task_queue* queue, task* ptask );

/**
 *  Remove the first task from the queue.
 *
 *  @param queue:               The queue instance.
 *  @return:                    Returns the first task.
 *                              If the queue is empty, the return value would be NULL.
 */
task* task_queue_pop_task( task_queue* queue );

/**
 *  Clear all tasks.
 *
 *  @param queue:               The queue instance
 */
void task_queue_clear_task( task_queue* queue );

#endif
