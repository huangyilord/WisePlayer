//
//  explore_node_map.c
//  graph
//
//  Created by Yi Huang on 18/5/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "explore_node_map.h"

boolean explore_node_map_initialize( explore_node_map* map, size_t key_size, size_t node_size, memory_pool* pool )
{
    if ( !object_pool_initialize( &map->node_pool, pool, node_size ) )
    {
        return FALSE;
    }
    if ( !hash_map_initialize( &map->nodes, key_size, sizeof(void*), pool ) )
    {
        object_pool_destroy( &map->node_pool );
        return FALSE;
    }
    return TRUE;
}

void explore_node_map_destroy( explore_node_map* map )
{
    hash_map_destroy( &map->nodes );
    object_pool_destroy( &map->node_pool );
}

void explore_node_map_clear( explore_node_map* map )
{
    hash_map_clear( &map->nodes );
    object_pool_clear( &map->node_pool );
}

void* explore_node_map_alloc_node( explore_node_map* map )
{
    return object_pool_alloc( &map->node_pool );
}

void explore_node_map_free_node( explore_node_map* map, void* node )
{
    object_pool_free( &map->node_pool, node );
}

boolean explore_node_map_insert_node_with_key( explore_node_map* map, const void* key, const void* node )
{
    return hash_map_insert( &map->nodes, key, &node );
}

void* explore_node_map_get_node_with_key( explore_node_map* map, const void* key )
{
    void** node = hash_map_get( &map->nodes, key );
    if ( NULL == node )
    {
        return NULL;
    }
    return *node;
}

void explore_node_map_remove_node_with_key( explore_node_map* map, const void* key )
{
    hash_map_remove( &map->nodes, key );
}

