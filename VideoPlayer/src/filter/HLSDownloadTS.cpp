//
//  HLSDownloadTS.cpp
//  VideoPlayer
//
//  Created by wangyu on 15/11/16.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#include "HLSDownloadTS.h"

HLSDownloadTS::HLSDownloadTS( PlayerContext* context ) : PlayerFilter( context )
{
    
}
HLSDownloadTS::~HLSDownloadTS()
{
    
}

bool HLSDownloadTS::Initialize()
{
    if ( !PlayerFilter::Initialize() )
    {
        Destroy();
        return false;
    }
    return true;
}
void HLSDownloadTS::Destroy()
{
    PlayerFilter::Destroy();
}
bool HLSDownloadTS::ProcessInput( const PlayerProcedureKey& key, void* data )
{
    hlsurldata* urldata = (hlsurldata*)data;
    hlstsdata* tsdata = (hlstsdata*)GetOutputBuffer(HLS_TS_PACKAGE, sizeof(hlstsdata));
    if (NULL != tsdata) {
        http_payload_t *payload = (http_payload_t *)malloc(sizeof(http_payload_t));
        if (curl_http_download(urldata->url, payload) == 0)
        {
            tsdata->buff = payload->memory;
            tsdata->len = payload->size;
            tsdata->duration = urldata->duration;
//            hls_dbg("commit tsdata len:%d\n", tsdata->len);
            CommitOutputBuffer(HLS_TS_PACKAGE);
        }
        delete payload;
    }
    return true;
}