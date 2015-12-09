#include "graph_explorer.h"

#include "common_debug.h"

boolean graph_explorer_initialize( graph_explorer* explorer, size_t node_key_size, size_t node_info_size, memory_pool* pool )
{
    if ( !explore_node_map_initialize( &explorer->explored_nodes, node_key_size, node_info_size, pool ) )
    {
        return FALSE;
    }
    if ( !red_black_tree_initialize( &explorer->opening_nodes, node_key_size, pool ) )
    {
        explore_node_map_destroy( &explorer->explored_nodes );
        return FALSE;
    }
    explorer->mempool = pool;
    explorer->key_size = node_key_size;
    explorer->node_size = node_info_size;
    return TRUE;
}

void graph_explorer_destroy( graph_explorer* explorer )
{
    red_black_tree_destroy( &explorer->opening_nodes );
    explore_node_map_destroy( &explorer->explored_nodes );
}

boolean graph_explorer_start_explore( graph_explorer* explorer, graph* g, const void* node, graph_explorer_callbacks callbacks )
{
    void* key = NULL;
    explorer->current_graph = g;
    explorer->opening_nodes.compare_fun = callbacks.compare_function;
    if ( callbacks.key_equal_function )
    {
        explorer->explored_nodes.nodes.values.key_equal = callbacks.key_equal_function;
    }
    if ( callbacks.key_hash_function )
    {
        explorer->explored_nodes.nodes.values.hash_function = callbacks.key_hash_function;
    }
    key = memory_pool_alloc( explorer->mempool );
    if ( NULL == key )
    {
        return FALSE;
    }
    if ( !g->node_key_function( g->context, node, key ) )
    {
        memory_pool_free( explorer->mempool, key );
        return FALSE;
    }
    if ( !explore_node_map_insert_node_with_key( &explorer->explored_nodes, key, node ) )
    {
        memory_pool_free( explorer->mempool, key );
        return FALSE;
    }
    if ( !red_black_tree_insert( &explorer->opening_nodes, key ) )
    {
        explore_node_map_clear( &explorer->explored_nodes );
        memory_pool_free( explorer->mempool, key );
        return FALSE;
    }
    memory_pool_free( explorer->mempool, key );
    return TRUE;
}

boolean graph_explorer_step( graph_explorer* explorer )
{
    red_black_tree_iterator node_iter;
    hash_set_iterator connection_iter;
    void* node_key = NULL;
    const void* node = NULL;
    hash_set connected_nodes;
    if ( RED_BLACK_TREE_IS_EMPTY( &explorer->opening_nodes ) )
    {
        return FALSE;
    }
    if ( !hash_set_initialize( &connected_nodes, explorer->key_size, explorer->mempool ) )
    {
        return FALSE;
    }
    connected_nodes.values.hash_function = explorer->explored_nodes.nodes.values.hash_function;
    connected_nodes.values.key_equal = explorer->explored_nodes.nodes.values.key_equal;
    RED_BLACK_TREE_ITERATOR_START( node_iter, &explorer->opening_nodes );
    node_key = RED_BLACK_TREE_ITERATOR_GET( node_iter );
    if ( explorer->current_graph->connected_nodes_function( explorer->current_graph->context, node_key, &connected_nodes ) )
    {
        HASH_SET_ITERATOR_START( connection_iter, &connected_nodes );
        while ( HASH_SET_ITERATOR_IS_VALID( connection_iter ) )
        {
            node_key = HASH_SET_ITERATOR_GET( connection_iter );
            ASSERT( NULL != node_key );
            if ( !red_black_tree_insert( &explorer->opening_nodes, node_key ) )
            {
                memory_pool_free( explorer->mempool, node_key );
                hash_set_destroy( &connected_nodes );
                return FALSE;
            }
            HASH_SET_ITERATOR_NEXT( connection_iter );
        }
    }
    red_black_tree_erase( &explorer->opening_nodes, node_iter );
    hash_set_destroy( &connected_nodes );
    return TRUE;
}

void graph_explorer_finish_explore( graph_explorer* explorer )
{
    red_black_tree_clear( &explorer->opening_nodes );
    explore_node_map_clear( &explorer->explored_nodes );
}