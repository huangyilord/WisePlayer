//
//  TestVideoPlayer.m
//  VideoPlayer
//
//  Created by Yi Huang on 25/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <XCTest/XCTest.h>
#import "VideoPlayer.h"

@interface TestVideoPlayer : XCTestCase

@end

@implementation TestVideoPlayer

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testVideoPlayer {
    VideoPlayer::VideoPlayer* player = new VideoPlayer::VideoPlayer();
    delete player;
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
