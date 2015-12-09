//
//  FFMpegAudioDecoder.cpp
//  VideoPlayer
//
//  Created by Yi Huang on 24/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "FFMpegAudioDecoder.h"

FFMpegAudioDecoder::FFMpegAudioDecoder( PlayerContext* context )
    : PlayerFilter( context )
    , m_pFrame( NULL )
    , m_swrContext( NULL )
{
    
}

FFMpegAudioDecoder::~FFMpegAudioDecoder()
{
    
}

bool FFMpegAudioDecoder::Initialize()
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
    m_currentSampleRate = 0;
    m_currentSampleFormat = (AVSampleFormat)0;
    m_currentBufferSize = 0;
    return true;
}

void FFMpegAudioDecoder::Destroy()
{
    if ( NULL != m_swrContext )
    {
        swr_free( &m_swrContext );
        m_swrContext = NULL;
    }
    av_frame_free( &m_pFrame );
    PlayerFilter::Destroy();
}

bool FFMpegAudioDecoder::ProcessInput( const PlayerProcedureKey& key, void* data )
{
    FFMpegAudioPackage* package = (FFMpegAudioPackage*)data;
    int got_frame = 0;
    bool ret = (avcodec_decode_audio4( package->codecContext, m_pFrame, &got_frame, &package->packet ) >= 0);
    av_free_packet( &package->packet );
    if ( got_frame )
    {
        AVRational timebase = (AVRational){1, m_pFrame->sample_rate};
        if ( m_pFrame->pts != AV_NOPTS_VALUE )
        {
            m_pFrame->pts = av_rescale_q( m_pFrame->pts, package->codecContext->time_base, timebase );
        }
        else if ( m_pFrame->pkt_pts != AV_NOPTS_VALUE )
        {
            m_pFrame->pts = av_rescale_q( m_pFrame->pkt_pts, av_codec_get_pkt_timebase( package->codecContext ), timebase );
        }
        else if ( m_nextPTS != AV_NOPTS_VALUE )
        {
            m_pFrame->pts = av_rescale_q( m_nextPTS, m_nextTimeBase, timebase );
        }
        if ( m_pFrame->pts != AV_NOPTS_VALUE )
        {
            m_nextPTS = m_pFrame->pts + m_pFrame->nb_samples;
            m_nextTimeBase = timebase;
        }
        int64_t layout = av_get_default_channel_layout( av_frame_get_channels(m_pFrame) );
        if ( m_currentSampleFormat != m_pFrame->format || m_currentSampleRate != m_pFrame->sample_rate )
        {
            m_swrContext = swr_alloc_set_opts( m_swrContext, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, m_pFrame->sample_rate, layout, (AVSampleFormat)m_pFrame->format, m_pFrame->sample_rate, 0, NULL );
            if ( NULL != m_swrContext )
            {
                if ( swr_init( m_swrContext ) < 0 )
                {
                    swr_free( &m_swrContext );
                }
                else
                {
                    m_currentSampleFormat = (AVSampleFormat)m_pFrame->format;
                    m_currentSampleRate = m_pFrame->sample_rate;
                }
            }
        }
        if ( NULL != m_swrContext )
        {
            int size = av_samples_get_buffer_size( NULL, av_frame_get_channels(m_pFrame), m_pFrame->nb_samples, AV_SAMPLE_FMT_S16, 0 );
            if ( size > 0 )
            {
                size_t bufferSize = (sizeof(AudioFragment) + size);
                if ( bufferSize > m_currentBufferSize )
                {
                    m_currentBufferSize = bufferSize;
                }
                AudioFragment* audioData = (AudioFragment*)GetOutputBuffer( AUDIO_FRAGMENT, m_currentBufferSize );
                if ( NULL != audioData )
                {
                    audioData->time = (m_pFrame->pts == AV_NOPTS_VALUE) ? NAN : m_pFrame->pts * av_q2d(timebase) * 1000.f;
                    audioData->duration = av_q2d((AVRational){m_pFrame->nb_samples, m_pFrame->sample_rate}) * 1000.f;
                    audioData->data = audioData + 1;
                    audioData->fraquency = m_pFrame->sample_rate;
                    uint8_t* data[] = { (uint8_t*)audioData->data };

                    // swr_convert sometimes may cause memory issue
                    int len = swr_convert( m_swrContext, data, m_pFrame->nb_samples, (const uint8_t **)m_pFrame->extended_data, m_pFrame->nb_samples );
                    if ( len > 0 )
                    {
                        audioData->size = len * av_frame_get_channels(m_pFrame) * av_get_bytes_per_sample( AV_SAMPLE_FMT_S16 );
                        CommitOutputBuffer( AUDIO_FRAGMENT );
                    }
                }
            }
        }
        av_frame_unref( m_pFrame );
    }
    return ret;
}
