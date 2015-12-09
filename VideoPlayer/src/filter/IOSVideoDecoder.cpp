//
//  IOSVideoDecoder.m
//  VideoPlayer
//
//  Created by DongRanRan on 15/11/16.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#import "IOSVideoDecoder.h"
#import "IOSMediaDecoder.h"

IOSVideoDecoder::IOSVideoDecoder( PlayerContext* context )
: PlayerFilter( context )
{
    
}

IOSVideoDecoder::~IOSVideoDecoder()
{
    
}

bool IOSVideoDecoder::Initialize()
{
    if ( !PlayerFilter::Initialize() )
    {
        Destroy();
        return false;
    }

    return true;
}

void IOSVideoDecoder::Destroy()
{
    PlayerFilter::Destroy();
}

bool IOSVideoDecoder::ProcessInput( const PlayerProcedureKey&, void* data )
{
    //从data中获取url
    hlsurldata* urlData = (hlsurldata*)data;
    IOSMediaDecoder mediaCoder = IOSMediaDecoder();
    void* buffer = mediaCoder.mediaDecoder(urlData->url, MediaType_Video);

    if (!buffer) {
        return false;
    }
    
    void* *outputBuffer = (void* *)GetOutputBuffer(IOS_VIDEO_PACKAGE, sizeof(buffer));
    
    if (!outputBuffer) {
        return false;
    }
    
    *outputBuffer = buffer;
    CommitOutputBuffer(IOS_VIDEO_PACKAGE);
    return true;
}
