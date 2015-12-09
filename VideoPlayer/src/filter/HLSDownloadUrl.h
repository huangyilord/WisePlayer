//
//  HLSDownloadUrl.h
//  VideoPlayer
//
//  Created by wangyu on 15/11/16.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#ifndef HLSDownloadUrl_h
#define HLSDownloadUrl_h

#include "PlayerTypes.h"
#include "HLSData.h"
#include "PlayerDataSource.h"
#include "HLScURL.h"
#include "hash_set.h"
#include "memory_pool.h"

class HLSDownloadUrl: public PlayerDataSource
{
public:
    HLSDownloadUrl( PlayerContext* context );
    virtual ~HLSDownloadUrl();
    
    bool Initialize();
    void Destroy();
    
    // PlayerDataSource
    virtual bool SetURL( const char* url );
    virtual bool Seek( int64 position );
    
    PROCEDURE_DESC_START( HLSDownloadUrl )
        PROCEDURE_INPUT_START()
        PROCEDURE_INPUT_END()
        PROCEDURE_OUTPUT_START()
            PROCEDURE_OUTPUT( HLS_URL_PACKAGE_NAME )
        PROCEDURE_OUTPUT_END()
    PROCEDURE_DESC_END( HLSDownloadUrl )
protected:
    virtual void OnStart();
    virtual void OnStop();
    virtual bool GetData();
private:
    int parse_playlist(HLSContext *c, http_payload_t *payload);
    void hls_newHLSContext(HLSContext* c);
    int hls_add_segment(char* url);
    
    HLSContext mainContext;
    hash_set urlset;
    memory_pool pool;
};
#endif /* HLSDownloadUrl_hpp */
