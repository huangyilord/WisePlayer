//
//  task_processor_thread_pool.h
//  task
//
//  Created by Yi Huang on 3/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _TASK_PROCESSOR_THREAD_POOL_ )
#define _TASK_PROCESSOR_THREAD_POOL_

#include "task_processor_thread.h"
#include "array_list.h"

typedef struct task_processor_thread_pool
{
    object_pool     processor_thread_pool;
    array_list      threads;
    size_t          max_size;
    mutex_type      mutex;
} task_processor_thread_pool;

/**
 *  Initialize a task_processor_thread_pool instance
 *
 *  @param processor_thread_pool:   the instance
 *  @param mem_pool :               the memory pool
 *  @return:                        Returns TRUE if success
 */
boolean task_processor_thread_pool_initialize( task_processor_thread_pool* processor_thread_pool, uint32 max_size, memory_pool* mem_pool );

/**
 *  Destroy a task_processor_thread_pool instance
 *
 *  @param processor_thread_pool :  the instance to destroy
 */
void task_processor_thread_pool_destroy( task_processor_thread_pool* processor_thread_pool );

/**
 *  Get a processor from the pool
 *
 *  @param pool:            The task_processor_thread_pool instance
 *  @return:                Returns an initialized task_processor_thread
 */
task_processor_thread* task_processor_thread_pool_get( task_processor_thread_pool* pool );

/**
 *  Put a task_processor_thread into the pool
 *
 *  @param pool:            The task_processor_thread_pool instance
 *  @param processor:       The task_processor_thread instance to put
 */
void task_processor_thread_pool_put( task_processor_thread_pool* pool, task_processor_thread* thread );

#endif /* defined(_TASK_PROCESSOR_THREAD_POOL_) */
