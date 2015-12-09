//
//  PlayerFilter.h
//  WisePlayer
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#if !defined ( _PLAYER_FILTER_H_ )
#define _PLAYER_FILTER_H_

#include "PlayerProcedure.h"

class PlayerFilter
    : public PlayerProcedure
{
public:
    PlayerFilter( PlayerContext* context );
    virtual ~PlayerFilter();

protected:
    // PlayerProcedure
    virtual void Update();
};

#endif /* defined( _PLAYER_FILTER_H_ ) */
