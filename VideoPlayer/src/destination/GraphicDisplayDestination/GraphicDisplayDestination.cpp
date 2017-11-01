//
//  PlayerDestination.cpp
//  WisePlayer
//
//  Created by Yi Huang on 1/6/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#include "GraphicDisplayDestination.h"

#include "GraphicMath.h"
#include "PlayerControl.h"

#include "Effects/EffectManager.h"

extern "C"
{
#include "atomic.h"
}

#define DISPLAY_TIME_TOLERANCE      2000

GraphicDisplayDestination::DisplayShader::DisplayShader()
    : m_program( NULL )
{
}

GraphicDisplayDestination::DisplayShader::~DisplayShader()
{
}

bool GraphicDisplayDestination::DisplayShader::Initialize( Graphic::IGraphicContext* context )
{
    Graphic::MatrixIdentity( &m_worldTransform );
    Graphic::MatrixIdentity( &m_viewTransform );
    Graphic::MatrixIdentity( &m_projectionTransform );
    Graphic::MatrixIdentity( &m_textureTransform );
    m_program = context->GetShaderProgram();
    m_transformConstantLocation = context->GetVertexShaderConstantLocation( m_program, "u_transform" );
    m_textureTransformConstantLocation = context->GetVertexShaderConstantLocation( m_program, "u_texTransform" );
    return true;
}

void GraphicDisplayDestination::DisplayShader::Destroy()
{
    m_program = NULL;
}

VOID GraphicDisplayDestination::DisplayShader::SetTextureTransform( UINT32 stage, const Graphic::Matrix16& mtrx )
{
    m_textureTransform = mtrx;
}

VOID GraphicDisplayDestination::DisplayShader::SetWorldTransform( const Graphic::Matrix16& mtrx )
{
    m_worldTransform = mtrx;
}

VOID GraphicDisplayDestination::DisplayShader::SetViewTransform( const Graphic::Matrix16& mtrx )
{
    m_viewTransform = mtrx;
}

VOID GraphicDisplayDestination::DisplayShader::SetProjectionTransform( const Graphic::Matrix16& mtrx )
{
    m_projectionTransform = mtrx;
}

VOID GraphicDisplayDestination::DisplayShader::CommitChanges( Graphic::IGraphicContext* pGraphicContext )
{
    pGraphicContext->SetVertexShaderConstantMatrix( m_textureTransformConstantLocation, &m_textureTransform );
    Graphic::Matrix16 mtrx;
    Graphic::MatrixMultiply( &mtrx, &m_worldTransform, &m_viewTransform );
    Graphic::MatrixMultiply( &mtrx, &mtrx, &m_projectionTransform );
    pGraphicContext->SetVertexShaderConstantMatrix( m_transformConstantLocation, &mtrx );
}

Graphic::SHADER_PROGRAM_HANDLE GraphicDisplayDestination::DisplayShader::GetProgram()
{
    return m_program;
}

GraphicDisplayDestination::DisplayGeometry::DisplayGeometry()
{
    m_target.origin.x = SCREEN_COORD_X_MIN;
    m_target.origin.y = SCREEN_COORD_Y_MIN;
    m_target.size.x = (SCREEN_COORD_X_MAX - SCREEN_COORD_X_MIN);
    m_target.size.y = (SCREEN_COORD_Y_MAX - SCREEN_COORD_Y_MIN);
    m_textureInfo.texture = NULL;
    m_textureInfo.texCoord.origin.x = 0.f;
    m_textureInfo.texCoord.origin.y = 0.f;
    m_textureInfo.texCoord.size.x = 1.f;
    m_textureInfo.texCoord.size.y = 1.f;
    m_textureWidth = 0;
    m_textureHeight = 0;
}

GraphicDisplayDestination::DisplayGeometry::~DisplayGeometry()
{
}

bool GraphicDisplayDestination::DisplayGeometry::Initialize( Graphic::IGraphicContext* context )
{
    if ( !m_shader.Initialize( context ) )
    {
        return false;
    }
    return true;
}

void GraphicDisplayDestination::DisplayGeometry::Destroy()
{
    m_shader.Destroy();
}

void GraphicDisplayDestination::DisplayGeometry::SetPos( FLOAT32 x, FLOAT32 y )
{
    m_target.origin.x = x;
    m_target.origin.y = y;
}

void GraphicDisplayDestination::DisplayGeometry::SetSize( FLOAT32 x, FLOAT32 y )
{
    m_target.size.x = x;
    m_target.size.y = y;
}

VOID GraphicDisplayDestination::DisplayGeometry::Render( Graphic::Render2D* render, const Graphic::RenderContext* context )
{
    render->SetShader( &m_shader );
    render->DrawTexture( m_target, m_textureInfo );
}


GraphicDisplayDestination::GraphicDisplayDestination( PlayerContext* context )
    : PlayerDestination( context )
    , m_pChangeEffectRequest( &m_changeEffectRequest )
    , m_pCurrentFilter( NULL )
{
}

GraphicDisplayDestination::~GraphicDisplayDestination()
{
    Destroy();
}

bool GraphicDisplayDestination::Initialize()
{
    if ( !PlayerDestination::Initialize() )
    {
        return false;
    }
    if ( !m_pPlayerContext->pPlayerControl->RegisterDestination( GetDesc().key ) )
    {
        PlayerDestination::Destroy();
        return false;
    }
    if ( !m_graphicManager.Initialize() )
    {
        Destroy();
        return false;
    }
    m_pPlayerContext->pGraphicContext->EnsureThreadContext();
    if ( !m_geometry.Initialize( m_pPlayerContext->pGraphicContext ) )
    {
        Destroy();
        return false;
    }
    m_graphicManager.AddLayerTop( &m_scene );
    m_scene.AddBack( &m_geometry );
    Graphic::UtilGraphicResourceController* resController = (Graphic::UtilGraphicResourceController*)m_pPlayerContext->pResourceManager->GetController( Graphic::GraphicResourceController::GRCID_UTIL );
    m_scene.SetResController( resController );
    m_graphicManager.SetResController( resController );
    m_pPlayerContext->pDisplayDestination = this;
    m_changeEffectRequest.isValid = FALSE;
    return true;
}

void GraphicDisplayDestination::Destroy()
{
    m_pPlayerContext->pGraphicContext->EnsureThreadContext();
    if ( NULL != m_pCurrentFilter )
    {
        EffectManager::ReleaseEffect( m_pCurrentFilter, m_pPlayerContext );
        m_pPlayerContext = NULL;
        m_graphicManager.ClearFilter();
    }
    m_geometry.Destroy();
    m_scene.Clear();
    m_graphicManager.ClearLayer();
    m_pPlayerContext->pPlayerControl->UnregisterDestination( GetDesc().key );
    PlayerDestination::Destroy();
}

VOID GraphicDisplayDestination::SetEffect( VideoPlayerEffect type )
{
    ChangeEffectRequest* pRequest = NULL;
    while ( NULL == pRequest )
    {
        pRequest = (ChangeEffectRequest*)atomic_exchange_ptr( (void* volatile*)&m_pChangeEffectRequest, pRequest );
    }
    pRequest->effectType = type;
    pRequest->isValid = true;
    m_pChangeEffectRequest = pRequest;
}

bool GraphicDisplayDestination::ProcessInput( const PlayerProcedureKey& key, void* data )
{
    m_pPlayerContext->pGraphicContext->EnsureThreadContext();
    RGBATexture* textureData = (RGBATexture*)data;
    SetProgress( textureData->time );
    int64 globalProgress = m_pPlayerContext->pPlayerControl->GetProgress();
    int64 diffProgress = GetProgress() - globalProgress;
    if ( diffProgress > DISPLAY_TIME_TOLERANCE || diffProgress < -DISPLAY_TIME_TOLERANCE )
    {
        // skip
        return true;
    }
    if ( diffProgress < 0 )
    {
        uint32 width = textureData->width;
        uint32 height = textureData->pixelCount / textureData->width;
        if ( width != m_geometry.m_textureWidth || height != m_geometry.m_textureHeight )
        {
            if ( NULL != m_geometry.m_textureInfo.texture )
            {
                m_pPlayerContext->pGraphicContext->DestroyTexture( m_geometry.m_textureInfo.texture );
                m_geometry.m_textureInfo.texture = NULL;
            }
            m_geometry.m_textureInfo.texture = m_pPlayerContext->pGraphicContext->CreateTexture( textureData->width, textureData->pixelCount / textureData->width );
            m_geometry.m_textureWidth = width;
            m_geometry.m_textureHeight = height;
        }
        m_pPlayerContext->pGraphicContext->TextureSubImage( m_geometry.m_textureInfo.texture, textureData->pixelData, textureData->width, textureData->pixelCount / textureData->width, textureData->x, textureData->y );
        float32 displayRatio = (float32)m_pPlayerContext->mediaInfo.displayHeight / (float32)m_pPlayerContext->mediaInfo.displayWidth;
        float32 frameRatio = (float32)m_pPlayerContext->mediaInfo.video.frameHeight / (float32)m_pPlayerContext->mediaInfo.video.frameWidth;
        float32 xSize = SCREEN_COORD_X_MAX - SCREEN_COORD_X_MIN;
        float32 ySize = SCREEN_COORD_Y_MAX - SCREEN_COORD_Y_MIN;
        Graphic::ViewPort viewPort;
        m_pPlayerContext->pGraphicContext->GetViewPort( viewPort );
        if ( displayRatio > frameRatio )
        {
            m_geometry.SetSize( xSize, ySize * frameRatio / displayRatio );
            m_geometry.SetPos( SCREEN_COORD_X_MIN, SCREEN_COORD_Y_MIN + ( ySize - ySize * frameRatio / displayRatio ) / 2.f );
        }
        else
        {
            m_geometry.SetSize( xSize * displayRatio / frameRatio, ySize );
            m_geometry.SetPos( SCREEN_COORD_X_MIN + ( xSize - xSize * displayRatio / frameRatio ) / 2.f, SCREEN_COORD_Y_MIN );
        }
        return true;
    }
    return false;
}

bool GraphicDisplayDestination::UpdateContent( const PlayerDestinationContext& context )
{
    bool ret = ProcessAnyInput();
    if ( ret )
    {
        ChangeEffectRequest* pRequest = NULL;
        pRequest = (ChangeEffectRequest*)atomic_exchange_ptr( (void* volatile*)&m_pChangeEffectRequest, pRequest );
        if ( pRequest )
        {
            // process seek operation
            if ( pRequest->isValid )
            {
                pRequest->isValid = false;
                if ( NULL != m_pCurrentFilter )
                {
                    EffectManager::ReleaseEffect( m_pCurrentFilter, m_pPlayerContext );
                    m_graphicManager.ClearFilter();
                    m_pCurrentFilter = NULL;
                }
                if ( VideoPlayerEffect_None != pRequest->effectType )
                {
                    static EffectManager::EffectType s_effectTypes[] = { EffectManager::EffectType_Mosaic
                        , EffectManager::EffectType_Mosaic
                        , EffectManager::EffectType_Fade
                        , EffectManager::EffectType_Blur
                        , EffectManager::EffectType_Sketch
                        , EffectManager::EffectType_Sharpening
                        , EffectManager::EffectType_HDR };
                    m_pCurrentFilter = EffectManager::CreateEffect( s_effectTypes[pRequest->effectType], m_pPlayerContext );
                    if ( NULL != m_pCurrentFilter )
                    {
                        m_graphicManager.AddFilter( m_pCurrentFilter );
                    }
                }
            }
            (void)atomic_exchange_ptr( (void* volatile*)&m_pChangeEffectRequest, pRequest );
        }
        Graphic::ColorF_RGBA clearColor = { 0.f, 0.f, 0.f, 1.f };
        m_pPlayerContext->pGraphicContext->Clear( Graphic::IGraphicContext::Clear_Color, clearColor, 0.f, 0 );
        m_graphicManager.Render( m_pPlayerContext->pGraphicContext, context.timeInterval );
        m_pPlayerContext->pGraphicContext->Present();
    }
    return ret;
}