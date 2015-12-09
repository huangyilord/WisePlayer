#if !defined ( _GRAPH_H_ )
#define _GRAPH_H_

#include "hash_set.h"

typedef boolean (*get_connected_nodes_function)( void* context, const void* from_node, hash_set* connected_nodes );

typedef boolean (*get_node_key_function)( void* context, const void* node, void* key );

typedef struct graph
{
    void*                           context;
    get_connected_nodes_function    connected_nodes_function;
    get_node_key_function           node_key_function;
} graph;

#endif /* defined(_GRAPH_H_) */
