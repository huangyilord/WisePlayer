//
//  OpenALInternalTypes.h
//  Audio
//
//  Created by Yi Huang on 3/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _OPENAL_INTERNAL_TYPES_H_ )
#define _OPENAL_INTERNAL_TYPES_H_

#include "AudioTypeDefs.h"

#include <queue>

namespace Audio
{
    class OpenALBuffer
    {
    public:
        OpenALBuffer( UINT32 ID );
        virtual ~OpenALBuffer();

        inline UINT32 GetID() const { return m_ID; }
    private:
        UINT32      m_ID;
    };

    class OpenALSource
    {
    public:
        OpenALSource( UINT32 ID );
        virtual ~OpenALSource();
        
        inline UINT32 GetID() const { return m_ID; }

        BOOL IsEmpty() { return m_bufferQueue.empty(); }
        VOID EnqueueBuffer( OpenALBuffer* buffer ) { m_bufferQueue.push( buffer ); }
        VOID DequeueBuffer() { m_bufferQueue.pop(); }
        OpenALBuffer* FirstBuffer() { return m_bufferQueue.front(); }
    private:
        UINT32                      m_ID;
        std::queue<OpenALBuffer*>   m_bufferQueue;
    };
}

#endif /* defined(_OPENAL_INTERNAL_TYPES_H_) */
