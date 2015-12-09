#import <XCTest/XCTest.h>

#import "task_processor_thread_pool.h"

static uint32 test_task_function( void* data, memory_pool* pool );

uint32 test_task_function( void* data, memory_pool* pool )
{
    NSLog( @"begin test task" );
    list l;
    list_initialize( &l, sizeof(void*), pool );
    void* value = NULL;
    for ( uint32 i = 0; i < 1000000; ++i )
    {
        list_push_back( &l, &value );
    }
    list_destroy( &l );
    NSLog( @"end test task" );
    return 0;
}

@interface TaskTest : XCTestCase
{
    memory_pool         mempool;
}
@end

@implementation TaskTest

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
    memory_pool_initialize( &mempool, 4096, 4 );
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    memory_pool_destroy( &mempool );
    [super tearDown];
}

- (void)testNormal {
    // This is an example of a functional test case.
    task_processor_thread_pool processor_thread_pool;
    XCTAssert( task_processor_thread_pool_initialize( &processor_thread_pool, 2, &mempool ) );

    task_queue queue1, queue2;
    XCTAssert( task_queue_initialize( &queue1 ) );
    XCTAssert( task_queue_initialize( &queue2 ) );

    task t;
    task_initialize( &t, test_task_function, NULL );

    list l;
    list_initialize( &l, sizeof(void*), &mempool );
    for ( uint32 i = 0; i < 20; ++i )
    {
        task_processor_thread* thread = task_processor_thread_pool_get( &processor_thread_pool );
        XCTAssert( NULL != thread );
        task_processor* processor = task_processor_thread_get_processor( thread );
        XCTAssert( task_processor_add_task_queue( processor, &queue1 ) );
        XCTAssert( task_processor_add_task_queue( processor, &queue2 ) );
        list_push_back( &l, &thread );
    }

    list_iterator iter;
    for ( uint32 j = 0; j < 500; ++j )
    {
        task_queue_add_task( &queue1, &t );
        task_queue_add_task( &queue2, &t );
    }

    thread_sleep( 3000 );

    LIST_ITERATOR_START(iter, &l);
    while ( LIST_ITERATOR_IS_VALID(iter) )
    {
        task_processor_thread* thread = *(task_processor_thread**)LIST_ITERATOR_GET(iter);
        task_processor* processor = task_processor_thread_get_processor( thread );
        task_processor_clear_task_queue( processor );
        task_processor_thread_pool_put( &processor_thread_pool, thread );
        LIST_ITERATOR_NEXT(iter);
    }
    list_destroy( &l );
    task_destroy( &t );
    task_queue_destroy( &queue1 );
    task_queue_destroy( &queue2 );
    task_processor_thread_pool_destroy( &processor_thread_pool );
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
