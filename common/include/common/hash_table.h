//
//  hash_table.h
//  common
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _HASH_TABLE_H_ )
#define _HASH_TABLE_H_

#include "memory_pool.h"

typedef struct hash_table hash_table;
typedef boolean (*hash_table_key_compare_function)( hash_table* table, const void* key1, const void* key2 );
typedef uint32 (*hash_table_hash_code_function)( const void* key, size_t size );

struct hash_table
{
    memory_pool*                        block_pool;
    size_t                              key_size;
    size_t                              value_size;
    hash_table_key_compare_function     key_equal;
    hash_table_hash_code_function       hash_function;
    void**                              index_block;
    size_t                              size;
    size_t                              element_count;
    size_t                              index_count;
};

boolean hash_table_initialize( hash_table* table, size_t key_size, size_t value_size, memory_pool* pool );

void hash_table_destroy( hash_table* table );

boolean hash_table_insert( hash_table* table, const void* key, const void* value );

void hash_table_remove( hash_table* table, const void* key );

void* hash_table_get( hash_table* table, const void* key );

boolean hash_table_exist( hash_table* table, const void* key );

void hash_table_clear( hash_table* table );

#endif /* defined(_HASH_TABLE_H_) */
