//
//  WisePlayerEventListener.cpp
//  WisePlayer
//
//  Created by Yi Huang on 12/10/15.
//  Copyright © 2015 IPTV. All rights reserved.
//

#include "WisePlayerEventListener.h"

WisePlayerEventListener::WisePlayerEventListener()
    : player( nil )
    , delegate( nil )
{
}

WisePlayerEventListener::~WisePlayerEventListener()
{
}

void WisePlayerEventListener::VideoPlayerStatusChanged( VideoPlayerEventListener::VideoPlayerStatus status )
{
    if ( delegate && [delegate respondsToSelector:@selector(playerStatusChanged:status:)] )
    {
        switch ( status )
        {
            case VideoPlayerEventListener::VideoPlayerStatus_Stop:
                [delegate playerStatusChanged:player status:WisePlayerStatus_Stop];
                break;
            case VideoPlayerEventListener::VideoPlayerStatus_Playing:
                [delegate playerStatusChanged:player status:WisePlayerStatus_Playing];
                break;
            case VideoPlayerEventListener::VideoPlayerStatus_Loading:
                [delegate playerStatusChanged:player status:WisePlayerStatus_Loading];
                break;
        }
    }
}
