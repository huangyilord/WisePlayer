//
//  PlayerDataSource.h
//  WisePlayer
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#if !defined ( _PLAYER_DATA_SOURCE_H_ )
#define _PLAYER_DATA_SOURCE_H_

#include "PlayerProcedure.h"

class PlayerDataSource
    : public PlayerProcedure
{
public:
    PlayerDataSource( PlayerContext* context );
    virtual ~PlayerDataSource();

    virtual bool SetURL( const char* url ) = 0;
    virtual bool Seek( int64 position ) = 0;
protected:
    virtual bool GetData() = 0;
private:
    // TaskProcedure
    virtual void Update();
};

#endif /* defined(_PLAYER_DATA_SOURCE_H_) */
