//
//  hash_set.h
//  common
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _HASH_SET_H_ )
#define _HASH_SET_H_

#include "hash_table.h"
#include "list.h"

typedef struct hash_set
{
    list        keys;
    hash_table  values;
    size_t      value_size;
} hash_set;

boolean hash_set_initialize( hash_set* set, size_t value_size, memory_pool* pool );

void hash_set_destroy( hash_set* set );

boolean hash_set_exist( hash_set* set, const void* value );

boolean hash_set_insert( hash_set* set, const void* value );

void hash_set_remove( hash_set* set, const void* value );

void hash_set_clear( hash_set* set );

typedef list_iterator hash_set_iterator;

#define HASH_SET_SIZE( set ) \
    LIST_SIZE( &(set)->keys )

#define HASH_SET_IS_EMPTY( set ) \
    LIST_IS_EMPTY( &(set)->keys )

#define HASH_SET_ITERATOR_START( iter, set ) \
    LIST_ITERATOR_START( iter, &((set)->keys) )

#define HASH_SET_ITERATOR_IS_VALID( iter ) \
    LIST_ITERATOR_IS_VALID( iter )

#define HASH_SET_ITERATOR_NEXT( iter ) \
    LIST_ITERATOR_NEXT( iter )

#define HASH_SET_ITERATOR_GET( iter ) \
    LIST_ITERATOR_GET( iter )

boolean hash_set_equal( hash_set* set1, hash_set* set2 );

boolean hash_set_copy( hash_set* set_dst, hash_set* set_src );

boolean hash_set_sub( hash_set* result, hash_set* set1, hash_set* set2 );

boolean hash_set_add( hash_set* result, hash_set* set1, hash_set* set2 );

boolean hash_set_intersect( hash_set* result, hash_set* set1, hash_set* set2 );

#endif /* defined( _HASH_SET_H_ ) */
