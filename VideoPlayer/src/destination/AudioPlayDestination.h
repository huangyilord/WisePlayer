//
//  AudioPlayDestination.h
//  VideoPlayer
//
//  Created by Yi Huang on 24/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _AUDIO_PLAY_DESTINATION_H_ )
#define _AUDIO_PLAY_DESTINATION_H_

#include "GraphicTypes.h"
#include "PlayerDestination.h"
#include "datatype/AudioFragment.h"
#include "IAudioContext.h"
#include "list.h"

class AudioPlayDestination
    : public PlayerDestination
{
public:
    AudioPlayDestination( PlayerContext* context );
    virtual ~AudioPlayDestination();

    bool Initialize();
    void Destroy();

    PROCEDURE_DESC_START( AudioPlayDestination )
        PROCEDURE_INPUT_START()
            PROCEDURE_INPUT( AUDIO_FRAGMENT_NAME )
        PROCEDURE_INPUT_END()
        PROCEDURE_OUTPUT_START()
        PROCEDURE_OUTPUT_END()
    PROCEDURE_DESC_END( AudioPlayDestination )
protected:
    // implement PlayerDestination
    virtual bool ProcessInput( const PlayerProcedureKey& key, void* data );
    virtual bool UpdateContent( const PlayerDestinationContext& context );
private:
    Audio::AUDIO_SOURCE_HANDLER         m_source;
    memory_pool                         m_memoryPool;
    list                                m_bufferQueue;
};

#endif /* defined(_AUDIO_PLAY_DESTINATION_H_) */
