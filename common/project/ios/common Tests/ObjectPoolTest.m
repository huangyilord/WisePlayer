#import <XCTest/XCTest.h>
#import "object_pool.h"

@interface ObjectPoolTest : XCTestCase
{
    memory_pool mem_pool;
}

@end

@implementation ObjectPoolTest

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
    memory_pool_initialize( &mem_pool, 4096, 8 );
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    memory_pool_destroy( &mem_pool );
    [super tearDown];
}

- (void)testAlloc1
{
    uint32 i = 0;
    object_pool obj_pool;
    void* objs[10000] = {0};
    XCTAssert( object_pool_initialize( &obj_pool, &mem_pool, 8 ), @"Pass" );
    for ( i = 0; i < 10000; ++i )
    {
        objs[i] = object_pool_alloc( &obj_pool );
        XCTAssert( NULL != objs[i], @"Pass" );
    }
    XCTAssert( obj_pool.first_free_block, @"Pass" );
    for ( i = 0; i < 10000; ++i )
    {
        object_pool_free( &obj_pool, objs[i] );
        objs[i] = NULL;
    }
    XCTAssert( NULL == obj_pool.first_free_block, @"Pass" );
    object_pool_destroy( &obj_pool );
}

- (void)testAlloc2
{
    uint32 i = 0;
    object_pool obj_pool;
    void* objs[20] = {0};
    XCTAssert( object_pool_initialize( &obj_pool, &mem_pool, 128 ), @"Pass" );
    for ( i = 0; i < 5; ++i )
    {
        objs[i] = object_pool_alloc( &obj_pool );
        XCTAssert( NULL != objs[i], @"Pass" );
    }
    for ( ; i > 3; --i )
    {
        object_pool_free( &obj_pool, objs[i-1] );
        objs[i-1] = NULL;
    }
    for ( ; i < 10; ++i )
    {
        objs[i] = object_pool_alloc( &obj_pool );
        XCTAssert( NULL != objs[i], @"Pass" );
    }
    for ( ; i > 6; --i )
    {
        object_pool_free( &obj_pool, objs[i-1] );
        objs[i-1] = NULL;
    }
    for ( ; i < 15; ++i )
    {
        objs[i] = object_pool_alloc( &obj_pool );
        XCTAssert( NULL != objs[i], @"Pass" );
    }
    for ( ; i > 9; --i )
    {
        object_pool_free( &obj_pool, objs[i-1] );
        objs[i-1] = NULL;
    }
    for ( ; i < 20; ++i )
    {
        objs[i] = object_pool_alloc( &obj_pool );
        XCTAssert( NULL != objs[i], @"Pass" );
    }
    for ( i = 0; i < 20; ++i )
    {
        object_pool_free( &obj_pool, objs[i] );
        objs[i] = NULL;
    }
    XCTAssert( NULL == obj_pool.first_free_block, @"Pass" );
    object_pool_destroy( &obj_pool );
}

/*- (void)testAllocPerformance1 {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
        object_pool obj_pool;
        object_pool_initialize( &obj_pool, &mem_pool, 32 );
        void* first = NULL;
        void** current = &first;
        for ( uint32 i = 0; i < 5000000; ++i )
        {
            *current = object_pool_alloc( &obj_pool );
            current = *current;
        }
        object_pool_destroy( &obj_pool );
    }];
}*/

/*- (void)testAllocPerformance2 {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
        object_pool obj_pool;
        object_pool_initialize( &obj_pool, &mem_pool, 32 );
        void* first = NULL;
        void** current = &first;
        for ( uint32 i = 0; i < 5000000; ++i )
        {
            *current = object_pool_alloc( &obj_pool );
            current = *current;
        }
        *current = NULL;
        current = first;
        while ( *current != NULL )
        {
            first = current;
            current = *current;
            object_pool_free( &obj_pool, first );
        }
        object_pool_destroy( &obj_pool );
    }];
}*/

@end
