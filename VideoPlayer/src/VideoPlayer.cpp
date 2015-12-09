//
//  VideoPlayer.cpp
//  VideoPlayer
//
//  Created by Yi Huang on 27/5/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#include "VideoPlayer.h"

#include "VideoPlayerInternal.h"

VideoPlayer::VideoPlayer()
    : m_pPlayer( NULL )
{
}

VideoPlayer::~VideoPlayer()
{
    Destroy();
}

bool VideoPlayer::Initialize( const VideoPlayerConfig& config  )
{
    if ( NULL == m_pPlayer )
    {
        m_pPlayer = new(std::nothrow) VideoPlayerInternal();
    }
    return m_pPlayer->Initialize( config );
}

void VideoPlayer::Destroy()
{
    if ( NULL != m_pPlayer )
    {
        delete m_pPlayer;
        m_pPlayer = NULL;
    }
}

void VideoPlayer::SetURL( const char* url )
{
    m_pPlayer->SetURL( url );
}

bool VideoPlayer::PrepareToPlay()
{
    return m_pPlayer->PrepareToPlay();
}

void VideoPlayer::Start()
{
    m_pPlayer->Start();
}

void VideoPlayer::Stop()
{
    m_pPlayer->Stop();
}

void VideoPlayer::Pause()
{
    m_pPlayer->Pause();
}

int64_t VideoPlayer::GetProgress()
{
    return m_pPlayer->GetProgress();
}

int64_t VideoPlayer::GetDuration()
{
    return m_pPlayer->GetDuration();
}

bool VideoPlayer::Seek( int64_t position )
{
    return m_pPlayer->Seek( position );
}

void VideoPlayer::SetDisplaySize( float width, float height )
{
    m_pPlayer->SetDisplaySize( width, height );
}
