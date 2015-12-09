//
//  graph_builder.c
//  graph
//
//  Created by Yi Huang on 18/5/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "graph_builder.h"

#include "common_debug.h"

static boolean graph_builder_get_node_key( graph_builder* builder, const void* node, void* key );

static void graph_builder_clear_nodes( graph_builder* builder );

boolean graph_builder_initialize( graph_builder* builder, size_t node_size, memory_pool* pool )
{
    if ( !hash_map_initialize( &builder->node_map, node_size, sizeof(hash_set), pool ) )
    {
        return FALSE;
    }
    if ( !hash_set_initialize( &builder->edge_set, node_size << 1, pool ) )
    {
        hash_map_destroy( &builder->node_map );
        return FALSE;
    }
    builder->current_graph.context = builder;
    builder->current_graph.connected_nodes_function = (get_connected_nodes_function)graph_builder_get_connected_nodes;
    builder->current_graph.node_key_function = (get_node_key_function)graph_builder_get_node_key;
    return TRUE;
}

void graph_builder_destroy( graph_builder* builder )
{
    hash_set_destroy( &builder->edge_set );
    graph_builder_clear_nodes( builder );
    hash_map_destroy( &builder->node_map );
}

boolean graph_builder_add_edge( graph_builder* builder, const void* from_node, const void* to_node )
{
    size_t node_size = builder->node_map.values.key_size;
    void* edge = memory_pool_alloc( builder->edge_set.values.block_pool );
    if ( NULL == edge )
    {
        return FALSE;
    }
    memcpy( edge, from_node, node_size );
    memcpy( (byte*)edge + node_size, to_node, node_size );
    if ( !hash_set_insert( &builder->edge_set, edge ) )
    {
        memory_pool_free( builder->edge_set.values.block_pool, edge );
        return FALSE;
    }
    memory_pool_free( builder->edge_set.values.block_pool, edge );
    return TRUE;
}

boolean graph_builder_remove_edge( graph_builder* builder, const void* from_node, const void* to_node )
{
    size_t node_size = builder->node_map.values.key_size;
    void* edge = memory_pool_alloc( builder->edge_set.values.block_pool );
    if ( NULL == edge )
    {
        return FALSE;
    }
    memcpy( edge, from_node, node_size );
    memcpy( (byte*)edge + node_size, to_node, node_size );
    if ( !hash_set_exist( &builder->edge_set, edge ) )
    {
        memory_pool_free( builder->edge_set.values.block_pool, edge );
        return FALSE;
    }
    hash_set_remove( &builder->edge_set, edge );
    memory_pool_free( builder->edge_set.values.block_pool, edge );
    return TRUE;
}

void graph_builder_clear_edge( graph_builder* builder )
{
    hash_set_clear( &builder->edge_set );
}

boolean graph_builder_rebuild( graph_builder* builder )
{
    size_t node_size = builder->node_map.values.key_size;
    const void* from = NULL;
    const void* to = NULL;
    hash_set_iterator iter = { 0 };
    hash_set node = { 0 };
    hash_set* pnode = NULL;
    graph_builder_clear_nodes( builder );
    HASH_SET_ITERATOR_START( iter, &builder->edge_set );
    while ( HASH_SET_ITERATOR_IS_VALID( iter ) )
    {
        from = HASH_SET_ITERATOR_GET( iter );
        to = (byte*)from + node_size;
        if ( !hash_map_exist( &builder->node_map, from ) )
        {
            if ( !hash_map_insert( &builder->node_map, from, &node ) )
            {
                hash_set_destroy( &node );
                return FALSE;
            }
            pnode = hash_map_get( &builder->node_map, from );
            ASSERT( NULL != pnode );
            if ( !hash_set_initialize( pnode, node_size, builder->node_map.mempool ) )
            {
                hash_map_remove( &builder->node_map, from );
                return FALSE;
            }
        }
        else
        {
            pnode = hash_map_get( &builder->node_map, from );
            ASSERT( NULL != pnode );
        }
        if ( !hash_set_insert( pnode, to ) )
        {
            return FALSE;
        }
        HASH_SET_ITERATOR_NEXT( iter );
    }
    return TRUE;
}

boolean graph_builder_get_connected_nodes( graph_builder* builder, const void* from_node, hash_set* connected_nodes )
{
    hash_set* c = hash_map_get( &builder->node_map, from_node );
    if ( NULL == c )
    {
        return FALSE;
    }
    return hash_set_copy( connected_nodes, c );
}

graph* graph_builder_get_graph( graph_builder* builder )
{
    return &builder->current_graph;
}

boolean graph_builder_get_node_key( graph_builder* builder, const void* node, void* key )
{
    size_t node_size = builder->node_map.values.key_size;
    memcpy( key, node, node_size );
    return TRUE;
}

void graph_builder_clear_nodes( graph_builder* builder )
{
    hash_set* node = NULL;
    hash_map_iterator iter_node = { 0 };
    HASH_MAP_ITERATOR_START( iter_node, &builder->node_map );
    while ( HASH_MAP_ITERATOR_IS_VALID( iter_node ) )
    {
        node = hash_map_get( &builder->node_map, HASH_MAP_ITERATOR_GET( iter_node ) );
        hash_set_destroy( node );
        HASH_MAP_ITERATOR_NEXT( iter_node );
    }
    hash_map_clear( &builder->node_map );
}

