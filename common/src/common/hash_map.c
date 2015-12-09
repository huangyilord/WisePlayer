//
//  hash_map.c
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "hash_map.h"

#include "common_debug.h"

typedef struct hash_map_element
{
    list_iterator       iter;
} hash_map_element;

boolean hash_map_initialize( hash_map* map, size_t key_size, size_t value_size, memory_pool* pool )
{
    map->mempool = pool;
    map->value_size = value_size;
    if ( !list_initialize( &map->keys, key_size, pool ) )
    {
        return FALSE;
    }
    if ( !hash_table_initialize( &map->values, key_size, sizeof(hash_map_element) + value_size, pool ) )
    {
        list_destroy( &map->keys );
        return FALSE;
    }
    return TRUE;
}

void hash_map_destroy( hash_map* map )
{
    hash_table_destroy( &map->values );
    list_destroy( &map->keys );
}

void* hash_map_get( hash_map* map, const void* key )
{
    hash_map_element* elem = hash_table_get( &map->values, key );
    if ( NULL != elem )
    {
        return elem + 1;
    }
    return NULL;
}

boolean hash_map_insert( hash_map* map, const void* key, const void* value )
{
    hash_map_element* elem = hash_table_get( &map->values, key );
    if ( NULL != elem )
    {
        memcpy( elem + 1, value, map->values.value_size );
        return TRUE;
    }
    elem = (hash_map_element*)memory_pool_alloc( map->mempool );
    if ( NULL == elem )
    {
        return FALSE;
    }
    if ( !list_push_back( &map->keys, key ) )
    {
        memory_pool_free( map->mempool, elem );
        return FALSE;
    }
    elem->iter = map->keys.last;
    memcpy( elem + 1, value, map->value_size );
    if ( !hash_table_insert( &map->values, key, elem ) )
    {
        list_pop_back( &map->keys );
        memory_pool_free( map->mempool, elem );
        return FALSE;
    }
    memory_pool_free( map->mempool, elem );
    return TRUE;
}

boolean hash_map_exist( hash_map* map, const void* key )
{
    return hash_table_exist( &map->values, key );
}

void hash_map_remove( hash_map* map, const void* key )
{
    hash_map_element* elem = hash_table_get( &map->values, key );
    if ( NULL != elem )
    {
        list_remove( &map->keys, elem->iter );
        hash_table_remove( &map->values, key );
    }
}

void hash_map_clear( hash_map* map )
{
    list_clear( &map->keys );
    hash_table_clear( &map->values );
}

