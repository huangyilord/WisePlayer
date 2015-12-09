//
//  FFMpegData.h
//  VideoPlayer
//
//  Created by Yi Huang on 12/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _FFMPEG_DATA_H_ )
#define _FFMPEG_DATA_H_

#include "PlayerTypes.h"

extern "C"
{
#include "libavfilter/avcodec.h"
#include "libavformat/avformat.h"
}

#define FFMPEG_VIDEO_PACKAGE_NAME   "FFMPEG_VIDEO_PACKAGE"
#define FFMPEG_AUDIO_PACKAGE_NAME   "FFMPEG_AUDIO_PACKAGE"

extern const PlayerProcedureKey FFMPEG_VIDEO_PACKAGE;
extern const PlayerProcedureKey FFMPEG_AUDIO_PACKAGE;

typedef struct FFMpegVideoPackage
{
    AVCodecContext*     codecContext;
    AVPacket            packet;
} FFMpegVideoPackage;

typedef struct FFMpegAudioPackage
{
    AVCodecContext*     codecContext;
    AVPacket            packet;
} FFMpegAudioPackage;

#endif /* defined(_FFMPEG_FRAME_H_) */
