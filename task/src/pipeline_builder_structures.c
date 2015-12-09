//
//  pipelin_builder_structures.c
//  task
//
//  Created by Yi Huang on 12/5/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "pipeline_builder_structures.h"

static uint32 DJB_hash_initialize()
{
    return 5381;
}

static uint32 DJB_hash_update( uint32 hash, const void* key, size_t size )
{
    byte* b = (byte*)key;
    uint32 i = 0;
    for( i = 0; i < size; ++i )
    {
        hash = ((hash << 5) + hash) + b[i];
    }
    return hash;
}

static uint32 DJB_hash( const void* key, size_t size )
{
    uint32 hash = DJB_hash_initialize();
    return DJB_hash_update( hash, key, size );
}

boolean pipeline_builder_node_initialize( pipeline_builder_node* node, size_t key_size, memory_pool* pool )
{
    if ( !hash_set_initialize( &node->unlinked_input, key_size, pool ) )
    {
        return FALSE;
    }
    if ( !hash_set_initialize( &node->unlinked_output, key_size, pool ) )
    {
        hash_set_destroy( &node->unlinked_input );
        return FALSE;
    }
    node->previous_node = NULL;
    node->new_procedure = NULL;
    node->cost = 0;
    return TRUE;
}

boolean pipeline_builder_node_destroy( pipeline_builder_node* node )
{
    hash_set_destroy( &node->unlinked_output );
    hash_set_destroy( &node->unlinked_input );
    return TRUE;
}

boolean pipeline_builder_node_get_key( void* context, const void* node, void* key )
{
    hash_set_iterator iter;
    pipeline_builder_node* builder_node = (pipeline_builder_node*)node;
    pipeline_builder_node_key* builder_key = (pipeline_builder_node_key*)key;
    builder_key->node = builder_node;
    builder_key->hash_key = 0;
    HASH_SET_ITERATOR_START( iter, &builder_node->unlinked_input );
    while ( HASH_SET_ITERATOR_IS_VALID( iter ) )
    {
        builder_key->hash_key += DJB_hash( HASH_SET_ITERATOR_GET( iter ), builder_node->unlinked_input.value_size );
        HASH_SET_ITERATOR_NEXT( iter );
    }
    HASH_SET_ITERATOR_START( iter, &builder_node->unlinked_output );
    while ( HASH_SET_ITERATOR_IS_VALID( iter ) )
    {
        builder_key->hash_key += DJB_hash( HASH_SET_ITERATOR_GET( iter ), builder_node->unlinked_output.value_size );
        HASH_SET_ITERATOR_NEXT( iter );
    }
    return TRUE;
}

int32 pipeline_builder_node_compare( red_black_tree* tree, const void* key1, const void* key2 )
{
    pipeline_builder_node_key* node_key1 = (pipeline_builder_node_key*)key1;
    pipeline_builder_node_key* node_key2 = (pipeline_builder_node_key*)key2;
    uint32 cost1 = node_key1->node->cost;
    uint32 cost2 = node_key2->node->cost;
    if ( !HASH_SET_IS_EMPTY(&node_key1->node->unlinked_output) )
    {
        cost1 += 1;
    }
    if ( !HASH_SET_IS_EMPTY(&node_key2->node->unlinked_output) )
    {
        cost2 += 1;
    }
    return cost1 - cost2;
}

boolean pipeline_builder_node_key_equal( hash_table* table, const void* key1, const void* key2 )
{
    pipeline_builder_node_key* node_key1 = (pipeline_builder_node_key*)key1;
    pipeline_builder_node_key* node_key2 = (pipeline_builder_node_key*)key2;
    if ( node_key1->hash_key != node_key2->hash_key )
    {
        return FALSE;
    }
    if ( !hash_set_equal( &node_key1->node->unlinked_input, &node_key2->node->unlinked_input ) )
    {
        return FALSE;
    }
    if ( !hash_set_equal( &node_key1->node->unlinked_output, &node_key2->node->unlinked_output ) )
    {
        return FALSE;
    }
    return TRUE;
}

uint32 pipeline_builder_node_key_hash_code( const void* key, size_t size )
{
    pipeline_builder_node_key* node_key = (pipeline_builder_node_key*)key;
    return node_key->hash_key;
}
