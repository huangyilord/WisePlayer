#include "pipeline_procedure_structures.h"

#include "common_debug.h"
#include "atomic.h"
#include "thread.h"
#include "pipeline_procedure.h"

static exchange_data_desc shut_down_command = { 0, NULL, exchange_data_shutdown };

boolean pipeline_connection_initialize( pipeline_connection* connection, uint32 max_data_num, memory_pool* pool )
{
    exchange_data_desc desc = { 0 };
    exchange_data_desc* pdesc = NULL;
    if ( !list_initialize( &connection->exchange_data, sizeof(exchange_data_desc), pool ) )
    {
        return FALSE;
    }
    for ( uint32 i = 0; i < max_data_num; ++i )
    {
        if ( !list_push_back( &connection->exchange_data, &desc ) )
        {
            list_destroy( &connection->exchange_data );
            return FALSE;
        }
        pdesc = list_back( &connection->exchange_data );
        pdesc->type = exchange_data_custom;
    }
    return TRUE;
}

void pipeline_connection_destroy( pipeline_connection* connection )
{
    exchange_data_desc* desc = NULL;
    list_iterator iter;
    pipeline_connection_disconnect( connection );
    LIST_ITERATOR_START( iter, &connection->exchange_data );
    while ( LIST_ITERATOR_IS_VALID( iter ) )
    {
        desc = LIST_ITERATOR_GET(iter);
        if ( NULL != desc->data )
        {
            free( desc->data );
        }
        LIST_ITERATOR_NEXT(iter);
    }
    list_destroy( &connection->exchange_data );
}

boolean pipeline_connection_connect( pipeline_connection* connection, pipeline_procedure_output* output, pipeline_procedure_input* input )
{
    if ( !pipeline_procedure_output_connect( output, connection ) )
    {
        return FALSE;
    }
    connection->source = output;
    if ( !pipeline_procedure_input_connect( input, connection ) )
    {
        pipeline_procedure_output_disconnect( output );
        return FALSE;
    }
    connection->target = input;
    return TRUE;
}

void pipeline_connection_disconnect( pipeline_connection* connection )
{
    if ( NULL != connection->source )
    {
        pipeline_procedure_output_disconnect( connection->source );
        connection->source = NULL;
    }
    if ( NULL != connection->target )
    {
        pipeline_procedure_input_disconnect( connection->target );
        connection->target = NULL;
    }
}

boolean pipeline_procedure_input_initialize( pipeline_procedure_input* input, pipeline_procedure* procedure, memory_pool* pool )
{
    if ( !list_initialize( &input->task_list, sizeof(void*), pool ) )
    {
        return FALSE;
    }
    input->new_input_event = synchronize_event_group_add_element( procedure->input_event );
    if ( NULL == input->new_input_event )
    {
        list_destroy( &input->task_list );
        return FALSE;
    }
    input->connection = NULL;
    input->procedure = procedure;
    input->incoming_task = &input->task_list;
    return TRUE;
}

void pipeline_procedure_input_destroy( pipeline_procedure_input* input )
{
    synchronize_event_group_remove_element( input->procedure->input_event, input->new_input_event );
    list_destroy( &input->task_list );
    memset( input, 0, sizeof(pipeline_procedure_input) );
}

boolean pipeline_procedure_input_connect( pipeline_procedure_input* input, pipeline_connection* connection )
{
    input->connection = connection;
    return TRUE;
}

void pipeline_procedure_input_disconnect( pipeline_procedure_input* input )
{
    list_destroy( &input->task_list );
    input->connection = NULL;
    input->incoming_task = NULL;
}

boolean pipeline_procedure_input_has_data( pipeline_procedure_input* input )
{
    boolean ret = FALSE;
    list* incomming_data = NULL;
    while ( NULL == incomming_data )
    {
        incomming_data = atomic_exchange_ptr( (void* volatile*)&input->incoming_task, incomming_data );
    }
    ret = !LIST_IS_EMPTY(incomming_data);
    atomic_exchange_ptr( (void* volatile*)&input->incoming_task, incomming_data );
    return ret;
}

exchange_data_desc* pipeline_procedure_input_try_get_data( pipeline_procedure_input* input )
{
    exchange_data_desc* desc = NULL;
    list* incomming_data = NULL;
    while ( NULL == incomming_data )
    {
        incomming_data = atomic_exchange_ptr( (void* volatile*)&input->incoming_task, incomming_data );
    }
    if ( !LIST_IS_EMPTY(incomming_data) )
    {
        desc = *(exchange_data_desc**)list_front( incomming_data );
    }
    atomic_exchange_ptr( (void* volatile*)&input->incoming_task, incomming_data );
    return desc;
}

exchange_data_desc* pipeline_procedure_input_get_data( pipeline_procedure_input* input )
{
    exchange_data_desc* desc = NULL;
    list* incomming_data = NULL;
    while ( NULL == incomming_data )
    {
        incomming_data = atomic_exchange_ptr( (void* volatile*)&input->incoming_task, incomming_data );
    }
    while ( LIST_IS_EMPTY(incomming_data) )
    {
        input->incoming_task = incomming_data;
        incomming_data = NULL;
        synchronize_event_group_wait_for_element( input->new_input_event );
        while ( NULL == incomming_data )
        {
            incomming_data = atomic_exchange_ptr( (void* volatile*)&input->incoming_task, incomming_data );
        }
    }
    desc = *(exchange_data_desc**)list_front( incomming_data );
    atomic_exchange_ptr( (void* volatile*)&input->incoming_task, incomming_data );
    return desc;
}

void pipeline_procedure_input_reset( pipeline_procedure_input* input )
{
    list* exchange_data = NULL;
    if ( NULL != input->connection )
    {
        while ( NULL == exchange_data )
        {
            exchange_data = atomic_exchange_ptr( (void* volatile*)&input->incoming_task, exchange_data );
        }
        list_clear( exchange_data );
        synchronize_event_group_reset_element( input->new_input_event );
        atomic_exchange_ptr( (void* volatile*)&input->incoming_task, exchange_data );
    }
}

void pipeline_procedure_input_moveon( pipeline_procedure_input* input )
{
    exchange_data_desc* desc = NULL;
    list* exchange_data = NULL;
    while ( NULL == exchange_data )
    {
        exchange_data = atomic_exchange_ptr( (void* volatile*)&input->incoming_task, exchange_data );
    }
    ASSERT( !LIST_IS_EMPTY(exchange_data) );
    desc = *(exchange_data_desc**)list_front( exchange_data );
    ASSERT( desc->type == exchange_data_custom );
    list_pop_front( exchange_data );
    input->incoming_task = exchange_data;

    ASSERT( NULL != input->connection );
    exchange_data = NULL;
    while ( NULL == exchange_data )
    {
        exchange_data = atomic_exchange_ptr( (void* volatile*)&input->connection->source->outgoing_task, exchange_data );
    }
    ASSERT( NULL == input->connection->source->outgoing_task );
    VERIFY( list_push_back( exchange_data, &desc ) );
    atomic_exchange_ptr( (void* volatile*)&input->connection->source->outgoing_task, exchange_data );
    synchronize_set_event( input->connection->source->recycle_data_event );
}

void pipeline_procedure_input_shutdown( pipeline_procedure_input* input )
{
    exchange_data_desc* command = &shut_down_command;
    list* exchange_data = NULL;
    if ( NULL != input->connection )
    {
        while ( NULL == exchange_data )
        {
            exchange_data = atomic_exchange_ptr( (void* volatile*)&input->incoming_task, exchange_data );
        }
        VERIFY(list_push_back( exchange_data, &command ));
        atomic_exchange_ptr( (void* volatile*)&input->incoming_task, exchange_data );
        synchronize_event_group_set_element( input->new_input_event );
    }
}

boolean pipeline_procedure_output_initialize( pipeline_procedure_output* output, pipeline_procedure* procedure, memory_pool* pool )
{
    if ( !list_initialize( &output->task_list, sizeof(void*), pool ) )
    {
        return FALSE;
    }
    output->recycle_data_event = synchronize_create_event();
    if ( NULL == output->recycle_data_event )
    {
        list_destroy( &output->task_list );
        return FALSE;
    }
    output->connection = NULL;
    output->procedure = procedure;
    output->outgoing_task = &output->task_list;
    output->data_size = 0;
    return TRUE;
}

void pipeline_procedure_output_set_data_size( pipeline_procedure_output* output, size_t data_size )
{
    output->data_size = data_size;
}

void pipeline_procedure_output_destroy( pipeline_procedure_output* output )
{
    list_destroy( &output->task_list );
    memset( output, 0, sizeof(pipeline_procedure_output) );
}

boolean pipeline_procedure_output_connect( pipeline_procedure_output* output, pipeline_connection* connection )
{
    exchange_data_desc* desc = NULL;
    list_iterator iter;
    output->connection = connection;
    LIST_ITERATOR_START( iter, &connection->exchange_data );
    while ( LIST_ITERATOR_IS_VALID( iter ) )
    {
        desc = LIST_ITERATOR_GET(iter);
        if ( !list_push_back( &output->task_list, &desc ) )
        {
            list_clear( &output->task_list );
            return FALSE;
        }
        LIST_ITERATOR_NEXT(iter);
    }
    return TRUE;
}

void pipeline_procedure_output_disconnect( pipeline_procedure_output* output )
{
    list_destroy( &output->task_list );
    output->connection = NULL;
    output->outgoing_task = NULL;
}

boolean pipeline_procedure_output_has_data( pipeline_procedure_output* output )
{
    boolean ret = FALSE;
    list* outgoing_data = NULL;
    while ( NULL == outgoing_data )
    {
        outgoing_data = atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, outgoing_data );
    }
    ret = !LIST_IS_EMPTY(outgoing_data);
    atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, outgoing_data );
    return ret;
}

exchange_data_desc* pipeline_procedure_output_try_get_data( pipeline_procedure_output* output )
{
    exchange_data_desc* desc = NULL;
    list* outgoing_data = NULL;
    while ( NULL == outgoing_data )
    {
        outgoing_data = atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, outgoing_data );
    }
    if ( !LIST_IS_EMPTY(outgoing_data) )
    {
        desc = *(exchange_data_desc**)list_front( outgoing_data );
        if ( desc->type == exchange_data_custom )
        {
            if ( desc->size != output->data_size )
            {
                if ( NULL != desc->data )
                {
                    free( desc->data );
                }
                desc->data = malloc( output->data_size );
                desc->size = output->data_size;
            }
        }
    }
    atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, outgoing_data );
    return desc;
}

exchange_data_desc* pipeline_procedure_output_get_data( pipeline_procedure_output* output )
{
    exchange_data_desc* desc = NULL;
    list* outgoing_data = NULL;
    while ( NULL == outgoing_data )
    {
        outgoing_data = atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, outgoing_data );
    }
    while ( LIST_IS_EMPTY(outgoing_data) )
    {
        output->outgoing_task = outgoing_data;
        outgoing_data = NULL;
        synchronize_wait_for_event( output->recycle_data_event );
        while ( NULL == outgoing_data )
        {
            outgoing_data = atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, outgoing_data );
        }
    }
    desc = *(exchange_data_desc**)list_front( outgoing_data );
    if ( desc->type == exchange_data_custom )
    {
        if ( desc->size != output->data_size )
        {
            if ( NULL != desc->data )
            {
                free( desc->data );
            }
            desc->data = malloc( output->data_size );
            desc->size = output->data_size;
        }
    }
    atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, outgoing_data );
    return desc;
}

void pipeline_procedure_output_reset( pipeline_procedure_output* output )
{
    exchange_data_desc* desc = NULL;
    list_iterator iter;
    list* exchange_data = NULL;
    if ( NULL != output->connection )
    {
        while ( NULL == exchange_data )
        {
            exchange_data = atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, exchange_data );
        }
        list_clear( exchange_data );
        LIST_ITERATOR_START( iter, &output->connection->exchange_data );
        while ( LIST_ITERATOR_IS_VALID( iter ) )
        {
            desc = LIST_ITERATOR_GET(iter);
            list_push_back( exchange_data, &desc );
            LIST_ITERATOR_NEXT(iter);
        }
        synchronize_reset_event( output->recycle_data_event );
        atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, exchange_data );
    }
}

void pipeline_procedure_output_passon( pipeline_procedure_output* output )
{
    exchange_data_desc* desc = NULL;
    pipeline_procedure_input* input = output->connection->target;
    list* exchange_data = NULL;
    while ( NULL == exchange_data )
    {
        exchange_data = atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, exchange_data );
    }
    ASSERT( !LIST_IS_EMPTY(exchange_data) );
    desc = *(exchange_data_desc**)list_front( exchange_data );
    ASSERT( desc->type == exchange_data_custom );
    list_pop_front( exchange_data );
    atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, exchange_data );

    ASSERT( NULL != output->connection );
    exchange_data = NULL;
    while ( NULL == exchange_data )
    {
        exchange_data = atomic_exchange_ptr( (void* volatile*)&input->incoming_task, exchange_data );
    }
    ASSERT( NULL == input->incoming_task );
    VERIFY( list_push_back( exchange_data, &desc ) );
    atomic_exchange_ptr( (void* volatile*)&input->incoming_task, exchange_data );
    synchronize_event_group_set_element( input->new_input_event );
}

void pipeline_procedure_output_shutdown( pipeline_procedure_output* output )
{
    exchange_data_desc* command = &shut_down_command;
    list* exchange_data = NULL;
    if ( NULL != output->connection )
    {
        while ( NULL == exchange_data )
        {
            exchange_data = atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, exchange_data );
        }
        VERIFY(list_push_back( exchange_data, &command ));
        atomic_exchange_ptr( (void* volatile*)&output->outgoing_task, exchange_data );
        synchronize_set_event( output->recycle_data_event );
    }
}

