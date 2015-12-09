//
//  VideoPlayerInternal.cpp
//  VideoPlayer
//
//  Created by Yi Huang on 10/10/15.
//  Copyright © 2015 huangyi. All rights reserved.
//

#include "VideoPlayerInternal.h"

#include "PlayerTypes.h"
#include "PlayerControl.h"
#include "RegisterProcedures.h"

#include "source/FFMpegStream.h"

#define MEMORY_BLOCK_SIZE       4096

VideoPlayerInternal::VideoPlayerInternal()
    : m_pPlayerContext( NULL )
{
    memset( &m_config, 0, sizeof(VideoPlayerConfig) );
}

VideoPlayerInternal::~VideoPlayerInternal()
{
    Destroy();
}

bool VideoPlayerInternal::Initialize( const VideoPlayerConfig& config  )
{
    m_config = config;
    m_pPlayerContext = new(std::nothrow) PlayerContext();
    if ( NULL == m_pPlayerContext )
    {
        return false;
    }
    memset( m_pPlayerContext, 0, sizeof(PlayerContext) );
    memory_pool_initialize( &m_pPlayerContext->mempool, MEMORY_BLOCK_SIZE, 32 );
    if ( !task_processor_thread_pool_initialize( &m_pPlayerContext->processor_thread_pool, 4, &m_pPlayerContext->mempool ) )
    {
        Destroy();
        return false;
    }
    m_pPlayerContext->builder = pipeline_builder_create( sizeof(PlayerProcedureKey) );
    if ( NULL == m_pPlayerContext->builder )
    {
        Destroy();
        return false;
    }
    if ( !hash_map_initialize( &m_pPlayerContext->procedure_types, sizeof(PlayerProcedureKey), sizeof(void*), &m_pPlayerContext->mempool ) )
    {
        Destroy();
        return false;
    }
    if ( !pipeline_initialize( &m_pPlayerContext->play_pipeline, &m_pPlayerContext->processor_thread_pool, &m_pPlayerContext->mempool ) )
    {
        Destroy();
        return false;
    }
    
    Graphic::UtilGraphicResourceController* utilResController = new(std::nothrow) Graphic::UtilGraphicResourceController();
    if ( NULL == utilResController || !config.resource->RegisterController( Graphic::GraphicResourceController::GRCID_UTIL, config.pGraphicContext, utilResController ) )
    {
        Destroy();
        return false;
    }
    m_pPlayerContext->pPlayerControl = new(std::nothrow) PlayerControl();
    if ( !m_pPlayerContext->pPlayerControl || !m_pPlayerContext->pPlayerControl->Initialize( m_pPlayerContext ) )
    {
        Destroy();
        return false;
    }
    m_pPlayerContext->pPlayerControl->SetStatusListener( this );
    if ( !RegisterAll( m_pPlayerContext->builder, m_pPlayerContext ) )
    {
        Destroy();
        return false;
    }
    if ( !pipeline_builder_rebuild_procedure_graph( m_pPlayerContext->builder ) )
    {
        UnregisterAll( m_pPlayerContext->builder );
        Destroy();
        return false;
    }
    m_pPlayerContext->pGraphicContext = config.pGraphicContext;
    m_pPlayerContext->pResourceManager = config.resource;
    m_pPlayerContext->pAudioContext = config.pAudioContext;
    Graphic::ViewPort viewPort;
    m_pPlayerContext->pGraphicContext->GetViewPort( viewPort );
    m_pPlayerContext->mediaInfo.displayWidth = viewPort.uWidth;
    m_pPlayerContext->mediaInfo.displayHeight = viewPort.uHeight;
    return true;
}

void VideoPlayerInternal::Destroy()
{
    if ( NULL != m_pPlayerContext )
    {
        Stop();
        if ( NULL != m_pPlayerContext->pPlayerControl )
        {
            delete m_pPlayerContext->pPlayerControl;
            m_pPlayerContext->pPlayerControl = NULL;
        }
        pipeline_destroy( &m_pPlayerContext->play_pipeline );
        hash_map_destroy( &m_pPlayerContext->procedure_types );
        if ( NULL != m_pPlayerContext->builder )
        {
            pipeline_builder_destroy( m_pPlayerContext->builder );
        }
        task_processor_thread_pool_destroy( &m_pPlayerContext->processor_thread_pool );
        delete m_pPlayerContext;
        m_pPlayerContext = NULL;
    }
    if ( NULL != m_config.resource )
    {
        Graphic::UtilGraphicResourceController* utilResController = (Graphic::UtilGraphicResourceController*)m_config.resource->GetController( Graphic::GraphicResourceController::GRCID_UTIL );
        if ( NULL != utilResController )
        {
            m_config.resource->UnregisterController( Graphic::GraphicResourceController::GRCID_UTIL );
            delete utilResController;
        }
    }
    memset( &m_config, 0, sizeof(VideoPlayerConfig) );
}

bool VideoPlayerInternal::PrepareToPlay()
{
    pipeline_procedure_desc* desc = RegisterProcedure( m_pPlayerContext->builder, FFMpegStream::_GetDesc(), m_pPlayerContext );
    if ( NULL == desc )
    {
        return false;
    }
    pipeline_procedure* src = pipeline_add_src( &m_pPlayerContext->play_pipeline, desc );
    if ( NULL == src )
    {
        UnregisterProcedure( m_pPlayerContext->builder, FFMpegStream::_GetDesc() );
        return false;
    }
    m_pDataSource = (PlayerDataSource*)src->config.context;
    if ( !m_pDataSource->SetURL( m_url.c_str() ) )
    {
        pipeline_clear( &m_pPlayerContext->play_pipeline );
        UnregisterProcedure( m_pPlayerContext->builder, FFMpegStream::_GetDesc() );
        return false;
    }
    if ( !pipeline_builder_build_pipeline( m_pPlayerContext->builder, &m_pPlayerContext->play_pipeline ) )
    {
        pipeline_clear( &m_pPlayerContext->play_pipeline );
        UnregisterProcedure( m_pPlayerContext->builder, FFMpegStream::_GetDesc() );
        return false;
    }
    return true;
}

void VideoPlayerInternal::Start()
{
    pipeline_start( &m_pPlayerContext->play_pipeline );
    m_pPlayerContext->pPlayerControl->Start();
}

void VideoPlayerInternal::Stop()
{
    m_pPlayerContext->pPlayerControl->Stop();
    pipeline_stop( &m_pPlayerContext->play_pipeline );
    Reset();
}

void VideoPlayerInternal::Pause()
{
    m_pPlayerContext->pPlayerControl->Pause();
}

int64_t VideoPlayerInternal::GetProgress()
{
    return m_pPlayerContext->pPlayerControl->GetProgress();
}

int64_t VideoPlayerInternal::GetDuration()
{
    return m_pPlayerContext->pPlayerControl->GetDuration();
}

bool VideoPlayerInternal::Seek( int64_t position )
{
    return m_pDataSource->Seek( position );
}

void VideoPlayerInternal::SetDisplaySize( float width, float height )
{
    m_pPlayerContext->mediaInfo.displayWidth = width;
    m_pPlayerContext->mediaInfo.displayHeight = height;
}

void VideoPlayerInternal::OnStatusChanged( PlayerControl::Status status )
{
    switch ( status )
    {
        case PlayerControl::Status_Stop:
            if ( NULL != m_config.pEventListener )
            {
                m_config.pEventListener->VideoPlayerStatusChanged( VideoPlayerEventListener::VideoPlayerStatus_Stop );
            }
            m_pPlayerContext->pPlayerControl->Stop();
            pipeline_stop( &m_pPlayerContext->play_pipeline );
            break;
        case PlayerControl::Status_Playing:
            if ( NULL != m_config.pEventListener )
            {
                m_config.pEventListener->VideoPlayerStatusChanged( VideoPlayerEventListener::VideoPlayerStatus_Playing );
            }
            break;
        case PlayerControl::Status_Pending:
            if ( NULL != m_config.pEventListener )
            {
                m_config.pEventListener->VideoPlayerStatusChanged( VideoPlayerEventListener::VideoPlayerStatus_Loading );
            }
            break;
        case PlayerControl::Status_Pause:
            break;
    }
}

void VideoPlayerInternal::Reset()
{
    pipeline_clear( &m_pPlayerContext->play_pipeline );
    UnregisterProcedure( m_pPlayerContext->builder, FFMpegStream::_GetDesc() );
    m_pDataSource = NULL;
}
