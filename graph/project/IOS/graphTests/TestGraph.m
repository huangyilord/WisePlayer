//
//  TestGraph.m
//  WisePlayer
//
//  Created by Yi Huang on 25/4/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "graph_builder.h"

#define MEMORY_BLOCK_SIZE 4096

@interface TestGraph : XCTestCase
{
    memory_pool         pool;
}

@end

@implementation TestGraph

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
    memory_pool_initialize( &pool, MEMORY_BLOCK_SIZE, 4 );
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    memory_pool_destroy( &pool );
    [super tearDown];
}

- (void)testExample {
    // This is an example of a functional test case.
    graph_builder g = { 0 };
    XCTAssert( graph_builder_initialize( &g, sizeof(uint32), &pool ) );

    uint32 from = 1;
    uint32 to = 2;
    XCTAssert( graph_builder_add_edge( &g, &from, &to ) );

    from = 2;
    to = 3;
    XCTAssert( graph_builder_add_edge( &g, &from, &to ) );

    from = 3;
    to = 4;
    XCTAssert( graph_builder_add_edge( &g, &from, &to ) );

    from = 4;
    to = 5;
    XCTAssert( graph_builder_add_edge( &g, &from, &to ) );

    from = 2;
    to = 5;
    XCTAssert( graph_builder_add_edge( &g, &from, &to ) );

    from = 5;
    to = 1;
    XCTAssert( graph_builder_add_edge( &g, &from, &to ) );

    XCTAssert( graph_builder_rebuild( &g ) );

    from = 1;
    to = 3;
    XCTAssert( graph_builder_add_edge( &g, &from, &to ) );
    XCTAssert( graph_builder_rebuild( &g ) );

    hash_set connections;
    hash_set_initialize( &connections, sizeof(uint32), &pool );
    from = 1;
    XCTAssert( graph_builder_get_connected_nodes( &g, &from, &connections ) );
    XCTAssert( connections.keys.size == 2 );
    to = 2;
    XCTAssert( hash_set_exist( &connections, &to ) );
    to = 3;
    XCTAssert( hash_set_exist( &connections, &to ) );

    from = 2;
    XCTAssert( graph_builder_get_connected_nodes( &g, &from, &connections ) );
    XCTAssert( connections.keys.size == 2 );
    to = 3;
    XCTAssert( hash_set_exist( &connections, &to ) );
    to = 5;
    XCTAssert( hash_set_exist( &connections, &to ) );

    from = 3;
    XCTAssert( graph_builder_get_connected_nodes( &g, &from, &connections ) );
    XCTAssert( connections.keys.size == 1 );
    to = 4;
    XCTAssert( hash_set_exist( &connections, &to ) );

    from = 4;
    XCTAssert( graph_builder_get_connected_nodes( &g, &from, &connections ) );
    XCTAssert( connections.keys.size == 1 );
    to = 5;
    XCTAssert( hash_set_exist( &connections, &to ) );

    from = 5;
    XCTAssert( graph_builder_get_connected_nodes( &g, &from, &connections ) );
    XCTAssert( connections.keys.size == 1 );
    to = 1;
    XCTAssert( hash_set_exist( &connections, &to ) );

    graph_builder_destroy( &g );
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
