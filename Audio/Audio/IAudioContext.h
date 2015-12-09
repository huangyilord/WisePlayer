//
//  IAudioContext.h
//  Audio
//
//  Created by Yi Huang on 2/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _I_AUDIO_CONTEXT_H_ )
#define _I_AUDIO_CONTEXT_H_

#include "AudioTypeDefs.h"

namespace Audio
{
    class IAudioContext
    {
    public:
        enum DataFormat
        {
            DataFormat_MONO8 = 0
            , DataFormat_MONO16
            , DataFormat_STEREO8
            , DataFormat_STEREO16
        };

        enum SourceType
        {
            SourceType_Static = 0
            , SourceType_Stream
        };

        virtual AUDIO_BUFFER_HANDLER CreateBuffer() = 0;
        virtual VOID DestroyBuffer( AUDIO_BUFFER_HANDLER buffer ) = 0;
        virtual BOOL BufferData( AUDIO_BUFFER_HANDLER buffer, DataFormat format, const VOID* data, UINT32 size, UINT32 frequency ) = 0;

        virtual AUDIO_SOURCE_HANDLER CreateSource( SourceType type ) = 0;
        virtual VOID DestroySource( AUDIO_SOURCE_HANDLER source ) = 0;
        virtual VOID SetSourcePos( AUDIO_SOURCE_HANDLER source, FLOAT32 x, FLOAT32 y, FLOAT32 z ) = 0;
        virtual VOID SetSourceVelocity( AUDIO_SOURCE_HANDLER source, FLOAT32 x, FLOAT32 y, FLOAT32 z ) = 0;
        virtual BOOL SourceEnqueueBuffer( AUDIO_SOURCE_HANDLER source, AUDIO_BUFFER_HANDLER buffer ) = 0;
        virtual AUDIO_BUFFER_HANDLER SourceDequeueBuffer( AUDIO_SOURCE_HANDLER source ) = 0;
        virtual UINT32 GetSourceProcessedBufferCount( AUDIO_SOURCE_HANDLER source ) = 0;
        virtual UINT32 GetSourceQueuedBufferCount( AUDIO_SOURCE_HANDLER source ) = 0;

        virtual VOID PlaySource( AUDIO_SOURCE_HANDLER source ) = 0;
        virtual VOID PauseSource( AUDIO_SOURCE_HANDLER source ) = 0;
        virtual VOID StopSource( AUDIO_SOURCE_HANDLER source ) = 0;
        virtual BOOL SourceIsPlaying( AUDIO_SOURCE_HANDLER source ) = 0;

        virtual VOID SetListenerPos( FLOAT32 x, FLOAT32 y, FLOAT32 z ) = 0;
        virtual VOID SetListenerDirection( FLOAT32 x, FLOAT32 y, FLOAT32 z ) = 0;
        virtual VOID SetListenerVelocity( FLOAT32 x, FLOAT32 y, FLOAT32 z ) = 0;

        virtual VOID Release() = 0;
        virtual VOID EnsureThreadContext() = 0;
    };
}


#endif
