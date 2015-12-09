//
//  hash_table.c
//  common
//
//  Created by Yi Huang on 6/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "hash_table.h"

#define HASH_TABLE_ELEMENT_FLAG_ENGAGED     0x1
#define HASH_TABLE_ELEMENT_FLAG_HASNEXT     0x2

typedef struct hash_table_element
{
    size_t  flags;
} hash_table_element;

static uint32 hash2( uint32 key, uint32 size )
{
    return 1 + ( (key >> 5) + 1 ) % ( size - 1 );
}

// DJB hash
static uint32 hash1( const void* key, size_t size )
{
    uint32 hash = 5381;
    byte* b = (byte*)key;
    uint32 i = 0;
    for( i = 0; i < size; ++i )
    {
        hash = ((hash << 5) + hash) + b[i];
    }
    return hash;
}

static boolean default_key_equals_function( hash_table* table, const void* key1, const void* key2 )
{
    return memcmp( key1, key2, table->key_size ) == 0;
}

boolean hash_table_initialize( hash_table* table, size_t key_size, size_t value_size, memory_pool* pool )
{
    if ( pool->block_size < key_size + sizeof(hash_table_element) + value_size )
    {
        return FALSE;
    }
    table->block_pool = pool;
    table->key_size = key_size;
    table->value_size = value_size;
    table->key_equal = default_key_equals_function;
    table->hash_function = hash1;
    table->index_block = (void**)memory_pool_alloc( pool );
    if ( NULL == table->index_block )
    {
        return FALSE;
    }
    memset( table->index_block, 0, pool->block_size );
    table->size = 0;
    table->element_count = (uint32)(pool->block_size / (key_size + sizeof(hash_table_element) + value_size) );
    table->index_count = (uint32)(pool->block_size / sizeof(void*));
    return TRUE;
}

void hash_table_destroy( hash_table* table )
{
    if ( NULL != table->index_block )
    {
        hash_table_clear( table );
        memory_pool_free( table->block_pool, table->index_block );
        table->index_block = NULL;
    }
}

boolean hash_table_insert( hash_table* table, const void* key, const void* value )
{
    uint32 hash_key = table->hash_function( key, table->key_size );
    hash_table_element* element = NULL;
    uint32 capacity = table->index_count * table->element_count;
    uint32 index = hash_key % capacity;
    uint32 block_index = index / table->element_count;
    uint32 element_index = index % table->element_count;
    void* element_key = NULL;
    uint32 i = 0;
    if ( table->size == capacity )
    {
        return FALSE;
    }
    for ( i = 0; i < table->index_count * table->element_count; ++i )
    {
        index = hash_key % capacity;
        block_index = index / table->element_count;
        element_index = index % table->element_count;
        element = (hash_table_element*)table->index_block[block_index];
        if ( NULL == element )
        {
            element = (hash_table_element*)memory_pool_alloc( table->block_pool );
            if ( NULL == element )
            {
                return FALSE;
            }
            memset( element, 0, table->block_pool->block_size );
            table->index_block[block_index] = element;
        }
        element_key = (byte*)element + (table->key_size + sizeof(hash_table_element) + table->value_size) * element_index;
        element = (hash_table_element*)((byte*)element_key + table->key_size);
        if ( element->flags & HASH_TABLE_ELEMENT_FLAG_ENGAGED )
        {
            if ( table->key_equal( table, element_key, key ) )
            {
                memcpy( element + 1, value, table->value_size );
                return TRUE;
            }
            element->flags |= HASH_TABLE_ELEMENT_FLAG_HASNEXT;
        }
        else
        {
            element->flags |= HASH_TABLE_ELEMENT_FLAG_ENGAGED;
            memcpy( element_key, key, table->key_size );
            memcpy( element + 1, value, table->value_size );
            ++table->size;
            return TRUE;
        }
        hash_key += hash2( hash_key, capacity );
    }
    return FALSE;
}

void hash_table_remove( hash_table* table, const void* key )
{
    uint32 hash_key = table->hash_function( key, table->key_size );
    hash_table_element* element = NULL;
    uint32 capacity = table->index_count * table->element_count;
    uint32 index = hash_key % capacity;
    uint32 block_index = index / table->element_count;
    uint32 element_index = index % table->element_count;
    void* element_key = NULL;
    uint32 i = 0;
    for ( i = 0; i < capacity; ++i )
    {
        index = hash_key % capacity;
        block_index = index / table->element_count;
        element_index = index % table->element_count;
        element = (hash_table_element*)table->index_block[block_index];
        if ( NULL == element )
        {
            return;
        }
        element_key = (byte*)element + (table->key_size + sizeof(hash_table_element) + table->value_size) * element_index;
        element = (hash_table_element*)((byte*)element_key + table->key_size);
        if ( element->flags & HASH_TABLE_ELEMENT_FLAG_ENGAGED )
        {
            if ( table->key_equal( table, element_key, key ) )
            {
                element->flags &= ~HASH_TABLE_ELEMENT_FLAG_ENGAGED;
                --table->size;
                return;
            }
        }
        if ( !(element->flags & HASH_TABLE_ELEMENT_FLAG_HASNEXT) )
        {
            return;
        }
        hash_key += hash2( hash_key, capacity );
    }
    return;
}

void* hash_table_get( hash_table* table, const void* key )
{
    uint32 hash_key = table->hash_function( key, table->key_size );
    hash_table_element* element = NULL;
    uint32 capacity = table->index_count * table->element_count;
    uint32 index = hash_key % ( capacity );
    uint32 block_index = index / table->element_count;
    uint32 element_index = index % table->element_count;
    void* element_key = NULL;
    uint32 i = 0;
    for ( i = 0; i < capacity; ++i )
    {
        index = hash_key % ( capacity );
        block_index = index / table->element_count;
        element_index = index % table->element_count;
        element = (hash_table_element*)table->index_block[block_index];
        if ( NULL == element )
        {
            return NULL;
        }
        element_key = (byte*)element + (table->key_size + sizeof(hash_table_element) + table->value_size) * element_index;
        element = (hash_table_element*)((byte*)element_key + table->key_size);
        if ( element->flags & HASH_TABLE_ELEMENT_FLAG_ENGAGED )
        {
            if ( table->key_equal( table, element_key, key ) )
            {
                return element + 1;
            }
        }
        if ( !(element->flags & HASH_TABLE_ELEMENT_FLAG_HASNEXT) )
        {
            return FALSE;
        }
        hash_key += hash2( hash_key, capacity );
    }
    return NULL;
}

boolean hash_table_exist( hash_table* table, const void* key )
{
    uint32 hash_key = table->hash_function( key, table->key_size );
    hash_table_element* element = NULL;
    uint32 capacity = table->index_count * table->element_count;
    uint32 index = hash_key % ( capacity );
    uint32 block_index = index / table->element_count;
    uint32 element_index = index % table->element_count;
    void* element_key = NULL;
    uint32 i = 0;
    for ( i = 0; i < capacity; ++i )
    {
        index = hash_key % ( capacity );
        block_index = index / table->element_count;
        element_index = index % table->element_count;
        element = (hash_table_element*)table->index_block[block_index];
        if ( NULL == element )
        {
            return FALSE;
        }
        element_key = (byte*)element + (table->key_size + sizeof(hash_table_element) + table->value_size) * element_index;
        element = (hash_table_element*)((byte*)element_key + table->key_size);
        if ( element->flags & HASH_TABLE_ELEMENT_FLAG_ENGAGED )
        {
            if ( table->key_equal( table, element_key, key ) )
            {
                return TRUE;
            }
        }
        if ( !(element->flags & HASH_TABLE_ELEMENT_FLAG_HASNEXT) )
        {
            return FALSE;
        }
        hash_key += hash2( hash_key, capacity );
    }
    return FALSE;
}

void hash_table_clear( hash_table* table )
{
    if ( table->index_block )
    {
        uint32 i = 0;
        for ( i = 0; i < table->index_count; ++i )
        {
            void* block = table->index_block[i];
            if ( NULL != block )
            {
                memory_pool_free( table->block_pool, block );
            }
        }
        memset( table->index_block, 0, table->block_pool->block_size );
        table->size = 0;
    }
}

