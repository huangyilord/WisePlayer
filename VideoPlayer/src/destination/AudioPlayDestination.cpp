//
//  AudioPlayDestination.cpp
//  VideoPlayer
//
//  Created by Yi Huang on 24/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "AudioPlayDestination.h"

#include "PlayerControl.h"

#include "common_debug.h"

#define AUDIO_DELAY_TOLERANCE   1000         // milliseconds

typedef struct AudioBuffer
{
    int64                           startTime;
    int64                           duration;
    Audio::AUDIO_BUFFER_HANDLER     buffer;
} AudioBuffer;

AudioPlayDestination::AudioPlayDestination( PlayerContext* context )
    : PlayerDestination( context )
    , m_source( NULL )
{
    memset( &m_memoryPool, 0, sizeof(memory_pool) );
    memset( &m_bufferQueue, 0, sizeof(list) );
}

AudioPlayDestination::~AudioPlayDestination()
{
}

bool AudioPlayDestination::Initialize()
{
    if ( !PlayerDestination::Initialize() )
    {
        return false;
    }
    m_source = m_pPlayerContext->pAudioContext->CreateSource( Audio::IAudioContext::SourceType_Stream );
    if ( NULL == m_source )
    {
        Destroy();
        return false;
    }
    memory_pool_initialize( &m_memoryPool, 1024, 4 );
    if ( !list_initialize( &m_bufferQueue, sizeof(AudioBuffer), &m_memoryPool ) )
    {
        Destroy();
        return false;
    }
    return true;
}

void AudioPlayDestination::Destroy()
{
    list_destroy( &m_bufferQueue );
    memory_pool_destroy( &m_memoryPool );
    if ( NULL != m_source )
    {
        m_pPlayerContext->pAudioContext->DestroySource( m_source );
        m_source = NULL;
    }
    PlayerDestination::Destroy();
}

bool AudioPlayDestination::ProcessInput( const PlayerProcedureKey& key, void* data )
{
    AudioFragment* audioFragment = (AudioFragment*)data;
    AudioBuffer buffer = { 0 };
    buffer.buffer = m_pPlayerContext->pAudioContext->CreateBuffer();
    m_pPlayerContext->pAudioContext->BufferData( buffer.buffer, Audio::IAudioContext::DataFormat_STEREO16, audioFragment->data, (UINT32)audioFragment->size, audioFragment->fraquency );
    buffer.startTime = audioFragment->time;
    buffer.duration = audioFragment->duration;
    if ( !list_push_back( &m_bufferQueue, &buffer ) )
    {
        m_pPlayerContext->pAudioContext->DestroyBuffer( buffer.buffer );
        return false;
    }
    return true;
}

bool AudioPlayDestination::UpdateContent( const PlayerDestinationContext& context )
{
    Audio::IAudioContext* pAudioContext = m_pPlayerContext->pAudioContext;
    pAudioContext->EnsureThreadContext();
    while ( TryProcessAnyInput() );
    // audio doesn't block progress
    int64 progress = m_pPlayerContext->pPlayerControl->GetProgress();
    SetProgress( progress );

    bool isPlaying = m_pPlayerContext->pAudioContext->SourceIsPlaying( m_source );
    uint32 processedBufferCount = m_pPlayerContext->pAudioContext->GetSourceProcessedBufferCount( m_source );
    while ( processedBufferCount-- )
    {
        Audio::AUDIO_BUFFER_HANDLER buffer = m_pPlayerContext->pAudioContext->SourceDequeueBuffer( m_source );
        ASSERT( NULL != buffer );
        m_pPlayerContext->pAudioContext->DestroyBuffer( buffer );
    }
    if ( isPlaying )
    {
        // enque audio buffers
        while ( !LIST_IS_EMPTY( &m_bufferQueue ) )
        {
            AudioBuffer* buffer = (AudioBuffer*)list_front( &m_bufferQueue );
            // skip audio piece if the progress is slower
            if ( buffer->startTime >= progress )
            {
                if ( buffer->startTime < (progress + AUDIO_DELAY_TOLERANCE) )
                {
                    m_pPlayerContext->pAudioContext->SourceEnqueueBuffer( m_source, buffer->buffer );
                }
                else
                {
                    break;
                }
            }
            list_pop_front( &m_bufferQueue );
        }
    }
    else
    {
        int64 startTime = -1;
        if ( !LIST_IS_EMPTY( &m_bufferQueue ) )
        {
            AudioBuffer* buffer = (AudioBuffer*)list_front( &m_bufferQueue );
            startTime = buffer->startTime;
        }
        // wait until the progress reach the start time to start play
        if ( startTime >= 0 && progress >= startTime )
        {
            while ( !LIST_IS_EMPTY( &m_bufferQueue ) )
            {
                AudioBuffer* buffer = (AudioBuffer*)list_front( &m_bufferQueue );
                if ( buffer->startTime >= progress )
                {
                    if ( buffer->startTime < (progress + AUDIO_DELAY_TOLERANCE) )
                    {
                        m_pPlayerContext->pAudioContext->SourceEnqueueBuffer( m_source, buffer->buffer );
                    }
                    else
                    {
                        break;
                    }
                }
                list_pop_front( &m_bufferQueue );
            }
            m_pPlayerContext->pAudioContext->PlaySource( m_source );
            return true;
        }
        return false;
    }
    return true;
}