//
//  AudioFragment.h
//  VideoPlayer
//
//  Created by Yi Huang on 24/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _AUDIO_FRAGMENT_H_ )
#define _AUDIO_FRAGMENT_H_

#include "PlayerTypes.h"

#define AUDIO_FRAGMENT_NAME             "AUDIO_FRAGMENT"

extern const PlayerProcedureKey AUDIO_FRAGMENT;

typedef struct AudioFragment
{
    int64   time;
    uint32  duration;
    uint32  fraquency;
    void*   data;
    size_t  size;
} AudioFragment;

#endif /* defined(_AUDIO_FRAGMENT_H_) */
