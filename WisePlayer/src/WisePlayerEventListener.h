//
//  WisePlayerEventListener.hpp
//  WisePlayer
//
//  Created by Yi Huang on 12/10/15.
//  Copyright © 2015 IPTV. All rights reserved.
//

#if !defined ( _WISE_PLAYER_EVENT_LISTENER_H_ )
#define _WISE_PLAYER_EVENT_LISTENER_H_

#include "VideoPlayer.h"
#import "WisePlayer.h"

class WisePlayerEventListener
    : public VideoPlayerEventListener
{
public:
    WisePlayerEventListener();
    ~WisePlayerEventListener();

    virtual void VideoPlayerStatusChanged( VideoPlayerStatus status );

public:
    __weak WisePlayer*                  player;
    __weak id<WisePlayerDelegate>       delegate;
};

#endif /* _VIDEO_PLAYER_EVENT_LISTENER_H_ */
