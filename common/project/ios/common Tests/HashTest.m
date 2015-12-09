#import <XCTest/XCTest.h>
#import "hash_map.h"
#import "hash_set.h"
#import "thread.h"

@interface HashTest : XCTestCase
{
    memory_pool         mempool;
}
@end

@implementation HashTest

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

- (void)testHashTable {
    // This is an example of a functional test case.
    hash_table table;
    XCTAssert( hash_table_initialize( &table, sizeof(uint32), sizeof(uint32), &mempool) );

    uint32 value = 1;
    for ( uint32 i = 0; i < 10000; ++i )
    {
        XCTAssert(hash_table_insert( &table, &i, &value ));
    }

    for ( uint32 i = 0; i < 10000; ++i )
    {
        XCTAssert( hash_table_exist( &table, &i ) );
        uint32* v = hash_table_get( &table, &i );
        XCTAssert( 1 == *v );
    }

    for ( uint32 i = 10000; i < 20000; ++i )
    {
        XCTAssert( !hash_table_exist( &table, &i ) );
        XCTAssert( NULL == hash_table_get( &table, &i ) );
    }

    for ( uint32 i = 0; i < 5000; ++i )
    {
        hash_table_remove( &table, &i );
    }
    for ( uint32 i = 0; i < 5000; ++i )
    {
        XCTAssert( !hash_table_exist( &table, &i ) );
        XCTAssert( NULL == hash_table_get( &table, &i ) );
    }

    for ( uint32 i = 5000; i < 10000; ++i )
    {
        XCTAssert( hash_table_exist( &table, &i ) );
        uint32* v = hash_table_get( &table, &i );
        XCTAssert( 1 == *v );
    }

    for ( uint32 i = 5000; i < 10000; ++i )
    {
        hash_table_remove( &table, &i );
    }

    XCTAssert( 0 == table.size );
    hash_table_destroy( &table );
}

- (void)testHashMap {
    // This is an example of a functional test case.
    hash_map map;
    XCTAssert( hash_map_initialize( &map, sizeof(uint32), sizeof(uint32), &mempool) );

    uint32 value = 1;
    for ( uint32 i = 0; i < 10000; ++i )
    {
        XCTAssert(hash_map_insert( &map, &i, &value ));
    }
    
    for ( uint32 i = 0; i < 10000; ++i )
    {
        XCTAssert( hash_map_exist( &map, &i ) );
        uint32* v = hash_map_get( &map, &i );
        XCTAssert( 1 == *v );
    }
    
    for ( uint32 i = 10000; i < 20000; ++i )
    {
        XCTAssert( !hash_map_exist( &map, &i ) );
        XCTAssert( NULL == hash_map_get( &map, &i ) );
    }
    
    for ( uint32 i = 0; i < 5000; ++i )
    {
        hash_map_remove( &map, &i );
    }
    for ( uint32 i = 0; i < 5000; ++i )
    {
        XCTAssert( !hash_map_exist( &map, &i ) );
        XCTAssert( NULL == hash_map_get( &map, &i ) );
    }
    
    for ( uint32 i = 5000; i < 10000; ++i )
    {
        XCTAssert( hash_map_exist( &map, &i ) );
        uint32* v = hash_map_get( &map, &i );
        XCTAssert( 1 == *v );
    }
    
    for ( uint32 i = 5000; i < 10000; ++i )
    {
        hash_map_remove( &map, &i );
    }
    
    XCTAssert( 0 == map.keys.size );
    hash_map_destroy( &map );
}

- (void)testHashSet {
    // This is an example of a functional test case.
    hash_set set;
    XCTAssert( hash_set_initialize( &set, sizeof(uint32), &mempool ) );
    
    for ( uint32 i = 0; i < 10000; ++i )
    {
        XCTAssert(hash_set_insert( &set, &i ));
    }
    
    for ( uint32 i = 0; i < 10000; ++i )
    {
        XCTAssert( hash_set_exist( &set, &i ) );
    }
    
    for ( uint32 i = 10000; i < 20000; ++i )
    {
        XCTAssert( !hash_set_exist( &set, &i ) );
    }
    
    for ( uint32 i = 0; i < 5000; ++i )
    {
        hash_set_remove( &set, &i );
    }
    for ( uint32 i = 0; i < 5000; ++i )
    {
        XCTAssert( !hash_set_exist( &set, &i ) );
    }
    
    for ( uint32 i = 5000; i < 10000; ++i )
    {
        XCTAssert( hash_set_exist( &set, &i ) );
    }
    
    for ( uint32 i = 5000; i < 10000; ++i )
    {
        hash_set_remove( &set, &i );
    }
    
    XCTAssert( 0 == set.keys.size );
    hash_set_destroy( &set );
}

/*- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
        hash_table table;
        XCTAssert( hash_table_initialize( &table, sizeof(uint32), sizeof(uint32), &mempool) );
        uint32 value = 1; 
        for ( uint32 i = 0; i < 100000; ++i )
        {
            (void)hash_table_insert( &table, &i, &value );
        }
        for ( uint32 i = 0; i < 100000; ++i )
        {
            (void)hash_table_exist( &table, &i );
            (void)hash_table_get( &table, &i );
        }
        for ( uint32 i = 100000; i < 200000; ++i )
        {
            (void)hash_table_exist( &table, &i );
            (void)hash_table_get( &table, &i );
        }
        for ( uint32 i = 0; i < 50000; ++i )
        {
            hash_table_remove( &table, &i );
        }
        for ( uint32 i = 0; i < 50000; ++i )
        {
            (void)hash_table_exist( &table, &i );
            (void)hash_table_get( &table, &i );
        }
        for ( uint32 i = 50000; i < 100000; ++i )
        {
            (void)hash_table_exist( &table, &i );
            (void)hash_table_get( &table, &i );
        }
        for ( uint32 i = 50000; i < 100000; ++i )
        {
            hash_table_remove( &table, &i );
        }
        hash_table_destroy( &table );
    }];
}*/

@end
