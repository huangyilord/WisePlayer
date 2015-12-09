//
//  RGBATexture.h
//  VideoPlayer
//
//  Created by Yi Huang on 11/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _RGBA_TEXTURE_H_ )
#define _RGBA_TEXTURE_H_

#include "PlayerTypes.h"

#define RGBA_TEXTURE_NAME               "RGBA_TEX"
#define STRETCHED_RGBA_TEXTURE_NAME     "STRETCHED_RGBA_TEX"

extern const PlayerProcedureKey RGBA_TEXTURE;
extern const PlayerProcedureKey STRETCHED_RGBA_TEXTURE;

typedef struct RGBATexture
{
    int64   time;
    void*   pixelData;
    uint32  duration;
    uint32  x;
    uint32  y;
    uint32  width;
    uint32  pixelCount;
    uint32  reserved;
} RGBATexture;

#endif // #if !defined ( _STRETCHED_TEXTURE_H_ )
