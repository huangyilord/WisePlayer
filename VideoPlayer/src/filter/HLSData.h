//
//  HLSData.h
//  VideoPlayer
//
//  Created by wangyu on 15/11/16.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#ifndef HLSData_h
#define HLSData_h

#include "PlayerFilter.h"
#include "PlayerTypes.h"

#define HLS_URL_PACKAGE_NAME   "HLS_URL_PACKAGE"
#define HLS_TS_PACKAGE_NAME   "HLS_TS_PACKAGE"

extern const PlayerProcedureKey HLS_URL_PACKAGE;
extern const PlayerProcedureKey HLS_TS_PACKAGE;

#define MAX_URL_SIZE 1024
#define MAX_SEGMENT_NUM 20
#define MAX_VARIANT_NUM 3

struct variant {
    int bandwidth;
    char base_url[MAX_URL_SIZE];
    char request_url[MAX_URL_SIZE];
    float md;                         //max duration
    uint32 lastSequeue;
};

typedef struct HLSContext {
    uint32 version;           //hls version
    uint32 allcache;          //0:no 1:yes
    int n_variants;
    int cur_variants;
    struct variant **variants;
} HLSContext;

typedef struct HLSURLDATA
{
    char url[MAX_URL_SIZE];
    float duration;
}hlsurldata;

typedef struct HLSTSDATA
{
    void* buff;
    float duration;
    uint32 len;
}hlstsdata;

#endif /* HLSDecoder_hpp */
