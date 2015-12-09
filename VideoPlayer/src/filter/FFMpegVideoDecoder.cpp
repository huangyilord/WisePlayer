//
//  FFMpegVideoDecoder.cpp
//  VideoPlayer
//
//  Created by Yi Huang on 23/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "FFMpegVideoDecoder.h"

FFMpegVideoDecoder::FFMpegVideoDecoder( PlayerContext* context )
    : PlayerFilter( context )
    , m_pFrame( NULL )
    , m_pImageConvertContext( NULL )
    , m_textureSize( 1 )
{
    
}

FFMpegVideoDecoder::~FFMpegVideoDecoder()
{
    
}

bool FFMpegVideoDecoder::Initialize()
{
    if ( !PlayerFilter::Initialize() )
    {
        Destroy();
        return false;
    }
    m_pFrame = av_frame_alloc();
    if ( NULL == m_pFrame )
    {
        Destroy();
        return false;
    }
    m_lastPts = 0;
    m_lastTimeStamp = 0;
    return true;
}

void FFMpegVideoDecoder::Destroy()
{
    av_frame_free( &m_pFrame );
    if ( NULL != m_pImageConvertContext )
    {
        sws_freeContext( m_pImageConvertContext );
        m_pImageConvertContext = NULL;
    }
    PlayerFilter::Destroy();
}

bool FFMpegVideoDecoder::ProcessInput( const PlayerProcedureKey&, void* data )
{
    FFMpegVideoPackage* package = (FFMpegVideoPackage*)data;
    int got_frame = 0;
    bool ret = (avcodec_decode_video2( package->codecContext, m_pFrame, &got_frame, &package->packet ) >= 0);
    av_free_packet( &package->packet );
    if ( got_frame )
    {
        uint32 size = GetTextureSize( m_pFrame->width, m_pFrame->height );
        m_pPlayerContext->mediaInfo.video.frameWidth = m_pFrame->width;
        m_pPlayerContext->mediaInfo.video.frameHeight = m_pFrame->height;
        m_pImageConvertContext = sws_getCachedContext( m_pImageConvertContext,
                                                      m_pFrame->width, m_pFrame->height, (AVPixelFormat)m_pFrame->format, size, size,
                                                      AV_PIX_FMT_RGBA, SWS_BILINEAR, NULL, NULL, NULL );
        if ( NULL == m_pImageConvertContext )
        {
            av_frame_unref( m_pFrame );
            return false;
        }
        RGBATexture* texture = (RGBATexture*)GetOutputBuffer( STRETCHED_RGBA_TEXTURE, sizeof(RGBATexture) + size * size * 4 );
        if ( NULL != texture )
        {
            AVPicture pict = { { 0 } };
            texture->pixelData = (uint8*)texture + sizeof(RGBATexture);
            texture->width = size;
            texture->pixelCount = size * size;
            int64_t pts = av_frame_get_best_effort_timestamp( m_pFrame );
            m_lastTimeStamp += (int64)((float32)(pts - m_lastPts) * (float32)m_pPlayerContext->mediaInfo.video.frameNum / (float32)m_pPlayerContext->mediaInfo.video.fraquency * 1000.f);
            m_lastPts = pts;
            if ( m_lastTimeStamp < 0 )
            {
                m_lastTimeStamp = 0;
            }
            texture->time = m_lastTimeStamp;
            pict.data[0] = (uint8_t*)texture->pixelData;
            pict.linesize[0] = texture->width * 4;
            sws_scale( m_pImageConvertContext, m_pFrame->data, m_pFrame->linesize,
                      0, m_pFrame->height, pict.data, pict.linesize );
            CommitOutputBuffer( STRETCHED_RGBA_TEXTURE );
        }
        av_frame_unref( m_pFrame );
    }
    return ret;
}

uint32 FFMpegVideoDecoder::GetTextureSize( uint32 width, uint32 height )
{
    while ( m_textureSize < width )
    {
        m_textureSize = m_textureSize << 1;
    }
    while ( m_textureSize < height )
    {
        m_textureSize = m_textureSize << 1;
    }
    if ( m_textureSize > 2048 )
    {
        m_textureSize = 2048;
    }
    return m_textureSize;
}
