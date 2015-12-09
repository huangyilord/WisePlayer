//
//  FFMpegAudioDecoder.h
//  VideoPlayer
//
//  Created by Yi Huang on 24/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _FFMPEG_AUDIO_DECODER_H_ )
#define _FFMPEG_AUDIO_DECODER_H_

#include "PlayerFilter.h"

#include "FFMpegData.h"
#include "AudioFragment.h"

extern "C"
{
#include "libswresample/swresample.h"
}

class FFMpegAudioDecoder
    : public PlayerFilter
{
public:
    FFMpegAudioDecoder( PlayerContext* context );
    virtual ~FFMpegAudioDecoder();
    
    bool Initialize();
    void Destroy();
    
    PROCEDURE_DESC_START( FFMpegAudioDecoder )
        PROCEDURE_INPUT_START()
            PROCEDURE_INPUT( FFMPEG_AUDIO_PACKAGE_NAME )
        PROCEDURE_INPUT_END()
        PROCEDURE_OUTPUT_START()
            PROCEDURE_OUTPUT( AUDIO_FRAGMENT_NAME )
        PROCEDURE_OUTPUT_END()
    PROCEDURE_DESC_END( FFMpegAudioDecoder )
    
protected:
    // PlayerFilter
    virtual bool ProcessInput( const PlayerProcedureKey& key, void* data );
    
    AVFrame*            m_pFrame;
    struct SwrContext*  m_swrContext;
    uint32              m_textureSize;
    int64_t             m_nextPTS;
    AVRational          m_nextTimeBase;

    int                 m_currentSampleRate;
    AVSampleFormat      m_currentSampleFormat;
    size_t              m_currentBufferSize;
};

#endif /* defined(_FFMPEG_AUDIO_DECODER_H_) */
