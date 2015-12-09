//
//  VideoPlayer.h
//  VideoPlayer
//
//  Created by Yi Huang on 27/5/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#if !defined ( _VIDEO_PLAYER_H_ )
#define _VIDEO_PLAYER_H_

#include <string>

namespace Graphic
{
    class IGraphicContext;
    class GraphicResourceMgr;
}

namespace Audio
{
    class IAudioContext;
}

class VideoPlayerInternal;
class VideoPlayerEventListener
{
public:
    enum VideoPlayerStatus
    {
        VideoPlayerStatus_Stop
        , VideoPlayerStatus_Preparing
        , VideoPlayerStatus_Loading
        , VideoPlayerStatus_Playing
    };
    virtual void VideoPlayerStatusChanged( VideoPlayerStatus status ) = 0;
};

typedef struct VideoPlayerConfig
{
    Graphic::IGraphicContext*       pGraphicContext;
    Graphic::GraphicResourceMgr*    resource;
    Audio::IAudioContext*           pAudioContext;
    VideoPlayerEventListener*       pEventListener;
} VideoPlayerConfig;

class VideoPlayer
{
public:
    VideoPlayer();
    ~VideoPlayer();

    bool Initialize( const VideoPlayerConfig& config );
    void Destroy();

    void SetURL( const char* url );

    bool PrepareToPlay();
    void Start();
    void Stop();
    void Pause();

    int64_t GetProgress();
    int64_t GetDuration();
    bool Seek( int64_t position );

    void SetDisplaySize( float width, float height );
private:
    VideoPlayerInternal*        m_pPlayer;
};

#endif
