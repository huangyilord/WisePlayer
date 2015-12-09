#include "pipeline.h"
#include "pipeline_procedure_structures.h"
#include "pipeline_builder.h"
#include "common_debug.h"

boolean pipeline_initialize( pipeline* p, task_processor_thread_pool* processor_thread_pool, memory_pool* pool )
{
    if ( !list_initialize( &p->procedures, sizeof(pipeline_procedure*), pool ) )
    {
        return FALSE;
    }
    if ( !array_list_initialize( &p->connections, sizeof(pipeline_connection), pool ) )
    {
        list_destroy( &p->procedures );
        return FALSE;
    }
    if ( !hash_set_initialize( &p->src, sizeof(pipeline_procedure*), pool ) )
    {
        array_list_destroy( &p->connections );
        list_destroy( &p->procedures );
        return FALSE;
    }
    if ( !hash_map_initialize( &p->procedure_desc_map, sizeof(pipeline_procedure*), sizeof(pipeline_procedure_desc*), pool ) )
    {
        hash_set_destroy( &p->src );
        array_list_destroy( &p->connections );
        list_destroy( &p->procedures );
        return FALSE;
    }
    p->mempool = pool;
    p->processor_thread_pool = processor_thread_pool;
    return TRUE;
}

void pipeline_destroy( pipeline* p )
{
    pipeline_clear( p );
    hash_map_destroy( &p->procedure_desc_map );
    hash_set_destroy( &p->src );
    array_list_clear( &p->connections );
    list_destroy( &p->procedures );
    p->processor_thread_pool = NULL;
    p->mempool = NULL;
}

pipeline_procedure* pipeline_add_src( pipeline* p, pipeline_procedure_desc* desc )
{
    pipeline_procedure* procedure = desc->create_function( desc->context );
    if ( NULL == procedure )
    {
        return NULL;
    }
    if ( !hash_map_insert( &p->procedure_desc_map, &procedure, &desc ) )
    {
        desc->destroy_function( procedure, desc->context );
        return NULL;
    }
    if ( !hash_set_insert( &p->src, &procedure ) )
    {
        hash_map_remove( &p->procedure_desc_map, &procedure );
        desc->destroy_function( procedure, desc->context );
        return NULL;
    }
    return procedure;
}

void pipeline_clear( pipeline* p )
{
    pipeline_procedure* procedure = NULL;
    pipeline_procedure_desc* procedure_desc = NULL;
    pipeline_connection* connection = NULL;
    hash_map_iterator procedure_iter;
    array_list_iterator connection_iter;

    pipeline_stop( p );
    ARRAY_LIST_ITERATOR_START( connection_iter, &p->connections );
    while ( ARRAY_LIST_ITERATOR_IS_VALID( connection_iter ) )
    {
        connection = (pipeline_connection*)ARRAY_LIST_ITERATOR_GET( connection_iter );
        pipeline_connection_destroy( connection );
        ARRAY_LIST_ITERATOR_NEXT( connection_iter );
    }
    HASH_MAP_ITERATOR_START( procedure_iter, &p->procedure_desc_map );
    while ( HASH_MAP_ITERATOR_IS_VALID( procedure_iter ) )
    {
        procedure = *(pipeline_procedure**)HASH_MAP_ITERATOR_GET( procedure_iter );
        procedure_desc = *(pipeline_procedure_desc**)hash_map_get( &p->procedure_desc_map, &procedure );
        procedure_desc->destroy_function( procedure, procedure_desc->context );
        HASH_MAP_ITERATOR_NEXT( procedure_iter );
    }
    array_list_clear( &p->connections );
    list_clear( &p->procedures );
    hash_set_clear( &p->src );
    hash_map_clear( &p->procedure_desc_map );
}

void pipeline_stop( pipeline* p )
{
    pipeline_procedure* procedure = NULL;
    list_iterator procedure_iter;

    LIST_ITERATOR_START( procedure_iter, &p->procedures );
    while ( LIST_ITERATOR_IS_VALID( procedure_iter ) )
    {
        procedure = *(pipeline_procedure**)LIST_ITERATOR_GET( procedure_iter );
        pipeline_procedure_stop( procedure );
        LIST_ITERATOR_NEXT( procedure_iter );
    }
}

void pipeline_start( pipeline* p )
{
    pipeline_procedure* procedure = NULL;
    list_iterator procedure_iter;
    
    LIST_ITERATOR_START( procedure_iter, &p->procedures );
    while ( LIST_ITERATOR_IS_VALID( procedure_iter ) )
    {
        procedure = *(pipeline_procedure**)LIST_ITERATOR_GET( procedure_iter );
        pipeline_procedure_start( procedure );
        LIST_ITERATOR_NEXT( procedure_iter );
    }
}
