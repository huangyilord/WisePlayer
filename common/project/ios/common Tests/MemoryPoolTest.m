#import <XCTest/XCTest.h>
#import "memory_pool.h"

@interface MemoryPoolTest : XCTestCase

@end

@implementation MemoryPoolTest

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testInitialize
{
    memory_pool pool;
    memory_pool_initialize( &pool, 1024, 8 );
    memory_pool_destroy( &pool );
}

- (void)testAlloc1
{
    memory_pool pool;
    memory_pool_initialize( &pool, 1024, 8 );
    void* block = memory_pool_alloc( &pool );
    XCTAssert( NULL != block, @"Pass" );
    memory_pool_free( &pool, block );
    memory_pool_destroy( &pool );
}

- (void)testAlloc2
{
    memory_pool pool;
    memory_pool_initialize( &pool, 1024, 8 );
    void* blocks[10] = {0};
    for ( uint32 i = 0; i < 10; ++i )
    {
        blocks[i] = memory_pool_alloc( &pool );
        XCTAssert( NULL != blocks[i], @"Pass" );
    }
    for ( uint32 i = 0; i < 10; ++i )
    {
        memory_pool_free( &pool, blocks[i] );
    }
    memory_pool_destroy( &pool );
}

/*
- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}
*/

@end
