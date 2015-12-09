//
//  TestRedBlackTree.m
//  common
//
//  Created by Yi Huang on 28/4/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#import <XCTest/XCTest.h>

#import "red_black_tree.h"

@interface TestRedBlackTree : XCTestCase
{
    memory_pool         pool;
}

@end

@implementation TestRedBlackTree

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
    memory_pool_initialize( &pool, 4096, 4 );
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    memory_pool_destroy( &pool );
    [super tearDown];
}

- (void)testRedBlackTree {
    red_black_tree tree;
    XCTAssert( red_black_tree_initialize( &tree, sizeof(uint32), &pool ) );

    for ( uint32 i = 0; i < 5000; ++i )
    {
        XCTAssert( red_black_tree_insert( &tree, &i ) );
    }

    red_black_tree_iterator iter;
    RED_BLACK_TREE_ITERATOR_START( iter, &tree );
    for ( uint32 i = 0; i < 5000; ++i )
    {
        red_black_tree_iterator temp = red_black_tree_find( &tree, &i );
        XCTAssert( temp != NULL );
        XCTAssert( *(uint32*)RED_BLACK_TREE_ITERATOR_GET( temp ) == i );

        XCTAssert( iter == temp );
        XCTAssert( RED_BLACK_TREE_ITERATOR_IS_VALID( iter ) );
        RED_BLACK_TREE_ITERATOR_NEXT( iter );
    }

    for ( uint32 i = 0; i < 5000; ++i )
    {
        red_black_tree_remove( &tree, &i );
    }
    XCTAssert( tree.size == 0 );

    for ( uint32 i = 5000; i > 0; --i )
    {
        XCTAssert( red_black_tree_insert( &tree, &i ) );
    }
    RED_BLACK_TREE_ITERATOR_START( iter, &tree );
    for ( uint32 i = 1; i < 5001; ++i )
    {
        red_black_tree_iterator temp = red_black_tree_find( &tree, &i );
        XCTAssert( temp != NULL );
        XCTAssert( *(uint32*)RED_BLACK_TREE_ITERATOR_GET( temp ) == i );
        
        XCTAssert( iter == temp );
        XCTAssert( RED_BLACK_TREE_ITERATOR_IS_VALID( iter ) );
        RED_BLACK_TREE_ITERATOR_NEXT( iter );
    }

    red_black_tree_clear( &tree );
    for ( uint32 i = 1; i < 5001; ++i )
    {
        XCTAssert( red_black_tree_find( &tree, &i ) == NULL );
    }

    red_black_tree_destroy( &tree );
}

- (void)testMultipleRedBlackTree {
    red_black_tree tree;
    XCTAssert( red_black_tree_initialize( &tree, sizeof(uint32), &pool ) );

    uint32 value = 1;
    for ( uint32 i = 0; i < 5000; ++i )
    {
        XCTAssert( red_black_tree_insert( &tree, &value ) );
    }

    value = 2;
    red_black_tree_iterator iter = red_black_tree_insert( &tree, &value );
    XCTAssert( NULL != iter );
    red_black_tree_riterator rIterator;
    RED_BLACK_TREE_RITERATOR_START( rIterator, &tree );
    XCTAssert( rIterator == iter );
    XCTAssert( *(uint32*)RED_BLACK_TREE_RITERATOR_GET( rIterator ) == 2 );
    RED_BLACK_TREE_RITERATOR_NEXT( rIterator );
    while ( RED_BLACK_TREE_RITERATOR_IS_VALID( rIterator ) )
    {
        XCTAssert( *(uint32*)RED_BLACK_TREE_RITERATOR_GET( rIterator ) == 1 );
        RED_BLACK_TREE_RITERATOR_NEXT( rIterator );
    }

    value = 1;
    red_black_tree_iterator first = red_black_tree_find( &tree, &value );
    RED_BLACK_TREE_ITERATOR_START( iter, &tree );
    XCTAssert( *(uint32*)RED_BLACK_TREE_RITERATOR_GET( first ) == 1 );
    XCTAssert( iter == first );

    red_black_tree_remove( &tree, &value );
    XCTAssert( tree.size == 5000 );
    
    red_black_tree_destroy( &tree );
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
