//
//  OpenALAudioContext.h
//  Audio
//
//  Created by Yi Huang on 3/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _OPENAL_AUDIO_CONTEXT_H_ )
#define _OPENAL_AUDIO_CONTEXT_H_

#include "IAudioContext.h"

namespace Audio
{
    class OpenALAudioContext
        : public IAudioContext
    {
    public:
        OpenALAudioContext();
        virtual ~OpenALAudioContext();

        BOOL Initialize();
        VOID Destroy();

        virtual AUDIO_BUFFER_HANDLER CreateBuffer();
        virtual VOID DestroyBuffer( AUDIO_BUFFER_HANDLER buffer );
        virtual BOOL BufferData( AUDIO_BUFFER_HANDLER buffer, DataFormat format, const VOID* data, UINT32 size, UINT32 frequency );

        virtual AUDIO_SOURCE_HANDLER CreateSource( SourceType type );
        virtual VOID DestroySource( AUDIO_SOURCE_HANDLER source );
        virtual VOID SetSourcePos( AUDIO_SOURCE_HANDLER source, FLOAT32 x, FLOAT32 y, FLOAT32 z );
        virtual VOID SetSourceVelocity( AUDIO_SOURCE_HANDLER source, FLOAT32 x, FLOAT32 y, FLOAT32 z );
        virtual BOOL SourceEnqueueBuffer( AUDIO_SOURCE_HANDLER source, AUDIO_BUFFER_HANDLER buffer );
        virtual AUDIO_BUFFER_HANDLER SourceDequeueBuffer( AUDIO_SOURCE_HANDLER source );
        virtual UINT32 GetSourceProcessedBufferCount( AUDIO_SOURCE_HANDLER source );
        virtual UINT32 GetSourceQueuedBufferCount( AUDIO_SOURCE_HANDLER source );

        virtual VOID PlaySource( AUDIO_SOURCE_HANDLER source );
        virtual VOID PauseSource( AUDIO_SOURCE_HANDLER source );
        virtual VOID StopSource( AUDIO_SOURCE_HANDLER source );
        virtual BOOL SourceIsPlaying( AUDIO_SOURCE_HANDLER source );

        virtual VOID SetListenerPos( FLOAT32 x, FLOAT32 y, FLOAT32 z );
        virtual VOID SetListenerDirection( FLOAT32 x, FLOAT32 y, FLOAT32 z );
        virtual VOID SetListenerVelocity( FLOAT32 x, FLOAT32 y, FLOAT32 z );

        virtual VOID Release();
        virtual VOID EnsureThreadContext();
    private:
        VOID*           m_device;
        VOID*           m_context;
    };
}

#endif /* defined(_OPENAL_AUDIO_CONTEXT_H_) */
