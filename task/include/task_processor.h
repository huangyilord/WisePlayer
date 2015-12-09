#if !defined ( _TASK_PROCESSOR_ )
#define _TASK_PROCESSOR_

#include "task.h"
#include "synchronize.h"
#include "task_queue.h"
#include "hash_map.h"

typedef struct task_processor
{
    memory_pool         mempool;
    hash_map            task_queues;
    event_group_type    event_group;
    mutex_type          mutex;
} task_processor;

/**
 *  Initialize a task processor instance
 *
 *  @param processor:   The processor to initialize
 */
boolean task_processor_initialize( task_processor* processor );

/**
 *  Destroy a task instance
 *
 *  @param processor:   The processor to be destroyed
 */
void task_processor_destroy( task_processor* processor );

/**
 *  Add a task queue to the processor
 *
 *  @param processor:   The processor
 *  @param queue:       The task queue to add
 *  @return:            Returns TRUE if succeeded
 */
boolean task_processor_add_task_queue( task_processor* processor, task_queue* queue );

/**
 *  Remove a task queue from a processor
 *
 *  @param processor:   The processor
 *  @param queue:       The task queue to remove
 */
void task_processor_remove_task_queue( task_processor* processor, task_queue* queue );

/**
 *  Clear all task queues.
 *
 *  @param processor:   The processor instance
 */
void task_processor_clear_task_queue( task_processor* processor );

/**
 *  Process a task
 *
 *  @param processor:   The processor instance
 */
void task_processor_process_task( task_processor* processor, memory_pool* mempool );

#endif
