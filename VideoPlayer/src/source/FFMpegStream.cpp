//
//  FFMpegStream.cpp
//  VideoPlayer
//
//  Created by Yi Huang on 12/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "FFMpegStream.h"
#include "PlayerControl.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "atomic.h"
}

FFMpegStream::FFMpegStream( PlayerContext* context )
    : PlayerDataSource( context )
    , m_pFormat( NULL )
    , m_pFormatContext( NULL )
    , m_pOptions( NULL )
    , m_pVideoCodecContext( NULL )
    , m_pVideoCodec( NULL )
    , m_pAudioCodecContext( NULL )
    , m_pAudioCodec( NULL )
    , m_videoStreamNumber( 0 )
    , m_audioStreamNumber( 0 )
    , m_pSeekRequest( &m_seekRequest )
{
}

FFMpegStream::~FFMpegStream()
{
    
}

bool FFMpegStream::Initialize()
{
    if ( !PlayerProcedure::Initialize() )
    {
        return false;
    }
    av_register_all();
    avformat_network_init();
    m_pSeekRequest = &m_seekRequest;
    m_seekRequest.isValid = false;
    return true;
}

void FFMpegStream::Destroy()
{
    CloseFFStream();
    PlayerProcedure::Destroy();
    avformat_network_deinit();
}


bool FFMpegStream::SetURL( const char* url )
{
    if ( !OpenFFStream( url ) )
    {
        return false;
    }
    return true;
}

bool FFMpegStream::Seek( int64 position )
{
    FFMpegSeekRequest* pRequest = NULL;
    while ( NULL == pRequest )
    {
        pRequest = (FFMpegSeekRequest*)atomic_exchange_ptr( (void* volatile*)&m_pSeekRequest, pRequest );
    }
    pRequest->position = position;
    pRequest->isValid = true;
    (void)atomic_exchange_ptr( (void* volatile*)&m_pSeekRequest, pRequest );
    return true;
}

void FFMpegStream::OnStart()
{
}

void FFMpegStream::OnStop()
{
    
}

bool FFMpegStream::GetData()
{
    FFMpegSeekRequest* pRequest = NULL;
    pRequest = (FFMpegSeekRequest*)atomic_exchange_ptr( (void* volatile*)&m_pSeekRequest, pRequest );
    if ( pRequest )
    {
        // process seek operation
        if ( pRequest->isValid )
        {
            pRequest->isValid = false;
            int flag = AVSEEK_FLAG_ANY;
            int64 currentPosition = m_pPlayerContext->pPlayerControl->GetProgress();
            if ( currentPosition > pRequest->position )
            {
                flag |= AVSEEK_FLAG_BACKWARD;
            }
            int64_t time = pRequest->position * AV_TIME_BASE / 1000.f;
            if ( av_seek_frame( m_pFormatContext, -1, time, flag ) >= 0 )
            {
                m_pPlayerContext->pPlayerControl->SetProgress( pRequest->position );
            }
        }
        (void)atomic_exchange_ptr( (void* volatile*)&m_pSeekRequest, pRequest );
    }
    if ( av_read_frame( m_pFormatContext, &m_tempPacket ) < 0 )
    {
        return false;
    }
    if ( m_tempPacket.stream_index == m_videoStreamNumber )
    {
        FFMpegVideoPackage* package = (FFMpegVideoPackage*)GetOutputBuffer( FFMPEG_VIDEO_PACKAGE, sizeof(FFMpegVideoPackage) );
        if ( NULL != package )
        {
            package->codecContext = m_pVideoCodecContext;
            av_packet_move_ref( &package->packet, &m_tempPacket );
            CommitOutputBuffer( FFMPEG_VIDEO_PACKAGE );
        }
    }
//    else if ( m_tempPacket.stream_index == m_audioStreamNumber )
//    {
//        FFMpegAudioPackage* package = (FFMpegAudioPackage*)GetOutputBuffer( FFMPEG_AUDIO_PACKAGE, sizeof(FFMpegAudioPackage) );
//        if ( NULL != package )
//        {
//            av_packet_move_ref( &package->packet, &m_tempPacket );
//            package->codecContext = m_pAudioCodecContext;
//            CommitOutputBuffer( FFMPEG_AUDIO_PACKAGE );
//        }
//    }
    av_free_packet( &m_tempPacket );
    return true;
}

bool FFMpegStream::OpenFFStream( const char* filePath )
{
    m_pFormatContext = NULL;
    if ( avformat_open_input( &m_pFormatContext, filePath, m_pFormat, &m_pOptions ) < 0 )
    {
        CloseFFStream();
        return false;
    }
    if ( avformat_find_stream_info( m_pFormatContext, &m_pOptions ) < 0 )
    {
        CloseFFStream();
        return false;
    }
    for ( int i = 0; i < m_pFormatContext->nb_streams; ++i )
    {
        AVStream* stream = m_pFormatContext->streams[i];
        switch ( stream->codec->codec_type )
        {
            case AVMEDIA_TYPE_VIDEO:
                m_videoStreamNumber = i;
                m_pPlayerContext->pPlayerControl->SetDuration( stream->duration * av_q2d(stream->time_base) * 1000 );
                break;
            case AVMEDIA_TYPE_AUDIO:
                m_audioStreamNumber = i;
                break;
            default:
                break;
        }
    }
    m_videoStreamNumber = av_find_best_stream( m_pFormatContext, AVMEDIA_TYPE_VIDEO, m_videoStreamNumber, -1, NULL, 0 );
    if ( m_videoStreamNumber < 0 )
    {
        CloseFFStream();
        return false;
    }
    AVStream* stream = m_pFormatContext->streams[m_videoStreamNumber];
    m_pVideoCodecContext = stream->codec;
    m_pPlayerContext->mediaInfo.video.frameNum = stream->time_base.num;
    m_pPlayerContext->mediaInfo.video.fraquency = stream->time_base.den;
    m_pVideoCodec = avcodec_find_decoder( m_pVideoCodecContext->codec_id );
    if ( NULL == m_pVideoCodec )
    {
        CloseFFStream();
        return false;
    }
    if ( avcodec_open2( m_pVideoCodecContext, m_pVideoCodec, NULL ) < 0 )
    {
        CloseFFStream();
        return false;
    }

    m_audioStreamNumber = av_find_best_stream( m_pFormatContext, AVMEDIA_TYPE_AUDIO, m_audioStreamNumber, m_videoStreamNumber, NULL, 0 );
    if ( m_audioStreamNumber >= 0 )
    {
        stream = m_pFormatContext->streams[m_audioStreamNumber];
        m_pAudioCodecContext = stream->codec;
        m_pAudioCodec = avcodec_find_decoder( m_pAudioCodecContext->codec_id );
        if ( NULL == m_pAudioCodec )
        {
            CloseFFStream();
            return false;
        }
        if ( avcodec_open2( m_pAudioCodecContext, m_pAudioCodec, NULL ) < 0 )
        {
            CloseFFStream();
            return false;
        }
    }
    return true;
}

void FFMpegStream::CloseFFStream()
{
    avcodec_close( m_pAudioCodecContext );
    avcodec_close( m_pVideoCodecContext );
    if ( NULL != m_pFormatContext )
    {
        avformat_close_input( &m_pFormatContext );
        avformat_free_context( m_pFormatContext );
        m_pFormatContext = NULL;
    }
}
