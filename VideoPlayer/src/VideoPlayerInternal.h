//
//  VideoPlayerInternal.hpp
//  VideoPlayer
//
//  Created by Yi Huang on 10/10/15.
//  Copyright © 2015 huangyi. All rights reserved.
//

#if !defined ( _VIDEO_PLAYER_INTERNAL_H_ )
#define _VIDEO_PLAYER_INTERNAL_H_

#include "VideoPlayer.h"
#include "PlayerControl.h"

namespace Graphic
{
    class IGraphicContext;
    class GraphicResourceMgr;
}

namespace Audio
{
    class IAudioContext;
}

struct PlayerContext;

class PlayerDataSource;

class VideoPlayerInternal
    : private PlayerStatusListener
{
public:
    VideoPlayerInternal();
    ~VideoPlayerInternal();
    
    bool Initialize( const VideoPlayerConfig& config );
    void Destroy();
    
    inline void SetURL( const char* url ) { m_url = url; }
    
    bool PrepareToPlay();
    void Start();
    void Stop();
    void Pause();

    int64_t GetProgress();
    int64_t GetDuration();
    bool Seek( int64_t position );
    
    void SetDisplaySize( float width, float height );
private:
    // PlayerStatusListener
    virtual void OnStatusChanged( PlayerControl::Status status );

    void Reset();
    
    PlayerContext*                  m_pPlayerContext;
    VideoPlayerConfig               m_config;
    PlayerDataSource*               m_pDataSource;
    std::string                     m_url;
};

#endif /* _VIDEO_PLAYER_INTERNAL_H_ */
