//
//  task_processor_thread.h
//  task
//
//  Created by Yi Huang on 8/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _TASK_PROCESSOR_THREAD_H_ )
#define _TASK_PROCESSOR_THREAD_H_

#include "thread.h"
#include "task_processor.h"


typedef struct task_processor_thread
{
    memory_pool         mempool;
    thread_type         thread;
    event_type          pause_status_changed_event;
    task_processor      processor;
    boolean volatile    is_running;
    boolean volatile    is_paused;
} task_processor_thread;

/**
 *  Initialize a task processor thread instance
 *
 *  @param thread:      The thread to initialize
 *  @return:            Returns TRUE if success.
 */
boolean task_processor_thread_initialize( task_processor_thread* thread );

/**
 *  Destroy a task thread instance
 *
 *  @param thread:      The thread to be destroyed
 */
void task_processor_thread_destroy( task_processor_thread* thread );

/**
 *  Get task processor
 *
 *  @param thread:      The thread instance.
 *  @return:            Returns the processor attached to this thread.
 */
task_processor* task_processor_thread_get_processor( task_processor_thread* thread );

/**
 *  Pause the task processor thread.
 *
 *  @param thread:      The processor instance
 */
void task_processor_thread_pause( task_processor_thread* thread );


/**
 *  Continue the task processor thread.
 *
 *  @param thread:      The processor instance
 */
void task_processor_thread_continue( task_processor_thread* thread );

#endif
