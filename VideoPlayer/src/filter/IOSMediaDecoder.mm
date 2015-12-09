//
//  IOSMediaDecoder.m
//  VideoPlayer
//
//  Created by DongRanRan on 15/11/16.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#include "IOSMediaDecoder.h"

static AVAssetReader* CreateReader(NSURL *url, MediaType mediaType);

void* IOSMediaDecoder::mediaDecoder(char *url, MediaType mediaType)
{
    NSString * urlStr = [NSString stringWithUTF8String:url];
    AVAssetReader *reader = CreateReader([NSURL URLWithString:urlStr], mediaType);
    
    AVAssetReaderOutput *readerOutput = reader.outputs[0];
    
    if (![reader startReading]) {
        NSLog(@"error reading from the file");
        return nil;
    }
    
    CMSampleBufferRef buffer;
    while ( [reader status]==AVAssetReaderStatusReading)
    {
        buffer = [readerOutput copyNextSampleBuffer];
        return buffer;
    }
    
    if (reader.status == AVAssetReaderStatusCompleted) {
        NSLog(@"AVAssetReaderStatusCompleted");
        [reader cancelReading];
    }
    else if (reader.status == AVAssetReaderStatusFailed || reader.status == AVAssetReaderStatusUnknown)
    {
        NSLog(@"AVAssetReaderStatusFailed error:%@",reader.error.localizedDescription);
    }
    else if (reader.status == AVAssetReaderStatusCancelled)
    {
        NSLog(@"AVAssetReaderStatusCancelled");
    }
    return nil;
}

AVAssetReader* CreateReader(NSURL *url, MediaType mediaType)
{
    AVAsset *asset = [AVAsset assetWithURL:url];
    
    NSError *error = nil;
    AVAssetReader *assetReader = [AVAssetReader assetReaderWithAsset:asset error:&error];
    
    NSMutableDictionary *outputSettings = [NSMutableDictionary dictionary];
    [outputSettings setObject:@(kCVPixelFormatType_32BGRA) forKey:(id)kCVPixelBufferPixelFormatTypeKey];
    
    NSArray* video_tracks;
    if (mediaType == MediaType_Video) {
        video_tracks = [asset tracksWithMediaType:AVMediaTypeVideo];
    }
    else
    {
        video_tracks = [asset tracksWithMediaType:AVMediaTypeAudio];
    }
    AVAssetTrack* video_track = [video_tracks objectAtIndex:0];
    
    AVAssetReaderTrackOutput* readerVideoTrackOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:video_track outputSettings:outputSettings];
    [assetReader addOutput:readerVideoTrackOutput];
    
    return assetReader;
}

