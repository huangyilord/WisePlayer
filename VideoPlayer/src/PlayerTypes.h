//
//  PlayerTypes.h
//
//  Created by Yi Huang on 8/6/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#if !defined ( _PLAYER_TYPES_H_ )
#define _PLAYER_TYPES_H_

extern "C"
{
#include "pipeline_procedure.h"
#include "pipeline_builder.h"
}

#include "IGraphicContext.h"
#include "IAudioContext.h"
#include "GraphicResourceMgr.h"

#define KEY_MAX_LENGTH          32

typedef struct VideoInfo
{
    uint32              frameWidth;
    uint32              frameHeight;

    uint32              frameNum;
    uint32              fraquency;
} VideoInfo;

typedef struct AudioInfo
{
    uint32              fraq;
    uint32              format;
} AudioInfo;

typedef struct PlayerMediaInfo
{
    uint32              displayWidth;
    uint32              displayHeight;
    VideoInfo           video;
    AudioInfo           audio;
} PlayerMediaInfo;

class PlayerControl;

typedef struct PlayerContext
{
    memory_pool                         mempool;
    task_processor_thread_pool          processor_thread_pool;
    pipeline_builder*                   builder;
    pipeline                            play_pipeline;
    hash_map                            procedure_types;

    PlayerMediaInfo                     mediaInfo;
    PlayerControl*                      pPlayerControl;
    Graphic::IGraphicContext*           pGraphicContext;
    Graphic::GraphicResourceMgr*        pResourceManager;
    Audio::IAudioContext*               pAudioContext;
} PlayerContext;

typedef struct PlayerProcedureKey
{
    char                    name[KEY_MAX_LENGTH];
} PlayerProcedureKey;

typedef struct ProcedureDesc
{
    PlayerProcedureKey                  key;
    const PlayerProcedureKey*           inputs;
    const PlayerProcedureKey*           outputs;
    pipeline_procedure_create_function  creator;
    pipeline_procedure_destroy_function destroyer;
} ProcedureDesc;

#endif /* defined(_PLAYER_TYPES_H_) */
