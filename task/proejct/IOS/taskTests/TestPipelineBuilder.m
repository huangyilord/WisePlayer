//
//  TestPipelineBuilder.m
//  task
//
//  Created by Yi Huang on 15/5/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "pipeline_builder.h"

#define DATA_SIZE (4 * 1024 * 1024)

typedef struct prc_key
{
    char    name[8];
} prc_key;

typedef struct pipeline_test_context
{
    memory_pool                     mempool;
    task_processor_thread_pool      processor_thread_pool;
    object_pool                     procedure_pool;
} pipeline_test_context;

static boolean test_src_function( pipeline_procedure* procedure, void* context )
{
    hash_map_iterator iter_output;
    HASH_MAP_ITERATOR_START( iter_output, &procedure->output );
    uint32* output_data = pipeline_procedure_get_output_buffer( procedure, HASH_MAP_ITERATOR_GET(iter_output), sizeof(uint32) );
    if ( output_data )
    {
        static uint32 index = 0;
        *output_data = index ++;
        NSLog(@"src %d", *output_data);
        pipeline_procedure_commit_output_buffer( procedure, HASH_MAP_ITERATOR_GET(iter_output) );
    }
    return TRUE;
}

static boolean test_dst_input( pipeline_procedure* procedure, void* context, const void* key, void* data )
{
    uint32* input_data = data;
    NSLog(@"dst %d", *input_data);
    return TRUE;
}

static boolean test_dst_function( pipeline_procedure* procedure, void* context )
{
    pipeline_procedure_handle_any_input( procedure );
    return TRUE;
}

static boolean test_procedure_input( pipeline_procedure* procedure, void* context, const void* key, void* data )
{
    hash_map_iterator iter_output;
    HASH_MAP_ITERATOR_START( iter_output, &procedure->output );
    uint32* input_data = data;
    uint32* output_data = pipeline_procedure_get_output_buffer( procedure, HASH_MAP_ITERATOR_GET(iter_output), sizeof(uint32) );
    if ( output_data )
    {
        *output_data = *input_data;
        NSLog(@"procedure %d", *output_data);
        pipeline_procedure_commit_output_buffer( procedure, HASH_MAP_ITERATOR_GET(iter_output) );
        return TRUE;
    }
    return FALSE;
}

static boolean test_procedure_function( pipeline_procedure* procedure, void* context )
{
    pipeline_procedure_handle_any_input( procedure );
    return TRUE;
}

static pipeline_procedure* create_src_procedure( void* context )
{
    pipeline_test_context* pipeline_context = (pipeline_test_context*)context;
    pipeline_procedure* new_procedure = object_pool_alloc( &pipeline_context->procedure_pool );
    if ( NULL != new_procedure )
    {
        pipeline_procedure_config config = {0};
        config.context = NULL;
        config.thread_pool = &pipeline_context->processor_thread_pool;
        config.key_size = sizeof(prc_key);
        config.update_function = test_src_function;
        if ( !pipeline_procedure_initialize( new_procedure, &config ) )
        {
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
        prc_key key = {0};
        key.name[0] = '2';
        if ( !pipeline_procedure_add_output( new_procedure, &key ) )
        {
            pipeline_procedure_destroy( new_procedure );
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
        key.name[0] = '4';
        if ( !pipeline_procedure_add_output( new_procedure, &key ) )
        {
            pipeline_procedure_destroy( new_procedure );
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
    }
    return new_procedure;
}

static void destroy_src_procedure( pipeline_procedure* proc, void* context )
{
    pipeline_test_context* pipeline_context = (pipeline_test_context*)context;
    pipeline_procedure_destroy( proc );
    object_pool_free( &pipeline_context->procedure_pool, proc );
}

static pipeline_procedure* create_dst_procedure( void* context )
{
    pipeline_test_context* pipeline_context = (pipeline_test_context*)context;
    pipeline_procedure* new_procedure = object_pool_alloc( &pipeline_context->procedure_pool );
    if ( NULL != new_procedure )
    {
        pipeline_procedure_config config = {0};
        config.context = NULL;
        config.thread_pool = &pipeline_context->processor_thread_pool;
        config.key_size = sizeof(prc_key);
        config.update_function = test_dst_function;
        config.input_data_handler = test_dst_input;
        if ( !pipeline_procedure_initialize( new_procedure, &config ) )
        {
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
        prc_key key = {0};
        key.name[0] = '0';
        if ( !pipeline_procedure_add_input( new_procedure, &key ) )
        {
            pipeline_procedure_destroy( new_procedure );
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
    }
    return new_procedure;
}

static void destroy_dst_procedure( pipeline_procedure* proc, void* context )
{
    pipeline_test_context* pipeline_context = (pipeline_test_context*)context;
    pipeline_procedure_destroy( proc );
    object_pool_free( &pipeline_context->procedure_pool, proc );
}

static pipeline_procedure* create_procedure1( void* context )
{
    pipeline_test_context* pipeline_context = (pipeline_test_context*)context;
    pipeline_procedure* new_procedure = object_pool_alloc( &pipeline_context->procedure_pool );
    if ( NULL != new_procedure )
    {
        pipeline_procedure_config config = {0};
        config.context = NULL;
        config.thread_pool = &pipeline_context->processor_thread_pool;
        config.key_size = sizeof(prc_key);
        config.update_function = test_procedure_function;
        config.input_data_handler = test_procedure_input;
        if ( !pipeline_procedure_initialize( new_procedure, &config ) )
        {
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
        prc_key key = {0};
        key.name[0] = '1';
        if ( !pipeline_procedure_add_input( new_procedure, &key ) )
        {
            pipeline_procedure_destroy( new_procedure );
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
        key.name[0] = '3';
        if ( !pipeline_procedure_add_input( new_procedure, &key ) )
        {
            pipeline_procedure_destroy( new_procedure );
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
        key.name[0] = '0';
        if ( !pipeline_procedure_add_output( new_procedure, &key ) )
        {
            pipeline_procedure_destroy( new_procedure );
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
    }
    return new_procedure;
}

static void destroy_procedure1( pipeline_procedure* proc, void* context )
{
    pipeline_test_context* pipeline_context = (pipeline_test_context*)context;
    pipeline_procedure_destroy( proc );
    object_pool_free( &pipeline_context->procedure_pool, proc );
}

static pipeline_procedure* create_procedure2( void* context )
{
    pipeline_test_context* pipeline_context = (pipeline_test_context*)context;
    pipeline_procedure* new_procedure = object_pool_alloc( &pipeline_context->procedure_pool );
    if ( NULL != new_procedure )
    {
        pipeline_procedure_config config = {0};
        config.context = NULL;
        config.thread_pool = &pipeline_context->processor_thread_pool;
        config.key_size = sizeof(prc_key);
        config.update_function = test_procedure_function;
        config.input_data_handler = test_procedure_input;
        if ( !pipeline_procedure_initialize( new_procedure, &config ) )
        {
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
        prc_key key = {0};
        key.name[0] = '2';
        if ( !pipeline_procedure_add_input( new_procedure, &key ) )
        {
            pipeline_procedure_destroy( new_procedure );
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
        key.name[0] = '1';
        if ( !pipeline_procedure_add_output( new_procedure, &key ) )
        {
            pipeline_procedure_destroy( new_procedure );
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
    }
    return new_procedure;
}

static void destroy_procedure2( pipeline_procedure* proc, void* context )
{
    pipeline_test_context* pipeline_context = (pipeline_test_context*)context;
    pipeline_procedure_destroy( proc );
    object_pool_free( &pipeline_context->procedure_pool, proc );
}

static pipeline_procedure* create_procedure3( void* context )
{
    pipeline_test_context* pipeline_context = (pipeline_test_context*)context;
    pipeline_procedure* new_procedure = object_pool_alloc( &pipeline_context->procedure_pool );
    if ( NULL != new_procedure )
    {
        pipeline_procedure_config config = {0};
        config.context = NULL;
        config.thread_pool = &pipeline_context->processor_thread_pool;
        config.key_size = sizeof(prc_key);
        config.update_function = test_procedure_function;
        config.input_data_handler = test_procedure_input;
        if ( !pipeline_procedure_initialize( new_procedure, &config ) )
        {
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
        prc_key key = {0};
        key.name[0] = '4';
        if ( !pipeline_procedure_add_input( new_procedure, &key ) )
        {
            pipeline_procedure_destroy( new_procedure );
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
        key.name[0] = '3';
        if ( !pipeline_procedure_add_output( new_procedure, &key ) )
        {
            pipeline_procedure_destroy( new_procedure );
            object_pool_free( &pipeline_context->procedure_pool, new_procedure );
            new_procedure = NULL;
        }
    }
    return new_procedure;
}

static void destroy_procedure3( pipeline_procedure* proc, void* context )
{
    pipeline_test_context* pipeline_context = (pipeline_test_context*)context;
    pipeline_procedure_destroy( proc );
    object_pool_free( &pipeline_context->procedure_pool, proc );
}


@interface TestPipelineBuilder : XCTestCase
{
    pipeline_test_context       context;
}

@end

@implementation TestPipelineBuilder

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
    memory_pool_initialize( &context.mempool, 4096, 8 );
    object_pool_initialize( &context.procedure_pool, &context.mempool, sizeof(pipeline_procedure) );
    task_processor_thread_pool_initialize( &context.processor_thread_pool, 4, &context.mempool );
    
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    task_processor_thread_pool_destroy( &context.processor_thread_pool );
    object_pool_destroy( &context.procedure_pool );
    memory_pool_destroy( &context.mempool );
    [super tearDown];
}

- (void)testPipelineBuilder {
    // This is an example of a functional test case.
    pipeline_builder* builder = pipeline_builder_create( sizeof(prc_key) );

    /**
     *  src 2 -------- 2 p2 1 -------- 1 p1 0 --------- dst
     *   4                                3
     *   |                                |
     *    ------------ 4 p3 3 ------------
     */
    XCTAssert( NULL != builder );
    // procedure 3
    prc_key proc_key = {0};
    proc_key.name[0] = 'p';
    proc_key.name[1] = '3';
    pipeline_procedure_desc* desc = pipeline_builder_add_procedure( builder, &proc_key );
    XCTAssert( NULL != desc );
    prc_key pin_key = {0};
    pin_key.name[0] = '4';
    hash_set_insert( &desc->input_keys, &pin_key );
    pin_key.name[0] = '3';
    hash_set_insert( &desc->output_keys, &pin_key );
    desc->context = &context;
    desc->create_function = create_procedure3;
    desc->destroy_function = destroy_procedure3;
    
    // procedure 2
    proc_key.name[0] = 'p';
    proc_key.name[1] = '2';
    desc = pipeline_builder_add_procedure( builder, &proc_key );
    XCTAssert( NULL != desc );
    pin_key.name[0] = '2';
    hash_set_insert( &desc->input_keys, &pin_key );
    pin_key.name[0] = '1';
    hash_set_insert( &desc->output_keys, &pin_key );
    desc->context = &context;
    desc->create_function = create_procedure2;
    desc->destroy_function = destroy_procedure2;

    // procedure 1
    proc_key.name[1] = '1';
    desc = pipeline_builder_add_procedure( builder, &proc_key );
    XCTAssert( NULL != desc );
    pin_key.name[0] = '1';
    hash_set_insert( &desc->input_keys, &pin_key );
    pin_key.name[0] = '3';
    hash_set_insert( &desc->input_keys, &pin_key );
    pin_key.name[0] = '0';
    hash_set_insert( &desc->output_keys, &pin_key );
    desc->context = &context;
    desc->create_function = create_procedure1;
    desc->destroy_function = destroy_procedure1;

    // dst
    proc_key.name[1] = '0';
    desc = pipeline_builder_add_procedure( builder, &proc_key );
    XCTAssert( NULL != desc );
    pin_key.name[0] = '0';
    hash_set_insert( &desc->input_keys, &pin_key );
    desc->context = &context;
    desc->create_function = create_dst_procedure;
    desc->destroy_function = destroy_dst_procedure;

    XCTAssert( pipeline_builder_rebuild_procedure_graph( builder ) );

    pipeline p;
    XCTAssert( pipeline_initialize( &p, &context.processor_thread_pool, &context.mempool ) );
    pipeline_procedure_desc src_desc;
    src_desc.context = &context;
    src_desc.create_function = create_src_procedure;
    src_desc.destroy_function = destroy_src_procedure;
    XCTAssert( pipeline_add_src( &p, &src_desc ) );
    XCTAssert( pipeline_builder_build_pipeline( builder, &p ) );

    pipeline_start( &p );

    thread_sleep( 30000 );

    pipeline_stop( &p );
    pipeline_stop( &p );
    pipeline_destroy( &p );
    pipeline_builder_destroy( builder );
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
