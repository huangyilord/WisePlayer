//
//  pipeline_builder.c
//  task
//
//  Created by Yi Huang on 2/5/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "pipeline_builder.h"
#include "pipeline_procedure_structures.h"
#include "pipeline_builder_structures.h"
#include "graph_builder.h"

#include "common_debug.h"

#define BLOCK_SIZE 4096

typedef struct pipeline_builder
{
    size_t                          key_size;
    memory_pool                     mempool;
    hash_map                        procedures;
    uint32                          connection_cache_data_num;
    graph_builder                   procedure_input_graph;
    graph_builder                   procedure_output_graph;
} pipeline_builder;

typedef struct build_pipeline_context
{
    pipeline_builder*               builder;
    graph_explorer*                 explorer;
} build_pipeline_context;

static boolean pipeline_procedure_desc_initialize( pipeline_procedure_desc* desc, size_t key_size, memory_pool* pool );
static void pipeline_procedure_desc_destroy( pipeline_procedure_desc* desc );

static boolean get_connected_nodes( void* graph_context, const void* from_node, hash_set* connections );

static boolean create_new_node_with_output( pipeline_builder_node* node, pipeline_builder_node* from_node, pipeline_procedure_desc* new_procedure, const void* output_key, size_t key_size, memory_pool* pool );

static boolean create_new_node_with_input( pipeline_builder_node* node, pipeline_builder_node* from_node, pipeline_procedure_desc* new_procedure, const void* input_key, size_t key_size, memory_pool* pool );

static boolean build_pipeline( pipeline_builder* builder, graph_explorer* explorer, pipeline* p, pipeline_builder_node_key* destination_key, memory_pool* pool );

static boolean serialize_pipeline( pipeline* p, memory_pool* pool );

pipeline_builder* pipeline_builder_create( size_t key_size )
{
    pipeline_builder* builder = malloc( sizeof(pipeline_builder) );
    if ( NULL == builder )
    {
        return NULL;
    }
    memory_pool_initialize( &builder->mempool, BLOCK_SIZE, 4 );
    if ( !hash_map_initialize( &builder->procedures, key_size, sizeof(pipeline_procedure_desc), &builder->mempool ) )
    {
        return NULL;
    }
    if ( !graph_builder_initialize( &builder->procedure_input_graph, key_size, &builder->mempool ) )
    {
        hash_map_destroy( &builder->procedures );
        return FALSE;
    }
    if ( !graph_builder_initialize( &builder->procedure_output_graph, key_size, &builder->mempool ) )
    {
        graph_builder_destroy( &builder->procedure_input_graph );
        hash_map_destroy( &builder->procedures );
        return FALSE;
    }
    builder->key_size = key_size;
    builder->connection_cache_data_num = 5;
    return builder;
}

void pipeline_builder_destroy( pipeline_builder* builder )
{
    pipeline_builder_clear_procedure_graph( builder );
    pipeline_builder_clear_procedure( builder );

    graph_builder_destroy( &builder->procedure_output_graph );
    graph_builder_destroy( &builder->procedure_input_graph );
    hash_map_destroy( &builder->procedures );
    memory_pool_destroy( &builder->mempool );
    free( builder );
}

pipeline_procedure_desc* pipeline_builder_add_procedure( pipeline_builder* builder, const void* key )
{
    pipeline_procedure_desc* result = NULL;
    pipeline_procedure_desc desc = { 0 };
    if ( !hash_map_insert( &builder->procedures, key, &desc ) )
    {
        return NULL;
    }
    result = hash_map_get( &builder->procedures, key );
    if ( !pipeline_procedure_desc_initialize( result, builder->key_size, &builder->mempool ) )
    {
        hash_map_remove( &builder->procedures, key );
        return NULL;
    }
    return result;
}

void pipeline_builder_remove_procedure( pipeline_builder* builder, const void* key )
{
    pipeline_procedure_desc* result = hash_map_get( &builder->procedures, key );
    if ( NULL != result )
    {
        pipeline_procedure_desc_destroy( result );
        hash_map_remove( &builder->procedures, key );
    }
}

void pipeline_builder_clear_procedure( pipeline_builder* builder )
{
    pipeline_procedure_desc* procedure_desc = NULL;
    hash_map_iterator procedure_iter;
    HASH_MAP_ITERATOR_START( procedure_iter, &builder->procedures );
    while ( HASH_MAP_ITERATOR_IS_VALID( procedure_iter ) )
    {
        procedure_desc = hash_map_get( &builder->procedures, HASH_MAP_ITERATOR_GET( procedure_iter ) );
        pipeline_procedure_desc_destroy( procedure_desc );
        HASH_MAP_ITERATOR_NEXT( procedure_iter );
    }
    hash_map_clear( &builder->procedures );
}

boolean pipeline_builder_rebuild_procedure_graph( pipeline_builder* builder )
{
    pipeline_procedure_desc* procedure_desc = NULL;
    const void* procedure_key = NULL;
    const void* input_key = NULL;
    const void* output_key = NULL;
    hash_map_iterator procedure_iter;
    hash_set_iterator input_iter;
    hash_set_iterator output_iter;
    pipeline_builder_clear_procedure_graph( builder );
    HASH_MAP_ITERATOR_START( procedure_iter, &builder->procedures );
    while ( HASH_MAP_ITERATOR_IS_VALID( procedure_iter ) )
    {
        procedure_key = HASH_MAP_ITERATOR_GET( procedure_iter );
        procedure_desc = (pipeline_procedure_desc*)hash_map_get( &builder->procedures, procedure_key );
        HASH_SET_ITERATOR_START( input_iter, &procedure_desc->input_keys );
        while ( HASH_SET_ITERATOR_IS_VALID( input_iter ) )
        {
            input_key = HASH_MAP_ITERATOR_GET( input_iter );
            if ( !graph_builder_add_edge( &builder->procedure_input_graph, input_key, procedure_key ) )
            {
                pipeline_builder_clear_procedure_graph( builder );
                return FALSE;
            }
            HASH_SET_ITERATOR_NEXT( input_iter );
        }
        HASH_SET_ITERATOR_START( output_iter, &procedure_desc->output_keys );
        while ( HASH_SET_ITERATOR_IS_VALID( output_iter ) )
        {
            output_key = HASH_SET_ITERATOR_GET( output_iter );
            if ( !graph_builder_add_edge( &builder->procedure_output_graph, output_key, procedure_key ) )
            {
                pipeline_builder_clear_procedure_graph( builder );
                return FALSE;
            }
            HASH_SET_ITERATOR_NEXT( output_iter );
        }
        HASH_MAP_ITERATOR_NEXT( procedure_iter );
    }
    if ( !graph_builder_rebuild( &builder->procedure_input_graph ) )
    {
        return FALSE;
    }
    if ( !graph_builder_rebuild( &builder->procedure_output_graph ) )
    {
        return FALSE;
    }
    return TRUE;
}

void pipeline_builder_clear_procedure_graph( pipeline_builder* builder )
{
    graph_builder_clear_edge( &builder->procedure_input_graph );
    graph_builder_clear_edge( &builder->procedure_output_graph );
}

boolean pipeline_builder_build_pipeline( pipeline_builder* builder, pipeline* p )
{
    boolean result = TRUE;
    graph_explorer_callbacks callbacks = { pipeline_builder_node_compare, pipeline_builder_node_key_equal,pipeline_builder_node_key_hash_code };
    graph_explorer explorer;
    build_pipeline_context context = { builder, &explorer };
    graph pipeline_buider_graph = { &context, get_connected_nodes, pipeline_builder_node_get_key };
    hash_map_iterator iter;
    hash_set_iterator src_iter;
    pipeline_procedure* src;
    pipeline_builder_node* node;
    pipeline_builder_node_key target_node_key;
    pipeline_builder_node target_node;
    memory_pool mempool;
    memory_pool_initialize( &mempool, 1024, 8 );

    // initialize target key
    if ( !pipeline_builder_node_initialize( &target_node, builder->key_size, &mempool ) )
    {
        memory_pool_destroy( &mempool );
        return FALSE;
    }
    pipeline_builder_node_get_key( &context, &target_node, &target_node_key );
    do
    {
        if ( !graph_explorer_initialize( &explorer, sizeof(pipeline_builder_node_key), sizeof(pipeline_builder_node), &mempool ) )
        {
            result = FALSE;
            break;
        }
        do
        {
            // initialize start key
            node = explore_node_map_alloc_node( &explorer.explored_nodes );
            if ( NULL == node )
            {
                result = FALSE;
                break;
            }
            if ( !pipeline_builder_node_initialize( node, builder->key_size, &mempool ) )
            {
                explore_node_map_free_node( &explorer.explored_nodes, node );
                result = FALSE;
                break;
            }
            HASH_SET_ITERATOR_START( src_iter, &p->src );
            while ( HASH_SET_ITERATOR_IS_VALID( src_iter ) && result )
            {
                src = *(pipeline_procedure**)HASH_SET_ITERATOR_GET( src_iter );
                HASH_MAP_ITERATOR_START( iter, &src->output );
                while ( HASH_MAP_ITERATOR_IS_VALID(iter) )
                {
                    if ( !hash_set_insert( &node->unlinked_output, HASH_MAP_ITERATOR_GET(iter) ) )
                    {
                        result = FALSE;
                        break;
                    }
                    HASH_MAP_ITERATOR_NEXT(iter);
                }
                HASH_SET_ITERATOR_NEXT( src_iter );
            }
            if ( !result )
            {
                break;
            }
            if ( !graph_explorer_start_explore( &explorer, &pipeline_buider_graph, node, callbacks ) )
            {
                pipeline_builder_node_destroy( node );
                explore_node_map_free_node( &explorer.explored_nodes, node );
                result = FALSE;
                break;
            }
            while ( (result = graph_explorer_step( &explorer )) )
            {
                node = explore_node_map_get_node_with_key( &explorer.explored_nodes, &target_node_key );
                if ( NULL != node )
                {
                    // build pipeline
                    result = build_pipeline( builder, &explorer, p, &target_node_key, &mempool );
                    break;
                }
            }
            HASH_MAP_ITERATOR_START( iter, &explorer.explored_nodes.nodes );
            while ( HASH_MAP_ITERATOR_IS_VALID( iter ) )
            {
                node = explore_node_map_get_node_with_key( &explorer.explored_nodes, HASH_MAP_ITERATOR_GET(iter) );
                pipeline_builder_node_destroy( node );
                HASH_MAP_ITERATOR_NEXT( iter );
            }
            graph_explorer_finish_explore( &explorer );
        } while ( 0 );
        graph_explorer_destroy( &explorer );
    } while ( 0 );

    pipeline_builder_node_destroy( &target_node );
    memory_pool_destroy( &mempool );
    return result;
}

boolean pipeline_procedure_desc_initialize( pipeline_procedure_desc* desc, size_t key_size, memory_pool* pool )
{
    if ( !hash_set_initialize( &desc->input_keys, key_size, pool ) )
    {
        return FALSE;
    }
    if ( !hash_set_initialize( &desc->output_keys, key_size, pool ) )
    {
        hash_set_destroy( &desc->input_keys );
        return FALSE;
    }
    return TRUE;
}

void pipeline_procedure_desc_destroy( pipeline_procedure_desc* desc )
{
    hash_set_destroy( &desc->output_keys );
    hash_set_destroy( &desc->input_keys );
}

boolean get_connected_nodes( void* context, const void* from_node, hash_set* connected_nodes )
{
    build_pipeline_context* build_context = (build_pipeline_context*)context;
    pipeline_builder* builder = build_context->builder;
    pipeline_builder_node_key* key = (pipeline_builder_node_key*)from_node;
    hash_set_iterator iter;
    hash_set_iterator procedure_iter;
    const void* procedure_key = NULL;
    const void* pin_key = NULL;
    pipeline_procedure_desc* desc = NULL;
    pipeline_builder_node* node = explore_node_map_get_node_with_key( &build_context->explorer->explored_nodes, key );
    pipeline_builder_node* original_node = NULL;
    pipeline_builder_node* new_node = NULL;
    pipeline_builder_node_key new_node_key;
    hash_set procedures;
    if ( !hash_set_initialize( &procedures, builder->key_size, build_context->explorer->mempool ) )
    {
        return FALSE;
    }
    ASSERT( NULL != node );
    if ( HASH_SET_IS_EMPTY( &node->unlinked_input ) )
    {
        HASH_SET_ITERATOR_START( iter, &node->unlinked_output );
        while ( HASH_SET_ITERATOR_IS_VALID( iter ) )
        {
            pin_key = HASH_SET_ITERATOR_GET(iter);
            if ( !graph_builder_get_connected_nodes( &builder->procedure_input_graph, pin_key, &procedures ) )
            {
                hash_set_destroy( &procedures );
                return FALSE;
            }
            HASH_SET_ITERATOR_START( procedure_iter, &procedures );
            while ( HASH_SET_ITERATOR_IS_VALID( procedure_iter ) )
            {
                procedure_key = HASH_SET_ITERATOR_GET( procedure_iter );
                desc = hash_map_get( &builder->procedures, procedure_key );
                ASSERT( NULL != desc );
                ASSERT( hash_set_exist( &desc->input_keys, pin_key ) );
                new_node = explore_node_map_alloc_node( &build_context->explorer->explored_nodes );
                if ( NULL == new_node )
                {
                    hash_set_destroy( &procedures );
                    return FALSE;
                }
                if ( !create_new_node_with_output( new_node, node, desc, pin_key, builder->key_size, build_context->explorer->mempool ) )
                {
                    explore_node_map_free_node( &build_context->explorer->explored_nodes, new_node );
                    hash_set_destroy( &procedures );
                    return FALSE;
                }
                pipeline_builder_node_get_key( context, new_node, &new_node_key );
                original_node = explore_node_map_get_node_with_key( &build_context->explorer->explored_nodes, &new_node_key );
                if ( NULL != original_node )
                {
                    if ( original_node->cost > new_node->cost )
                    {
                        pipeline_builder_node_destroy( original_node );
                        explore_node_map_free_node( &build_context->explorer->explored_nodes, original_node );
                        original_node = NULL;
                    }
                }
                if ( NULL == original_node )
                {
                    if ( !explore_node_map_insert_node_with_key( &build_context->explorer->explored_nodes, &new_node_key, new_node ) )
                    {
                        pipeline_builder_node_destroy( new_node );
                        explore_node_map_free_node( &build_context->explorer->explored_nodes, new_node );
                        hash_set_destroy( &procedures );
                        return FALSE;
                    }
                    if ( !hash_set_insert( connected_nodes, &new_node_key ) )
                    {
                        hash_set_destroy( &procedures );
                        return FALSE;
                    }
                }
                HASH_SET_ITERATOR_NEXT( procedure_iter );
            }
            HASH_SET_ITERATOR_NEXT( iter );
        }
    }
    else
    {
        HASH_SET_ITERATOR_START( iter, &node->unlinked_input );
        while ( HASH_SET_ITERATOR_IS_VALID( iter ) )
        {
            pin_key = HASH_SET_ITERATOR_GET(iter);
            if ( !graph_builder_get_connected_nodes( &builder->procedure_output_graph, pin_key, &procedures ) )
            {
                hash_set_destroy( &procedures );
                return FALSE;
            }
            HASH_SET_ITERATOR_START( procedure_iter, &procedures );
            while ( HASH_SET_ITERATOR_IS_VALID( procedure_iter ) )
            {
                procedure_key = HASH_SET_ITERATOR_GET( procedure_iter );
                desc = hash_map_get( &builder->procedures, procedure_key );
                ASSERT( NULL != desc );
                ASSERT( hash_set_exist( &desc->output_keys, pin_key ) );
                new_node = explore_node_map_alloc_node( &build_context->explorer->explored_nodes );
                if ( NULL == new_node )
                {
                    hash_set_destroy( &procedures );
                    return FALSE;
                }
                if ( !create_new_node_with_input( new_node, node, desc, pin_key, builder->key_size, build_context->explorer->mempool ) )
                {
                    explore_node_map_free_node( &build_context->explorer->explored_nodes, new_node );
                    hash_set_destroy( &procedures );
                    return FALSE;
                }
                pipeline_builder_node_get_key( context, new_node, &new_node_key );
                original_node = explore_node_map_get_node_with_key( &build_context->explorer->explored_nodes, &new_node_key );
                if ( NULL != original_node )
                {
                    if ( original_node->cost > new_node->cost )
                    {
                        pipeline_builder_node_destroy( original_node );
                        explore_node_map_free_node( &build_context->explorer->explored_nodes, original_node );
                        original_node = NULL;
                    }
                }
                if ( NULL == original_node )
                {
                    if ( !explore_node_map_insert_node_with_key( &build_context->explorer->explored_nodes, &new_node_key, new_node ) )
                    {
                        pipeline_builder_node_destroy( new_node );
                        explore_node_map_free_node( &build_context->explorer->explored_nodes, new_node );
                        hash_set_destroy( &procedures );
                        return FALSE;
                    }
                    if ( !hash_set_insert( connected_nodes, &new_node_key ) )
                    {
                        hash_set_destroy( &procedures );
                        return FALSE;
                    }
                }
                HASH_SET_ITERATOR_NEXT( procedure_iter );
            }
            HASH_SET_ITERATOR_NEXT( iter );
        }
    }
    hash_set_destroy( &procedures );
    return TRUE;
}

boolean create_new_node_with_output( pipeline_builder_node* node, pipeline_builder_node* from_node, pipeline_procedure_desc* new_procedure, const void* output_key, size_t key_size, memory_pool* pool )
{
    hash_set_iterator iter;
    const void* pin_key = NULL;
    if ( !pipeline_builder_node_initialize( node, key_size, pool ) )
    {
        return FALSE;
    }
    if ( !hash_set_copy( &node->unlinked_input, &from_node->unlinked_input ) )
    {
        pipeline_builder_node_destroy( node );
        return FALSE;
    }
    if ( !hash_set_copy( &node->unlinked_output, &from_node->unlinked_output ) )
    {
        pipeline_builder_node_destroy( node );
        return FALSE;
    }
    HASH_SET_ITERATOR_START( iter, &new_procedure->input_keys );
    while ( HASH_SET_ITERATOR_IS_VALID( iter ) )
    {
        pin_key = HASH_SET_ITERATOR_GET(iter);
        if ( hash_set_exist( &node->unlinked_output, pin_key ) )
        {
            hash_set_remove( &node->unlinked_output, pin_key );
        }
        else
        {
            hash_set_insert( &node->unlinked_input, pin_key );
        }
        HASH_SET_ITERATOR_NEXT( iter );
    }
    HASH_SET_ITERATOR_START( iter, &new_procedure->output_keys );
    while ( HASH_SET_ITERATOR_IS_VALID( iter ) )
    {
        pin_key = HASH_SET_ITERATOR_GET(iter);
        if ( hash_set_exist( &node->unlinked_input, pin_key ) )
        {
            hash_set_remove( &node->unlinked_input, pin_key );
        }
        else
        {
            hash_set_insert( &node->unlinked_output, pin_key );
        }
        HASH_SET_ITERATOR_NEXT( iter );
    }
    node->new_procedure = new_procedure;
    node->previous_node = from_node;
    node->cost = from_node->cost + 1;
    return TRUE;
}

boolean create_new_node_with_input( pipeline_builder_node* node, pipeline_builder_node* from_node, pipeline_procedure_desc* new_procedure, const void* input_key, size_t key_size, memory_pool* pool )
{
    hash_set_iterator iter;
    const void* pin_key = NULL;
    if ( !pipeline_builder_node_initialize( node, key_size, pool ) )
    {
        return FALSE;
    }
    if ( !hash_set_copy( &node->unlinked_input, &from_node->unlinked_input ) )
    {
        pipeline_builder_node_destroy( node );
        return FALSE;
    }
    if ( !hash_set_copy( &node->unlinked_output, &from_node->unlinked_output ) )
    {
        pipeline_builder_node_destroy( node );
        return FALSE;
    }
    HASH_SET_ITERATOR_START( iter, &new_procedure->output_keys );
    while ( HASH_SET_ITERATOR_IS_VALID( iter ) )
    {
        pin_key = HASH_SET_ITERATOR_GET(iter);
        if ( hash_set_exist( &node->unlinked_input, pin_key ) )
        {
            hash_set_remove( &node->unlinked_input, pin_key );
        }
        else
        {
            hash_set_insert( &node->unlinked_output, pin_key );
        }
        HASH_SET_ITERATOR_NEXT( iter );
    }
    HASH_SET_ITERATOR_START( iter, &new_procedure->input_keys );
    while ( HASH_SET_ITERATOR_IS_VALID( iter ) )
    {
        pin_key = HASH_SET_ITERATOR_GET(iter);
        if ( hash_set_exist( &node->unlinked_output, pin_key ) )
        {
            hash_set_remove( &node->unlinked_output, pin_key );
        }
        else
        {
            hash_set_insert( &node->unlinked_input, pin_key );
        }
        HASH_SET_ITERATOR_NEXT( iter );
    }
    node->new_procedure = new_procedure;
    node->previous_node = from_node;
    node->cost = from_node->cost + 1;
    return TRUE;
}

boolean build_pipeline( pipeline_builder* builder, graph_explorer* explorer, pipeline* p, pipeline_builder_node_key* destination_key, memory_pool* pool )
{
    boolean result = TRUE;
    list l;
    list_iterator iter;
    hash_map unlinked_output;
    hash_map unlinked_input;
    hash_set_iterator iter_src;
    hash_map_iterator iter_pin;
    hash_map_iterator iter_proc;
    array_list_iterator iter_connection;
    pipeline_builder_node* node = NULL;
    pipeline_procedure* procedure = NULL;
    pipeline_procedure_desc* procedure_desc = NULL;
    pipeline_procedure_input* input = NULL;
    pipeline_procedure_output* output = NULL;
    pipeline_connection* connection = NULL;
    const void* key = NULL;
    void* value = NULL;
    // initialize
    if ( !list_initialize( &l, sizeof(pipeline_procedure_desc*), pool ) )
    {
        return FALSE;
    }
    if ( !hash_map_initialize( &unlinked_output, builder->key_size, sizeof(pipeline_procedure_output*), pool ) )
    {
        list_destroy( &l );
        return FALSE;
    }
    if ( !hash_map_initialize( &unlinked_input, builder->key_size, sizeof(pipeline_procedure_input*), pool ) )
    {
        hash_map_destroy( &unlinked_output );
        list_destroy( &l );
        return FALSE;
    }
    HASH_SET_ITERATOR_START( iter_src, &p->src );
    while ( HASH_SET_ITERATOR_IS_VALID(iter_src) )
    {
        procedure = *(pipeline_procedure**)HASH_SET_ITERATOR_GET( iter_src );
        HASH_MAP_ITERATOR_START( iter_pin, &procedure->output );
        while ( HASH_MAP_ITERATOR_IS_VALID( iter_pin ) )
        {
            key = HASH_MAP_ITERATOR_GET(iter_pin);
            output = hash_map_get( &procedure->output, key );
            if ( !hash_map_insert( &unlinked_output, key, &output ) )
            {
                hash_map_destroy( &unlinked_input );
                hash_map_destroy( &unlinked_output );
                list_destroy( &l );
                return FALSE;
            }
            HASH_MAP_ITERATOR_NEXT( iter_pin );
        }
        HASH_SET_ITERATOR_NEXT( iter_src );
    }

    // trace back to the original status
    node = explore_node_map_get_node_with_key( &explorer->explored_nodes, destination_key );
    while ( NULL != node && NULL != node->new_procedure )
    {
        if ( !list_push_front( &l, &node->new_procedure ) )
        {
            result = FALSE;
            break;
        }
        node = node->previous_node;
    }
    LIST_ITERATOR_START( iter, &l );
    while ( LIST_ITERATOR_IS_VALID(iter) && result )
    {
        procedure_desc = *((pipeline_procedure_desc**)LIST_ITERATOR_GET(iter));
        procedure = procedure_desc->create_function( procedure_desc->context );
        if ( NULL == procedure )
        {
            result = FALSE;
            break;
        }
        if ( !hash_map_insert( &p->procedure_desc_map, &procedure, &procedure_desc ) )
        {
            procedure_desc->destroy_function( procedure, procedure_desc->context );
            result = FALSE;
            break;
        }
        // connect procedure
        HASH_MAP_ITERATOR_START( iter_pin, &procedure->input );
        while ( HASH_MAP_ITERATOR_IS_VALID(iter_pin) && result )
        {
            key = HASH_MAP_ITERATOR_GET(iter_pin);
            input = hash_map_get( &procedure->input, key );
            value = hash_map_get( &unlinked_output, key );
            if ( NULL != value )
            {
                output = *(pipeline_procedure_output**)value;
                connection = array_list_push( &p->connections );
                if ( NULL == connection )
                {
                    result = FALSE;
                    break;
                }
                if ( !pipeline_connection_initialize( connection, builder->connection_cache_data_num, p->mempool ) )
                {
                    array_list_pop( &p->connections );
                    result = FALSE;
                    break;
                }
                if ( !pipeline_connection_connect( connection, output, input ) )
                {
                    pipeline_connection_destroy( connection );
                    array_list_pop( &p->connections );
                    result = FALSE;
                    break;
                }
                hash_map_remove( &unlinked_output, key );
            }
            else
            {
                if ( !hash_map_insert( &unlinked_input, key, &input ) )
                {
                    result = FALSE;
                    break;
                }
            }
            HASH_MAP_ITERATOR_NEXT(iter_pin);
        }
        HASH_MAP_ITERATOR_START( iter_pin, &procedure->output );
        while ( HASH_MAP_ITERATOR_IS_VALID(iter_pin) && result )
        {
            key = HASH_MAP_ITERATOR_GET(iter_pin);
            output = hash_map_get( &procedure->output, key );
            value = hash_map_get( &unlinked_input, key );
            if ( NULL != value )
            {
                input = *(pipeline_procedure_input**)value;
                connection = array_list_push( &p->connections );
                if ( NULL == connection )
                {
                    result = FALSE;
                    break;
                }
                if ( !pipeline_connection_initialize( connection, builder->connection_cache_data_num, p->mempool ) )
                {
                    array_list_pop( &p->connections );
                    result = FALSE;
                    break;
                }
                if ( !pipeline_connection_connect( connection, output, input ) )
                {
                    pipeline_connection_destroy( connection );
                    array_list_pop( &p->connections );
                    result = FALSE;
                    break;
                }
                hash_map_remove( &unlinked_input, key );
            }
            else
            {
                if ( !hash_map_insert( &unlinked_output, key, &output ) )
                {
                    result = FALSE;
                    break;
                }
            }
            HASH_MAP_ITERATOR_NEXT(iter_pin);
        }
        if ( !result )
        {
            procedure_desc->destroy_function( procedure, procedure_desc->context );
        }
        LIST_ITERATOR_NEXT(iter);
    }

    // serialize procedures
    if ( result )
    {
        ASSERT( HASH_MAP_IS_EMPTY(&unlinked_input) );
        ASSERT( HASH_MAP_IS_EMPTY(&unlinked_output) );
        if ( !serialize_pipeline( p, pool ) )
        {
            // destroy procedures
            ARRAY_LIST_ITERATOR_START( iter_connection, &p->connections );
            while ( ARRAY_LIST_ITERATOR_IS_VALID( iter_connection ) )
            {
                connection = (pipeline_connection*)ARRAY_LIST_ITERATOR_GET( iter_connection );
                pipeline_connection_destroy( connection );
                ARRAY_LIST_ITERATOR_NEXT( iter_connection );
            }
            array_list_clear( &p->connections );

            HASH_MAP_ITERATOR_START( iter_proc, &p->procedure_desc_map );
            while ( HASH_MAP_ITERATOR_IS_VALID( iter_proc ) )
            {
                procedure = *(pipeline_procedure**)HASH_MAP_ITERATOR_GET( iter_proc );
                procedure_desc = *(pipeline_procedure_desc**)hash_map_get( &p->procedure_desc_map, &procedure );
                procedure_desc->destroy_function( procedure, procedure_desc->context );
                HASH_MAP_ITERATOR_NEXT( iter_proc );
            }
        }
    }
    hash_map_destroy( &unlinked_input );
    hash_map_destroy( &unlinked_output );
    list_destroy( &l );
    return result;
}

boolean serialize_pipeline( pipeline* p, memory_pool* pool )
{
    boolean result = TRUE;
    hash_set procedures;
    hash_set_iterator procedure_iter;
    hash_map_iterator output_iter;
    pipeline_procedure* procedure;
    pipeline_procedure_output* output;
    list_clear( &p->procedures );
    if ( !hash_set_initialize( &procedures, sizeof(pipeline_procedure*), pool ) )
    {
        return FALSE;
    }
    do
    {
        HASH_SET_ITERATOR_START( procedure_iter, &p->src );
        while ( HASH_SET_ITERATOR_IS_VALID( procedure_iter ) )
        {
            procedure = *(pipeline_procedure**)HASH_SET_ITERATOR_GET( procedure_iter );
            if ( !hash_set_insert( &procedures, &procedure ) )
            {
                result = FALSE;
                break;
            }
            HASH_SET_ITERATOR_NEXT( procedure_iter );
        }

        HASH_SET_ITERATOR_START( procedure_iter, &procedures );
        while ( HASH_SET_ITERATOR_IS_VALID( procedure_iter ) && result )
        {
            procedure = *(pipeline_procedure**)HASH_SET_ITERATOR_GET( procedure_iter );
            if ( !list_push_back( &p->procedures, &procedure ) )
            {
                result = FALSE;
                break;
            }
            HASH_MAP_ITERATOR_START( output_iter, &procedure->output );
            while ( HASH_MAP_ITERATOR_IS_VALID( output_iter ) )
            {
                output = hash_map_get( &procedure->output, HASH_MAP_ITERATOR_GET( output_iter ) );
                if ( !hash_set_insert( &procedures, &output->connection->target->procedure ) )
                {
                    result = FALSE;
                    break;
                }
                HASH_MAP_ITERATOR_NEXT( output_iter );
            }
            HASH_SET_ITERATOR_NEXT( procedure_iter );
        }
    } while ( FALSE );
    if ( !result )
    {
        list_clear( &p->procedures );
    }
    hash_set_destroy( &procedures );
    return result;
}


