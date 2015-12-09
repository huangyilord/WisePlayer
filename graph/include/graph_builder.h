//
//  graph_builder.h
//  graph
//
//  Created by Yi Huang on 18/5/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _GRAPH_BUILDER_H_ )
#define _GRAPH_BUILDER_H_

#include "graph.h"
#include "hash_map.h"

typedef struct graph_builder
{
    hash_map        node_map;
    hash_set        edge_set;
    graph           current_graph;
} graph_builder;

boolean graph_builder_initialize( graph_builder* builder, size_t node_size, memory_pool* pool );

void graph_builder_destroy( graph_builder* builder );

boolean graph_builder_add_edge( graph_builder* builder, const void* from_node, const void* to_node );

boolean graph_builder_remove_edge( graph_builder* builder, const void* from_node, const void* to_node );

void graph_builder_clear_edge( graph_builder* builder );

boolean graph_builder_rebuild( graph_builder* builder );

boolean graph_builder_get_connected_nodes( graph_builder* builder, const void* from_node, hash_set* connected_nodes );

graph* graph_builder_get_graph( graph_builder* builder );


#endif
