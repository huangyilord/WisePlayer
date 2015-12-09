//
//  hash_map.h
//  common
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _HASH_MAP_H_ )
#define _HASH_MAP_H_

#include "hash_table.h"
#include "list.h"

typedef struct hash_map
{
    list            keys;
    hash_table      values;
    size_t          value_size;
    memory_pool*    mempool;
} hash_map;

boolean hash_map_initialize( hash_map* map, size_t key_size, size_t value_size, memory_pool* pool );

void hash_map_destroy( hash_map* map );

void* hash_map_get( hash_map* map, const void* key );

boolean hash_map_insert( hash_map* map, const void* key, const void* value );

boolean hash_map_exist( hash_map* map, const void* key );

void hash_map_remove( hash_map* map, const void* key );

void hash_map_clear( hash_map* map );

typedef list_iterator hash_map_iterator;

#define HASH_MAP_SIZE( map ) \
    LIST_SIZE( &(map)->keys )

#define HASH_MAP_IS_EMPTY( map ) \
    LIST_IS_EMPTY( &(map)->keys )

#define HASH_MAP_ITERATOR_START( iter, map ) \
    LIST_ITERATOR_START( iter, &((map)->keys) )

#define HASH_MAP_ITERATOR_IS_VALID( iter ) \
    LIST_ITERATOR_IS_VALID( iter )

#define HASH_MAP_ITERATOR_NEXT( iter ) \
    LIST_ITERATOR_NEXT( iter )

#define HASH_MAP_ITERATOR_GET( iter ) \
    ((LIST_ITERATOR_GET( iter )))

#endif /* defined(_HASH_MAP_H_) */
