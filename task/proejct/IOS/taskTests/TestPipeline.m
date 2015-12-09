#import <XCTest/XCTest.h>

#import "pipeline_procedure.h"
#import "pipeline_procedure_structures.h"
#import "thread.h"

#define PROCEDURE_NUM   2

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

boolean test_dst_input( pipeline_procedure* procedure, void* context, const void* key, void* data )
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

boolean test_procedure_input( pipeline_procedure* procedure, void* context, const void* key, void* data )
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

@interface TestPipeline : XCTestCase
{
    memory_pool                     mempool;
    task_processor_thread_pool      thread_pool;
}

@end

@implementation TestPipeline

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
    memory_pool_initialize( &mempool, 4096, 4 );
    task_processor_thread_pool_initialize( &thread_pool, 4, &mempool );
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    task_processor_thread_pool_destroy( &thread_pool );
    memory_pool_destroy( &mempool );
    [super tearDown];
}

- (void)testPipeline1 {
    pipeline_procedure_config config = {0};
    config.context = NULL;
    config.thread_pool = &thread_pool;
    config.key_size = sizeof(uint32);
    config.update_function = test_src_function;
    
    pipeline_procedure src;
    pipeline_procedure target;
    XCTAssert( pipeline_procedure_initialize( &src, &config ) );
    
    config.update_function = test_dst_function;
    config.input_data_handler = test_dst_input;
    XCTAssert( pipeline_procedure_initialize( &target, &config ) );
    uint32 index = 0;
    pipeline_procedure_output* srcoutput = pipeline_procedure_add_output( &src, &index );
    XCTAssert( NULL != srcoutput );
    pipeline_procedure_input* targetinput = pipeline_procedure_add_input( &target, &index );
    XCTAssert( NULL != targetinput );
    
    pipeline_procedure procedure[PROCEDURE_NUM];
    pipeline_connection connection[PROCEDURE_NUM+1];
    pipeline_procedure_output* output = srcoutput;
    config.update_function = test_procedure_function;
    config.input_data_handler = test_procedure_input;
    for ( uint32 i = 0; i < PROCEDURE_NUM; ++i )
    {
        XCTAssert( pipeline_procedure_initialize( &procedure[i], &config ) );
        pipeline_procedure_input* input = pipeline_procedure_add_input( &procedure[i], &index );
        XCTAssert( NULL != input );
        XCTAssert( pipeline_connection_initialize( &connection[i], 8, &mempool ) );
        XCTAssert( pipeline_connection_connect( &connection[i], output, input ) );
        output = pipeline_procedure_add_output( &procedure[i], &index );
        XCTAssert( NULL != output );
    }
    XCTAssert( pipeline_connection_initialize( &connection[PROCEDURE_NUM], 8, &mempool ) );
    XCTAssert( pipeline_connection_connect( &connection[PROCEDURE_NUM], output, targetinput ) );
    
    for ( uint32 n = 0; n < 100000; ++n )
    {
        XCTAssert( pipeline_procedure_start( &src ) );
        for ( uint32 i = 0; i <  PROCEDURE_NUM; ++i )
        {
            XCTAssert( pipeline_procedure_start( &procedure[i] ) );
        }
        XCTAssert( pipeline_procedure_start( &target ) );

        pipeline_procedure_stop( &src );
        for ( uint32 i = 0; i < PROCEDURE_NUM; ++i )
        {
            pipeline_procedure_stop( &procedure[i] );
        }
        pipeline_procedure_stop( &target );
    }
    
    pipeline_procedure_destroy( &target );
    for ( uint32 i = 0; i < PROCEDURE_NUM; ++i )
    {
        pipeline_procedure_destroy( &procedure[i] );
    }
    pipeline_procedure_destroy( &src );
    
    for ( uint32 i = 0; i < PROCEDURE_NUM + 1; ++i )
    {
        pipeline_connection_destroy( &connection[i] );
    }
}

/*
- (void)testPipeline2 {
    pipeline_procedure_config config = {0};
    config.context = NULL;
    config.thread_pool = &thread_pool;
    config.key_size = sizeof(uint32);
    config.update_function = test_src_function;

    pipeline_procedure src;
    pipeline_procedure target;
    XCTAssert( pipeline_procedure_initialize( &src, &config ) );

    config.update_function = test_dst_function;
    config.input_data_handler = test_dst_input;
    XCTAssert( pipeline_procedure_initialize( &target, &config ) );
    uint32 index = 0;
    pipeline_procedure_output* srcoutput = pipeline_procedure_add_output( &src, &index );
    XCTAssert( NULL != srcoutput );
    pipeline_procedure_input* targetinput = pipeline_procedure_add_input( &target, &index );
    XCTAssert( NULL != targetinput );

    pipeline_procedure procedure[PROCEDURE_NUM];
    pipeline_connection connection[PROCEDURE_NUM+1];
    pipeline_procedure_output* output = srcoutput;
    config.update_function = test_procedure_function;
    config.input_data_handler = test_procedure_input;
    for ( uint32 i = 0; i < PROCEDURE_NUM; ++i )
    {
        XCTAssert( pipeline_procedure_initialize( &procedure[i], &config ) );
        pipeline_procedure_input* input = pipeline_procedure_add_input( &procedure[i], &index );
        XCTAssert( NULL != input );
        XCTAssert( pipeline_connection_initialize( &connection[i], 8, &mempool ) );
        XCTAssert( pipeline_connection_connect( &connection[i], output, input ) );
        output = pipeline_procedure_add_output( &procedure[i], &index );
        XCTAssert( NULL != output );
    }
    XCTAssert( pipeline_connection_initialize( &connection[PROCEDURE_NUM], 8, &mempool ) );
    XCTAssert( pipeline_connection_connect( &connection[PROCEDURE_NUM], output, targetinput ) );

    XCTAssert( pipeline_procedure_start( &src ) );
    for ( uint32 i = 0; i <  PROCEDURE_NUM; ++i )
    {
        XCTAssert( pipeline_procedure_start( &procedure[i] ) );
    }
    XCTAssert( pipeline_procedure_start( &target ) );

    thread_sleep( 10000 );

    for ( uint32 i = 0; i < 1000; ++i )
    pipeline_procedure_stop( &src );
    for ( uint32 i = 0; i < PROCEDURE_NUM; ++i )
    {
        pipeline_procedure_stop( &procedure[i] );
    }
    pipeline_procedure_stop( &target );

    pipeline_procedure_destroy( &target );
    for ( uint32 i = 0; i < PROCEDURE_NUM; ++i )
    {
        pipeline_procedure_destroy( &procedure[i] );
    }
    pipeline_procedure_destroy( &src );

    for ( uint32 i = 0; i < PROCEDURE_NUM + 1; ++i )
    {
        pipeline_connection_destroy( &connection[i] );
    }
}
*/

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
