//
//  WisePlayerTest.m
//  WisePlayer
//
//  Created by Yi Huang on 25/6/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <XCTest/XCTest.h>
#import "WisePlayer.h"

@interface WisePlayerTest : XCTestCase

@end

@implementation WisePlayerTest

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testWisePlayer {
    WisePlayer* player = [[WisePlayer alloc] init];
    [player stop];
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
