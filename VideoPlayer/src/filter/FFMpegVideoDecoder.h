//
//  FFMpegVideoDecoder.h
//  VideoPlayer
//
//  Created by Yi Huang on 23/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _FFMPEG_VIDEO_DECODER_H_ )
#define _FFMPEG_VIDEO_DECODER_H_

#include "PlayerFilter.h"
#include "datatype/RGBATexture.h"

extern "C"
{
#include "libswscale/swscale.h"
}
#include "FFMpegData.h"

class FFMpegVideoDecoder
    : public PlayerFilter
{
public:
    FFMpegVideoDecoder( PlayerContext* context );
    virtual ~FFMpegVideoDecoder();

    bool Initialize();
    void Destroy();

    PROCEDURE_DESC_START( FFMpegVideoDecoder )
        PROCEDURE_INPUT_START()
            PROCEDURE_INPUT( FFMPEG_VIDEO_PACKAGE_NAME )
        PROCEDURE_INPUT_END()
        PROCEDURE_OUTPUT_START()
            PROCEDURE_OUTPUT( STRETCHED_RGBA_TEXTURE_NAME )
        PROCEDURE_OUTPUT_END()
    PROCEDURE_DESC_END( FFMpegVideoDecoder )

protected:
    // PlayerFilter
    virtual bool ProcessInput( const PlayerProcedureKey& key, void* data );
private:
    uint32 GetTextureSize( uint32 width, uint32 height );

    AVFrame*            m_pFrame;
    SwsContext*         m_pImageConvertContext;
    uint32              m_textureSize;
    int64_t             m_lastPts;
    int64_t             m_lastTimeStamp;
};

#endif /* defined(_FFMPEG_VIDEO_DECODER_H_) */
