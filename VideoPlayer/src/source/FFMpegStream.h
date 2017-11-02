//
//  FFMpegStream.h
//  VideoPlayer
//
//  Created by Yi Huang on 12/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _FFMPEG_STREAM_H_ )
#define _FFMPEG_STREAM_H_

#include "PlayerDataSource.h"
#include "FFMpegData.h"

typedef struct FFMpegSeekRequest
{
    int64           position;
    bool            isValid;
} FFMpegSeekRequest;

class FFMpegStream
    : public PlayerDataSource
{
public:
    FFMpegStream( PlayerContext* context );
    virtual ~FFMpegStream();

    bool Initialize();
    void Destroy();

    // PlayerDataSource
    virtual bool SetURL( const char* url );
    virtual bool Seek( int64 position );

    PROCEDURE_DESC_START( FFMpegStream )
        PROCEDURE_INPUT_START()
        PROCEDURE_INPUT_END()
        PROCEDURE_OUTPUT_START()
            PROCEDURE_OUTPUT( FFMPEG_VIDEO_PACKAGE_NAME )
            //PROCEDURE_OUTPUT( FFMPEG_AUDIO_PACKAGE_NAME )
        PROCEDURE_OUTPUT_END()
    PROCEDURE_DESC_END( FFMpegStream )
protected:
    virtual void OnStart();
    virtual void OnStop();
    virtual bool GetData();
private:
    bool OpenFFStream( const char* filePath );
    void CloseFFStream();

    AVInputFormat*                  m_pFormat;
    AVFormatContext*                m_pFormatContext;
    AVDictionary*                   m_pOptions;
    AVCodecContext*                 m_pVideoCodecContext;
    AVCodec*                        m_pVideoCodec;
    AVCodecContext*                 m_pAudioCodecContext;
    AVCodec*                        m_pAudioCodec;
    int                             m_videoStreamNumber;
    int                             m_audioStreamNumber;
    AVPacket                        m_tempPacket;

    FFMpegSeekRequest               m_seekRequest;
    FFMpegSeekRequest* volatile     m_pSeekRequest;
};

#endif /* defined(_FFMPEG_STREAM_H_) */
