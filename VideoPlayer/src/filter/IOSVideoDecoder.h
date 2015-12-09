//
//  IOSVideoDecoder.h
//  VideoPlayer
//
//  Created by DongRanRan on 15/11/16.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#ifndef IOSVideoDecoder_h
#define IOSVideoDecoder_h

#include "PlayerFilter.h"

extern "C"
{
#include "libswscale/swscale.h"
}
#include "IOSMediaData.h"
#include "HLSData.h"

class IOSVideoDecoder : public PlayerFilter
{
public:
    IOSVideoDecoder( PlayerContext* context );
    virtual ~IOSVideoDecoder();
    
    bool Initialize();
    void Destroy();
    
    PROCEDURE_DESC_START( IOSVideoDecoder )
    PROCEDURE_INPUT_START()
    PROCEDURE_INPUT( HLS_URL_PACKAGE_NAME )
    PROCEDURE_INPUT_END()
    PROCEDURE_OUTPUT_START()
    PROCEDURE_OUTPUT( IOS_VIDEO_PACKAGE_NAME )
    PROCEDURE_OUTPUT_END()
    PROCEDURE_DESC_END( IOSVideoDecoder )
    
protected:
    // PlayerFilter
    virtual bool ProcessInput( const PlayerProcedureKey& key, void* data );
};
#endif /* IOSVideoDecoder_h */
