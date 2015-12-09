//
//  TestAudio.m
//  Audio
//
//  Created by Yi Huang on 23/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#import <XCTest/XCTest.h>

#import "OpenALAudioContext.h"
#import "Voice.h"

@interface TestAudio : XCTestCase
{
    Audio::IAudioContext*   context;
}

@end

@implementation TestAudio

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
    Audio::OpenALAudioContext* alContext = new Audio::OpenALAudioContext();
    alContext->Initialize();
    context = alContext;
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    context->Release();
    [super tearDown];
}

- (void)testPlay {
    Audio::AUDIO_SOURCE_HANDLER source = context->CreateSource( Audio::IAudioContext::SourceType_Static );
    Audio::AUDIO_BUFFER_HANDLER buffer = context->CreateBuffer();
    XCTAssert( source != NULL );
    XCTAssert( buffer != NULL );
    XCTAssert( context->BufferData( buffer, Audio::IAudioContext::DataFormat_STEREO16, s_testVoice, sizeof(s_testVoice), 5000 ) );
    XCTAssert( context->SourceEnqueueBuffer( source, buffer ) );
    context->PlaySource( source );
    sleep( 2 );
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
