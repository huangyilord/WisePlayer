//
//  OpenALAudioContext.cpp
//  Audio
//
//  Created by Yi Huang on 3/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "OpenALAudioContext.h"

#include "OpenALInternalTypes.h"

#include <new>

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

namespace Audio
{
    OpenALAudioContext::OpenALAudioContext()
        : m_device( NULL )
        , m_context( NULL )
    {
        
    }

    OpenALAudioContext::~OpenALAudioContext()
    {
        Destroy();
    }

    BOOL OpenALAudioContext::Initialize()
    {
        m_device = alcOpenDevice( NULL );
        if ( NULL == m_device )
        {
            return FALSE;
        }
        m_context = alcCreateContext( (ALCdevice*)m_device, NULL );
        if ( NULL == m_context )
        {
            Destroy();
            return FALSE;
        }
        alcMakeContextCurrent( (ALCcontext*)m_context );
        alSpeedOfSound( 1.f );
        return TRUE;
    }

    VOID OpenALAudioContext::Destroy()
    {
        if ( NULL != m_context)
        {
            alcDestroyContext( (ALCcontext*)m_context );
            m_context = NULL;
        }
        if ( NULL != m_device )
        {
            alcCloseDevice( (ALCdevice*)m_device );
            m_device = NULL;
        }
    }

    AUDIO_BUFFER_HANDLER OpenALAudioContext::CreateBuffer()
    {
        UINT32 bufferID = 0;
        alGenBuffers( 1, &bufferID );
        OpenALBuffer* pBuffer = new(std::nothrow) OpenALBuffer( bufferID );
        if ( NULL == pBuffer )
        {
            alDeleteBuffers( 1, &bufferID );
            return NULL;
        }
        return pBuffer;
    }

    VOID OpenALAudioContext::DestroyBuffer( AUDIO_BUFFER_HANDLER buffer )
    {
        OpenALBuffer* pBuffer = (OpenALBuffer*)buffer;
        UINT32 bufferID = pBuffer->GetID();
        alDeleteBuffers( 1, &bufferID );
        delete pBuffer;
    }

    BOOL OpenALAudioContext::BufferData( AUDIO_BUFFER_HANDLER buffer, DataFormat format, const VOID* data, UINT32 size, UINT32 frequency )
    {
        OpenALBuffer* pBuffer = (OpenALBuffer*)buffer;
        static const ALenum s_formats[] = { AL_FORMAT_MONO8, AL_FORMAT_MONO16, AL_FORMAT_STEREO8, AL_FORMAT_STEREO16 };
        alBufferData( pBuffer->GetID(), s_formats[format], data, size, frequency );
        return TRUE;
    }

    AUDIO_SOURCE_HANDLER OpenALAudioContext::CreateSource( SourceType type )
    {
        UINT32 sourceID = 0;
        alGenSources( 1, &sourceID );
        OpenALSource* pSource = new(std::nothrow) OpenALSource( sourceID );
        if ( NULL == pSource )
        {
            alDeleteSources( 1, &sourceID );
            return NULL;
        }
        static const ALint s_types[] = { AL_STATIC, AL_STREAMING };
        alSourcef( sourceID, AL_SOURCE_TYPE, s_types[type] );
        alSourcef(sourceID, AL_PITCH, 1.f);
        return pSource;
    }

    VOID OpenALAudioContext::DestroySource( AUDIO_SOURCE_HANDLER source )
    {
        OpenALSource* pSource = (OpenALSource*)source;
        UINT32 sourceID = pSource->GetID();
        alDeleteSources( 1, &sourceID );
        delete pSource;
    }

    VOID OpenALAudioContext::SetSourcePos( AUDIO_SOURCE_HANDLER source, FLOAT32 x, FLOAT32 y, FLOAT32 z )
    {
        OpenALSource* pSource = (OpenALSource*)source;
        alSource3f( pSource->GetID(), AL_POSITION, x, y, z );
    }

    VOID OpenALAudioContext::SetSourceVelocity( AUDIO_SOURCE_HANDLER source, FLOAT32 x, FLOAT32 y, FLOAT32 z )
    {
        OpenALSource* pSource = (OpenALSource*)source;
        alSource3f( pSource->GetID(), AL_VELOCITY, x, y, z );
    }

    BOOL OpenALAudioContext::SourceEnqueueBuffer( AUDIO_SOURCE_HANDLER source, AUDIO_BUFFER_HANDLER buffer )
    {
        OpenALSource* pSource = (OpenALSource*)source;
        OpenALBuffer* pBuffer = (OpenALBuffer*)buffer;
        UINT32 bufferID = pBuffer->GetID();
        alSourceQueueBuffers( pSource->GetID(), 1, &bufferID );
        pSource->EnqueueBuffer( pBuffer );
        return TRUE;
    }

    AUDIO_BUFFER_HANDLER OpenALAudioContext::SourceDequeueBuffer( AUDIO_SOURCE_HANDLER source )
    {
        OpenALSource* pSource = (OpenALSource*)source;
        if ( pSource->IsEmpty() )
        {
            return NULL;
        }
        OpenALBuffer* pBuffer = pSource->FirstBuffer();
        UINT32 bufferID = pBuffer->GetID();
        alSourceUnqueueBuffers( pSource->GetID(), 1, &bufferID );
        pSource->DequeueBuffer();
        return pBuffer;
    }

    UINT32 OpenALAudioContext::GetSourceProcessedBufferCount( AUDIO_SOURCE_HANDLER source )
    {
        OpenALSource* pSource = (OpenALSource*)source;
        ALint processedBufferCount = 0;
        alGetSourcei( pSource->GetID(), AL_BUFFERS_PROCESSED, &processedBufferCount );
        return processedBufferCount;
    }

    UINT32 OpenALAudioContext::GetSourceQueuedBufferCount( AUDIO_SOURCE_HANDLER source )
    {
        OpenALSource* pSource = (OpenALSource*)source;
        ALint queuedBufferCount = 0;
        alGetSourcei( pSource->GetID(), AL_BUFFERS_QUEUED, &queuedBufferCount );
        return queuedBufferCount;
    }

    VOID OpenALAudioContext::PlaySource( AUDIO_SOURCE_HANDLER source )
    {
        OpenALSource* pSource = (OpenALSource*)source;
        alSourcePlay( pSource->GetID() );
    }

    VOID OpenALAudioContext::PauseSource( AUDIO_SOURCE_HANDLER source )
    {
        OpenALSource* pSource = (OpenALSource*)source;
        alSourcePause( pSource->GetID() );
    }

    VOID OpenALAudioContext::StopSource( AUDIO_SOURCE_HANDLER source )
    {
        OpenALSource* pSource = (OpenALSource*)source;
        alSourceStop( pSource->GetID() );
    }

    BOOL OpenALAudioContext::SourceIsPlaying( AUDIO_SOURCE_HANDLER source )
    {
        ALint state = 0;
        OpenALSource* pSource = (OpenALSource*)source;
        alGetSourcei( pSource->GetID(), AL_SOURCE_STATE, &state );
        return state == AL_PLAYING;
    }

    VOID OpenALAudioContext::SetListenerPos( FLOAT32 x, FLOAT32 y, FLOAT32 z )
    {
        alListener3f( AL_POSITION, x, y, z );
    }

    VOID OpenALAudioContext::SetListenerDirection( FLOAT32 x, FLOAT32 y, FLOAT32 z )
    {
        alListener3f( AL_DIRECTION, x, y, z );
    }

    VOID OpenALAudioContext::SetListenerVelocity( FLOAT32 x, FLOAT32 y, FLOAT32 z )
    {
        alListener3f( AL_VELOCITY, x, y, z );
    }

    VOID OpenALAudioContext::Release()
    {
        delete this;
    }

    VOID OpenALAudioContext::EnsureThreadContext()
    {
        alcMakeContextCurrent( (ALCcontext*)m_context );
    }
}
