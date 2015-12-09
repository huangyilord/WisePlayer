//
//  HLSDownloadTS.h
//  VideoPlayer
//
//  Created by wangyu on 15/11/16.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#ifndef HLSDownloadTS_h
#define HLSDownloadTS_h

#include "PlayerFilter.h"
#include "PlayerTypes.h"
#include "HLSData.h"
#include "HLScURL.h"

class HLSDownloadTS : public PlayerFilter
{
public:
    HLSDownloadTS( PlayerContext* context );
    virtual ~HLSDownloadTS();
    
    bool Initialize();
    void Destroy();
    
    PROCEDURE_DESC_START( HLSDownloadTS )
        PROCEDURE_INPUT_START()
            PROCEDURE_INPUT( HLS_URL_PACKAGE_NAME )
        PROCEDURE_INPUT_END()
        PROCEDURE_OUTPUT_START()
            PROCEDURE_OUTPUT( HLS_TS_PACKAGE_NAME )
        PROCEDURE_OUTPUT_END()
    PROCEDURE_DESC_END( HLSDownloadTS )
protected:
    virtual bool ProcessInput( const PlayerProcedureKey& key, void* data );
private:
    
};

#endif /* HLSDownloadTS_hpp */
