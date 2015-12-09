//
//  pipeline_procedure.c
//  task
//
//  Created by Yi Huang on 3/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "pipeline_procedure.h"
#include "pipeline_procedure_task.h"
#include "pipeline_procedure_structures.h"
#include "common_debug.h"

static boolean pipeline_procedure_task_function( void* context );

boolean pipeline_procedure_initialize( pipeline_procedure* procedure, const pipeline_procedure_config* config )
{
    memset( procedure, 0, sizeof(pipeline_procedure) );
    procedure->config = *config;
    memory_pool_initialize( &procedure->mem_pool, 1024, 1 );
    if ( !hash_map_initialize( &procedure->input, config->key_size, sizeof(pipeline_procedure_input), &procedure->mem_pool ) )
    {
        memory_pool_destroy( &procedure->mem_pool );
        return FALSE;
    }
    if ( !hash_map_initialize( &procedure->output, config->key_size, sizeof(pipeline_procedure_output), &procedure->mem_pool ) )
    {
        hash_map_destroy( &procedure->input );
        memory_pool_destroy( &procedure->mem_pool );
        return FALSE;
    }
    procedure->procedure_task = malloc( sizeof(pipeline_procedure_task) );
    if ( NULL == procedure->procedure_task )
    {
        hash_map_destroy( &procedure->input );
        hash_map_destroy( &procedure->output );
        memory_pool_destroy( &procedure->mem_pool );
    }
    if ( !pipeline_procedure_task_initialize( procedure->procedure_task, pipeline_procedure_task_function, procedure ) )
    {
        free( procedure->procedure_task );
        hash_map_destroy( &procedure->input );
        hash_map_destroy( &procedure->output );
        memory_pool_destroy( &procedure->mem_pool );
        return FALSE;
    }
    if ( !task_queue_initialize( &procedure->procedure_task_queue ) )
    {
        pipeline_procedure_task_destroy( procedure->procedure_task );
        free( procedure->procedure_task );
        hash_map_destroy( &procedure->input );
        hash_map_destroy( &procedure->output );
        memory_pool_destroy( &procedure->mem_pool );
    }
    procedure->input_event = synchronize_create_event_group();
    if ( NULL == procedure->input_event )
    {
        task_queue_destroy( &procedure->procedure_task_queue );
        pipeline_procedure_task_destroy( procedure->procedure_task );
        free( procedure->procedure_task );
        hash_map_destroy( &procedure->input );
        hash_map_destroy( &procedure->output );
        memory_pool_destroy( &procedure->mem_pool );
    }
    procedure->thread = task_processor_thread_pool_get( config->thread_pool );
    if ( NULL == procedure->thread )
    {
        synchronize_destroy_event_group( procedure->input_event );
        task_queue_destroy( &procedure->procedure_task_queue );
        pipeline_procedure_task_destroy( procedure->procedure_task );
        free( procedure->procedure_task );
        hash_map_destroy( &procedure->input );
        hash_map_destroy( &procedure->output );
        memory_pool_destroy( &procedure->mem_pool );
        return FALSE;
    }
    if ( !task_processor_add_task_queue( task_processor_thread_get_processor( procedure->thread ), &procedure->procedure_task_queue ) )
    {
        task_processor_thread_pool_put( config->thread_pool, procedure->thread );
        synchronize_destroy_event_group( procedure->input_event );
        task_queue_destroy( &procedure->procedure_task_queue );
        pipeline_procedure_task_destroy( procedure->procedure_task );
        free( procedure->procedure_task );
        hash_map_destroy( &procedure->input );
        hash_map_destroy( &procedure->output );
        memory_pool_destroy( &procedure->mem_pool );
    }
    return TRUE;
}

void pipeline_procedure_destroy( pipeline_procedure* procedure )
{
    pipeline_procedure_stop( procedure );
    pipeline_procedure_clear_input( procedure );
    pipeline_procedure_clear_output( procedure );

    task_processor_clear_task_queue( task_processor_thread_get_processor( procedure->thread ) );
    task_processor_thread_pool_put( procedure->config.thread_pool, procedure->thread );
    synchronize_destroy_event_group( procedure->input_event );
    task_queue_destroy( &procedure->procedure_task_queue );
    pipeline_procedure_task_destroy( procedure->procedure_task );
    free( procedure->procedure_task );
    hash_map_destroy( &procedure->input );
    hash_map_destroy( &procedure->output );
    memory_pool_destroy( &procedure->mem_pool );
    memset( procedure, 0, sizeof(pipeline_procedure) );
}

pipeline_procedure_input* pipeline_procedure_add_input( pipeline_procedure* procedure, const void* key )
{
    pipeline_procedure_input* result = NULL;
    pipeline_procedure_input input;
    if ( !hash_map_insert( &procedure->input, key, &input ) )
    {
        return NULL;
    }
    result = hash_map_get( &procedure->input, key );
    ASSERT( NULL != result );
    if ( !pipeline_procedure_input_initialize( result, procedure, &procedure->mem_pool ) )
    {
        hash_map_remove( &procedure->input, key );
        return NULL;
    }
    return result;
}

void pipeline_procedure_remove_input( pipeline_procedure* procedure, const void* key )
{
    pipeline_procedure_input* input = hash_map_get( &procedure->input, key );
    if ( NULL != input )
    {
        pipeline_procedure_input_destroy( input );
        hash_map_remove( &procedure->input, key );
    }
}

void pipeline_procedure_clear_input( pipeline_procedure* procedure )
{
    pipeline_procedure_input* input = NULL;
    hash_map_iterator iter;
    HASH_MAP_ITERATOR_START( iter, &procedure->input );
    while ( HASH_MAP_ITERATOR_IS_VALID( iter ) )
    {
        input = hash_map_get( &procedure->input, HASH_MAP_ITERATOR_GET( iter ) );
        pipeline_procedure_input_destroy( input );
        HASH_MAP_ITERATOR_NEXT( iter );
    }
    hash_map_clear( &procedure->input );
}

pipeline_procedure_output* pipeline_procedure_add_output( pipeline_procedure* procedure, const void* key )
{
    pipeline_procedure_output* result = NULL;
    pipeline_procedure_output output;
    if ( !hash_map_insert( &procedure->output, key, &output ) )
    {
        return NULL;
    }
    result = hash_map_get( &procedure->output, key );
    ASSERT( NULL != result );
    if ( !pipeline_procedure_output_initialize( result, procedure, &procedure->mem_pool ) )
    {
        hash_map_remove( &procedure->output, key );
        return NULL;
    }
    return result;
}

void pipeline_procedure_remove_output( pipeline_procedure* procedure, const void* key )
{
    pipeline_procedure_output* output = hash_map_get( &procedure->output, key );
    if ( NULL != output )
    {
        pipeline_procedure_output_destroy( output );
        hash_map_remove( &procedure->output, key );
    }
}

void pipeline_procedure_clear_output( pipeline_procedure* procedure )
{
    pipeline_procedure_output* output = NULL;
    hash_map_iterator iter;
    HASH_MAP_ITERATOR_START( iter, &procedure->output );
    while ( HASH_MAP_ITERATOR_IS_VALID( iter ) )
    {
        output = hash_map_get( &procedure->output, HASH_MAP_ITERATOR_GET( iter ) );
        pipeline_procedure_output_destroy( output );
        HASH_MAP_ITERATOR_NEXT( iter );
    }
    hash_map_clear( &procedure->output );
}

boolean pipeline_procedure_start( pipeline_procedure* procedure )
{
    if ( !pipeline_procedure_task_is_running( procedure->procedure_task ) )
    {
        pipeline_procedure_task_reset( procedure->procedure_task );
        if ( !task_queue_add_task( &procedure->procedure_task_queue, &procedure->procedure_task->taskbase ) )
        {
            return FALSE;
        }
        return pipeline_procedure_task_start( procedure->procedure_task );
    }
    return TRUE;
}

void pipeline_procedure_stop( pipeline_procedure* procedure )
{
    pipeline_procedure_output* output = NULL;
    pipeline_procedure_input* input = NULL;
    hash_map_iterator iter;
    if ( pipeline_procedure_task_is_running( procedure->procedure_task ) )
    {
        procedure->procedure_task->is_running = FALSE;
        HASH_MAP_ITERATOR_START( iter, &procedure->output );
        while ( HASH_MAP_ITERATOR_IS_VALID( iter ) )
        {
            output = hash_map_get( &procedure->output, HASH_MAP_ITERATOR_GET( iter ) );
            pipeline_procedure_output_shutdown( output );
            HASH_MAP_ITERATOR_NEXT( iter );
        }
        HASH_MAP_ITERATOR_START( iter, &procedure->input );
        while ( HASH_MAP_ITERATOR_IS_VALID( iter ) )
        {
            input = hash_map_get( &procedure->input, HASH_MAP_ITERATOR_GET( iter ) );
            pipeline_procedure_input_shutdown( input );
            HASH_MAP_ITERATOR_NEXT( iter );
        }
        pipeline_procedure_task_stop( procedure->procedure_task );
    }
    HASH_MAP_ITERATOR_START( iter, &procedure->output );
    while ( HASH_MAP_ITERATOR_IS_VALID( iter ) )
    {
        output = hash_map_get( &procedure->output, HASH_MAP_ITERATOR_GET( iter ) );
        pipeline_procedure_output_reset( output );
        HASH_MAP_ITERATOR_NEXT( iter );
    }
    HASH_MAP_ITERATOR_START( iter, &procedure->input );
    while ( HASH_MAP_ITERATOR_IS_VALID( iter ) )
    {
        input = hash_map_get( &procedure->input, HASH_MAP_ITERATOR_GET( iter ) );
        pipeline_procedure_input_reset( input );
        HASH_MAP_ITERATOR_NEXT( iter );
    }
}

boolean pipeline_procedure_task_function( void* context )
{
    pipeline_procedure* procedure = (pipeline_procedure*)context;
    return procedure->config.update_function( procedure, procedure->config.context );
}

boolean pipeline_procedure_try_handle_input( pipeline_procedure* procedure, const void* key )
{
    boolean ret = FALSE;
    exchange_data_desc* desc = NULL;
    pipeline_procedure_input* input = NULL;
    if ( pipeline_procedure_task_is_running( procedure->procedure_task ) )
    {
        input = hash_map_get( &procedure->input, key );
        desc = pipeline_procedure_input_try_get_data( input );
        if ( NULL != desc )
        {
            switch ( desc->type )
            {
                case exchange_data_custom:
                    ret = procedure->config.input_data_handler( procedure, procedure->config.context, key, desc->data );
                    if ( ret )
                    {
                        pipeline_procedure_input_moveon( input );
                    }
                    break;
                case exchange_data_shutdown:
                    break;
                default:
                    break;
            }
        }
    }
    return ret;
}

boolean pipeline_procedure_handle_input( pipeline_procedure* procedure, const void* key )
{
    boolean ret = FALSE;
    exchange_data_desc* desc = NULL;
    pipeline_procedure_input* input = NULL;
    if ( pipeline_procedure_task_is_running( procedure->procedure_task ) )
    {
        input = hash_map_get( &procedure->input, key );
        desc = pipeline_procedure_input_get_data( input );
        if ( NULL != desc )
        {
            switch ( desc->type )
            {
                case exchange_data_custom:
                    ret = procedure->config.input_data_handler( procedure, procedure->config.context, key, desc->data );
                    if ( ret )
                    {
                        pipeline_procedure_input_moveon( input );
                    }
                    break;
                case exchange_data_shutdown:
                    break;
                default:
                    break;
            }
        }
    }
    return ret;
}

boolean pipeline_procedure_handle_any_input( pipeline_procedure* procedure )
{
    boolean ret = FALSE;
    const void* key = NULL;
    pipeline_procedure_input* input = NULL;
    exchange_data_desc* desc = NULL;
    event_group_element_type element = NULL;
    hash_map_iterator iter;
    while ( pipeline_procedure_task_is_running( procedure->procedure_task ) )
    {
        HASH_MAP_ITERATOR_START( iter, &procedure->input );
        while ( HASH_MAP_ITERATOR_IS_VALID( iter ) )
        {
            key = HASH_MAP_ITERATOR_GET( iter );
            input = hash_map_get( &procedure->input, key );
            desc = pipeline_procedure_input_try_get_data( input );
            if ( NULL != desc )
            {
                switch ( desc->type )
                {
                    case exchange_data_custom:
                        ret = procedure->config.input_data_handler( procedure, procedure->config.context, key, desc->data );
                        if ( ret )
                        {
                            pipeline_procedure_input_moveon( input );
                        }
                        return ret;
                    case exchange_data_shutdown:
                        return FALSE;
                    default:
                        return FALSE;
                }
            }
            HASH_MAP_ITERATOR_NEXT( iter );
        }
        element = synchronize_event_group_wait_for_any( procedure->input_event );
    }
    return ret;
}

boolean pipeline_procedure_try_handle_any_input( pipeline_procedure* procedure )
{
    boolean ret = FALSE;
    const void* key = NULL;
    pipeline_procedure_input* input = NULL;
    exchange_data_desc* desc = NULL;
    event_group_element_type element = NULL;
    hash_map_iterator iter;
    if ( pipeline_procedure_task_is_running( procedure->procedure_task ) )
    {
        element = synchronize_event_group_try_wait_for_any( procedure->input_event );
        HASH_MAP_ITERATOR_START( iter, &procedure->input );
        while ( HASH_MAP_ITERATOR_IS_VALID( iter ) )
        {
            key = HASH_MAP_ITERATOR_GET( iter );
            input = hash_map_get( &procedure->input, key );
            desc = pipeline_procedure_input_try_get_data( input );
            if ( NULL != desc )
            {
                switch ( desc->type )
                {
                    case exchange_data_custom:
                        ret = procedure->config.input_data_handler( procedure, procedure->config.context, key, desc->data );
                        if ( ret )
                        {
                            pipeline_procedure_input_moveon( input );
                        }
                        return ret;
                    case exchange_data_shutdown:
                        return FALSE;
                    default:
                        return FALSE;
                }
            }
            HASH_MAP_ITERATOR_NEXT( iter );
        }
    }
    return ret;
}

void* pipeline_procedure_try_get_output_buffer( pipeline_procedure* procedure, const void* key, size_t data_size )
{
    exchange_data_desc* desc = NULL;
    pipeline_procedure_output* output = hash_map_get( &procedure->output, key );
    if ( !pipeline_procedure_task_is_running( procedure->procedure_task ) )
    {
        return NULL;
    }
    pipeline_procedure_output_set_data_size( output, data_size );
    desc = pipeline_procedure_output_try_get_data( output );
    if ( NULL == desc )
    {
        return NULL;
    }
    switch ( desc->type )
    {
        case exchange_data_custom:
            return desc->data;
        case exchange_data_shutdown:
        default:
            return NULL;
    }
}

void* pipeline_procedure_get_output_buffer( pipeline_procedure* procedure, const void* key, size_t data_size )
{
    exchange_data_desc* desc = NULL;
    pipeline_procedure_output* output = hash_map_get( &procedure->output, key );
    if ( !pipeline_procedure_task_is_running( procedure->procedure_task ) )
    {
        return NULL;
    }
    pipeline_procedure_output_set_data_size( output, data_size );
    desc = pipeline_procedure_output_get_data( output );
    ASSERT( NULL != desc );
    switch ( desc->type )
    {
        case exchange_data_custom:
            return desc->data;
        case exchange_data_shutdown:
        default:
            return NULL;
    }
}

void pipeline_procedure_commit_output_buffer( pipeline_procedure* procedure, const void* key )
{
    pipeline_procedure_output* output = hash_map_get( &procedure->output, key );
    pipeline_procedure_output_passon( output );
}

