//
//  explore_node_map.h
//  graph
//
//  Created by Yi Huang on 18/5/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _EXPLORE_NODE_MAP_H_ )
#define _EXPLORE_NODE_MAP_H_

#include "hash_map.h"

typedef hash_table_key_compare_function explore_node_map_key_equal_function;
typedef hash_table_hash_code_function explore_node_map_key_hash_function;

typedef struct explore_node_map
{
    hash_map            nodes;
    object_pool         node_pool;
} explore_node_map;

boolean explore_node_map_initialize( explore_node_map* map, size_t key_size, size_t node_size, memory_pool* pool );

void explore_node_map_destroy( explore_node_map* map );

void explore_node_map_clear( explore_node_map* map );

void* explore_node_map_alloc_node( explore_node_map* map );

void explore_node_map_free_node( explore_node_map* map, void* node );

boolean explore_node_map_insert_node_with_key( explore_node_map* map, const void* key, const void* node );

void* explore_node_map_get_node_with_key( explore_node_map* map, const void* key );

void explore_node_map_remove_node_with_key( explore_node_map* map, const void* key );


#endif /* defined(_EXPLORE_NODE_MAP_H_) */
