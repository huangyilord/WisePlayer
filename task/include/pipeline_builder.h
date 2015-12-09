//
//  pipeline_builder.h
//  task
//
//  Created by Yi Huang on 2/5/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _PIPELINE_BUILDER_H_ )
#define _PIPELINE_BUILDER_H_

#include "pipeline.h"

typedef struct pipeline_builder pipeline_builder;

typedef pipeline_procedure* (*pipeline_procedure_create_function)( void* );
typedef void (*pipeline_procedure_destroy_function)( pipeline_procedure*, void* );

typedef struct pipeline_procedure_desc
{
    hash_set                                input_keys;
    hash_set                                output_keys;
    void*                                   context;
    pipeline_procedure_create_function      create_function;
    pipeline_procedure_destroy_function     destroy_function;
} pipeline_procedure_desc;

pipeline_builder* pipeline_builder_create( size_t key_size );

void pipeline_builder_destroy( pipeline_builder* builder );

pipeline_procedure_desc* pipeline_builder_add_procedure( pipeline_builder* builder, const void* key );

void pipeline_builder_remove_procedure( pipeline_builder* builder, const void* key );

void pipeline_builder_clear_procedure( pipeline_builder* builder );

boolean pipeline_builder_rebuild_procedure_graph( pipeline_builder* builder );

void pipeline_builder_clear_procedure_graph( pipeline_builder* builder );

boolean pipeline_builder_build_pipeline( pipeline_builder* builder, pipeline* p );

#endif /* defined(_PIPELINE_BUILDER_H_) */
