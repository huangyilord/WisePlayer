#import <XCTest/XCTest.h>

extern "C"
{
#import "list.h"
}
#include <list>

@interface ListTest : XCTestCase
{
    memory_pool pool;
}

@end

@implementation ListTest

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.

    memory_pool_initialize( &pool, 512, 5 );
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];

    memory_pool_destroy( &pool );
}

- (void)testExample {
    // This is an example of a functional test case.
    list l;
    list_initialize( &l, sizeof(void*), &pool );
    uint32 array[] = { 0, 1, 2, 3, 4 };
    for ( uint32 i = 0; i < 5; ++i )
    {
        XCTAssert(list_push_back( &l, array + i ));
    }
    XCTAssert( *(uint32*)list_front( &l ) == 0 );
    XCTAssert( *(uint32*)list_back( &l ) == 4 );
    list_destroy( &l );
}


- (void)testNSArrayPerformance
{
    NSMutableArray* array = [[NSMutableArray alloc] init];
    [self measureBlock:^{
        for ( uint32 i = 0; i < 3000000; ++i )
        {
            [array addObject:@(i)];
        }
    }];
}

- (void)testListPerformance
{
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
        list l;
        list_initialize( &l, sizeof(uint32), &pool );
        for ( uint32 i = 0; i < 3000000; ++i )
        {
            list_push_back( &l, &i );
        }
    }];
}

- (void)testSTLListPerformance
{
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
        std::list<uint32> l;
        for ( uint32 i = 0; i < 3000000; ++i )
        {
            l.push_back( i );
        }
    }];
}


@end
