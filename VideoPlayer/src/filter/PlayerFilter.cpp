//
//  PlayerFilter.cpp
//  WisePlayer
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#include "PlayerFilter.h"

PlayerFilter::PlayerFilter( PlayerContext* context )
    : PlayerProcedure( context )
{
    
}

PlayerFilter::~PlayerFilter()
{
}

void PlayerFilter::Update()
{
    ProcessAnyInput();
}
