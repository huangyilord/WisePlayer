//
//  pipelin_builder_structures.h
//  task
//
//  Created by Yi Huang on 12/5/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _PIPELINE_BUILDER_STRUCTURES_H_ )
#define _PIPELINE_BUILDER_STRUCTURES_H_

#include "graph_explorer.h"
#include "pipeline_procedure.h"
#include "pipeline_builder.h"

typedef struct pipeline_builder_node_key pipeline_builder_node_key;
typedef struct pipeline_builder_node pipeline_builder_node;

struct pipeline_builder_node_key
{
    pipeline_builder_node*          node;
    uint32                          hash_key;
    uint32                          reserved;
};

struct pipeline_builder_node
{
    hash_set                        unlinked_input;
    hash_set                        unlinked_output;
    pipeline_procedure_desc*        new_procedure;
    pipeline_builder_node*          previous_node;
    uint32                          cost;
    uint32                          reserved;
};

boolean pipeline_builder_node_initialize( pipeline_builder_node* node, size_t key_size, memory_pool* pool );

boolean pipeline_builder_node_destroy( pipeline_builder_node* node );

boolean pipeline_builder_node_get_key( void* context, const void* node, void* key );

int32 pipeline_builder_node_compare( red_black_tree* tree, const void* key1, const void* key2 );

boolean pipeline_builder_node_key_equal( hash_table* table, const void* key1, const void* key2 );

uint32 pipeline_builder_node_key_hash_code( const void* key, size_t size );

#endif /* defined(__task__pipelin_builder_structures__) */
