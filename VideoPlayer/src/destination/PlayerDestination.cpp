//
//  PlayerDestination.cpp
//  WisePlayer
//
//  Created by Yi Huang on 8/6/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#include "PlayerDestination.h"
#include "PlayerControl.h"

extern "C"
{
#include "common_time.h"
}

PlayerDestination::PlayerDestination( PlayerContext* context )
    : PlayerProcedure( context )
    , m_progress( 0 )
{
    m_destinationContext.totalTimeElapst = 0;
    m_destinationContext.lastUpdateTime = 0;
    m_destinationContext.timeInterval = 0;
}

PlayerDestination::~PlayerDestination()
{
}

bool PlayerDestination::Initialize()
{
    if ( !PlayerProcedure::Initialize() )
    {
        return false;
    }
    return true;
}

void PlayerDestination::Destroy()
{
    PlayerProcedure::Destroy();
}

bool PlayerDestination::UpdateContent( const PlayerDestinationContext& )
{
    return ProcessAnyInput();
}

int64 PlayerDestination::GetProgress() const
{
    return m_progress;
}

void PlayerDestination::SetProgress( int64 progress )
{
    m_progress = progress;
    m_pPlayerContext->pPlayerControl->ReportProgress( GetDesc().key, progress );
}

void PlayerDestination::Update()
{
    uint32 tickCount = time_get_tick_count();
    m_destinationContext.timeInterval = tickCount - m_destinationContext.lastUpdateTime;
    m_destinationContext.lastUpdateTime = tickCount;
    if ( UpdateContent( m_destinationContext ) )
    {
        m_destinationContext.totalTimeElapst += m_destinationContext.timeInterval;
    }
    thread_sleep( 10 );
}
