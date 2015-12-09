//
//  PlayerDataSource.cpp
//  WisePlayer
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#include "PlayerDataSource.h"
#include "PlayerControl.h"

PlayerDataSource::PlayerDataSource( PlayerContext* context )
    : PlayerProcedure( context )
{
    
}

PlayerDataSource::~PlayerDataSource()
{
    
}

void PlayerDataSource::Update()
{
    if ( !GetData() )
    {
        m_pPlayerContext->pPlayerControl->StopAsynchronize();
    }
}
