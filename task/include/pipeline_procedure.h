//
//  pipeline_procedure.h
//  task
//
//  Created by Yi Huang on 3/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _PIPE_LINE_PROCEDURE_H_ )
#define _PIPE_LINE_PROCEDURE_H_

#include "task_processor_thread_pool.h"
#include "hash_map.h"
#include "synchronize.h"

typedef struct pipeline_procedure_input pipeline_procedure_input;
typedef struct pipeline_procedure_output pipeline_procedure_output;
typedef struct pipeline_procedure pipeline_procedure;
typedef struct pipeline_procedure_task pipeline_procedure_task;

typedef boolean (*procedure_update_function)( pipeline_procedure* procedure, void* context );
typedef boolean (*procedure_data_handler)( pipeline_procedure* procedure, void* context, const void* key, void* data );

typedef struct pipeline_procedure_config
{
    procedure_update_function   update_function;
    procedure_data_handler      input_data_handler;
    void*                       context;
    size_t                      key_size;
    task_processor_thread_pool* thread_pool;
} pipeline_procedure_config;

typedef struct pipeline_procedure
{
    memory_pool                 mem_pool;
    pipeline_procedure_config   config;
    pipeline_procedure_task*    procedure_task;
    task_processor_thread*      thread;
    task_queue                  procedure_task_queue;
    hash_map                    input;
    hash_map                    output;
    event_group_type            input_event;
} pipeline_procedure;

/**
 *  Initialize a pipeline instance.
 *
 *  @param procedure:       The pipeline instance.
 *  @return:                Returns TRUE if success.
 */
boolean pipeline_procedure_initialize( pipeline_procedure* procedure, const pipeline_procedure_config* config );

/**
 *  Destroy a pipeline instance.
 *
 *  @param procedure:       The pipeline instance.
 */
void pipeline_procedure_destroy( pipeline_procedure* procedure );

/**
 *  Add an input to a procedure.
 *
 *  @param procedure:       The procedure instance.
 *  @param key:             The key of input to add.
 *  @return:                Returns TRUE if success.
 */
pipeline_procedure_input* pipeline_procedure_add_input( pipeline_procedure* procedure, const void* key );

/**
 *  Remove an input.
 *
 *  @param procedure:       The procedure instance.
 *  @param key:             The key of input to remove.
 */
void pipeline_procedure_remove_input( pipeline_procedure* procedure, const void* key );

/**
 *  Remove all inputs.
 *
 *  @param procedure:       The procedure instance.
 */
void pipeline_procedure_clear_input( pipeline_procedure* procedure );

/**
 *  Add an output to a procedure.
 *
 *  @param procedure:       The procedure instance.
 *  @param key:             The key of output to add.
 *  @return:                Returns TRUE if success.
 */
pipeline_procedure_output* pipeline_procedure_add_output( pipeline_procedure* procedure, const void* key );

/**
 *  Remove an output.
 *
 *  @param procedure:       The procedure instance.
 *  @param key:             The key of output to remove.
 */
void pipeline_procedure_remove_output( pipeline_procedure* procedure, const void* key );

/**
 *  Remove all outputs.
 *
 *  @param procedure:       The procedure instance.
 */
void pipeline_procedure_clear_output( pipeline_procedure* procedure );

/**
 *  Start a procedure.
 *
 *  @param procedure:       The procedure instance.
 *  @return:                Returns TRUE if success.
 */
boolean pipeline_procedure_start( pipeline_procedure* procedure );

/**
 *  Stop a procedure.
 *
 *  @param procedure:       The procedure instance.
 */
void pipeline_procedure_stop( pipeline_procedure* procedure );

/**
 *  Try to process data if there is input from a specific input.
 *  This function will return immediately.
 *
 *  @param procedure:       The procedure instance.
 *  @param key:             The input key.
 *  @return:                Returns TRUE if handled any input.
 */
boolean pipeline_procedure_try_handle_input( pipeline_procedure* procedure, const void* key );

/**
 *  Process data from a specific input.
 *  This function will block until there is input data.
 *
 *  @param procedure:       The procedure instance.
 *  @param key:             The input key.
 *  @return:                Returns TRUE if handled any input.
 */
boolean pipeline_procedure_handle_input( pipeline_procedure* procedure, const void* key );

/**
 *  Process any input data.
 *  This function will block until there is input data from any inputs.
 *
 *  @param procedure:       The procedure instance.
 *  @return:                Returns TRUE if handled any input.
 */
boolean pipeline_procedure_handle_any_input( pipeline_procedure* procedure );

/**
 *  Try to process any input data.
 *  This function will return immediately.
 *
 *  @param procedure:       The procedure instance.
 *  @return:                Returns TRUE if handled any input.
 */
boolean pipeline_procedure_try_handle_any_input( pipeline_procedure* procedure );

/**
 *  Try to get block of memory to output.
 *  This function will return immediately.
 *  You can request only one buffer each time. Before you commit the buffer, this function will always return the same buffer.
 *
 *  @param procedure:       The procedure instance.
 *  @param key:             The output key.
 *  @return:                Returns NULL if failed to get a buffer to output.
 */
void* pipeline_procedure_try_get_output_buffer( pipeline_procedure* procedure, const void* key, size_t data_size );

/**
 *  Wait to get a block to output.
 *  This function will block until there is an availible block or the pipeline has terminated.
 *  You can request only one buffer each time. Before you commit the buffer, this function will always return the same buffer.
 *
 *  @param procedure:       The procedure instance.
 *  @param key:             The output key.
 *  @return:                Returns NULL if failed to get a block to output.
 */
void* pipeline_procedure_get_output_buffer( pipeline_procedure* procedure, const void* key, size_t data_size );

/**
 *  Commit the output buffer, returned by @pipeline_procedure_try_get_output_buffer or @pipeline_procedure_get_output_buffer.
 *
 *  @param procedure:       The procedure instance.
 *  @param key:             The output key.
 */
void pipeline_procedure_commit_output_buffer( pipeline_procedure* procedure, const void* key );

#endif /* defined( _PIPE_LINE_PROCEDURE_H_ ) */
