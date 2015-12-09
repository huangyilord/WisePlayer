//
//  RegisterProcedures.cpp
//  VideoPlayer
//
//  Created by Yi Huang on 24/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "RegisterProcedures.h"

#include "destination/GraphicDisplayDestination.h"
#include "destination/AudioPlayDestination.h"

#include "filter/FFMpegVideoDecoder.h"
#include "filter/FFMpegAudioDecoder.h"

pipeline_procedure_desc* RegisterProcedure( pipeline_builder* builder, const ProcedureDesc& desc, void* context )
{
    const PlayerProcedureKey* key = NULL;
    pipeline_procedure_desc* ret = pipeline_builder_add_procedure( builder, &desc.key );
    if ( NULL == ret )
    {
        return NULL;
    }
    key = desc.inputs;
    while ( 0 != key->name[0] )
    {
        if ( !hash_set_insert( &ret->input_keys, key ) )
        {
            pipeline_builder_remove_procedure( builder, &desc.key );
            return NULL;
        }
        ++key;
    }
    key = desc.outputs;
    while ( 0 != key->name[0] )
    {
        if ( !hash_set_insert( &ret->output_keys, key ) )
        {
            pipeline_builder_remove_procedure( builder, &desc.key );
            return NULL;
        }
        ++key;
    }
    ret->context = context;
    ret->create_function = desc.creator;
    ret->destroy_function = desc.destroyer;
    return ret;
}

void UnregisterProcedure( pipeline_builder* builder, const ProcedureDesc& desc )
{
    pipeline_builder_remove_procedure( builder, &desc.key );
}

bool RegisterAll( pipeline_builder* builder, void* context )
{
    bool ret = true;
    if ( NULL == RegisterProcedure( builder, GraphicDisplayDestination::_GetDesc(), context ) )
    {
        UnregisterAll( builder );
        return false;
    }
    if ( NULL == RegisterProcedure( builder, AudioPlayDestination::_GetDesc(), context ) )
    {
        UnregisterAll( builder );
        return false;
    }
    if ( NULL == RegisterProcedure( builder, FFMpegVideoDecoder::_GetDesc(), context ) )
    {
        UnregisterAll( builder );
        return false;
    }
    if ( NULL == RegisterProcedure( builder, FFMpegAudioDecoder::_GetDesc(), context ) )
    {
        UnregisterAll( builder );
        return false;
    }
    return ret;
}

void UnregisterAll( pipeline_builder* builder )
{
    UnregisterProcedure( builder, GraphicDisplayDestination::_GetDesc() );
    UnregisterProcedure( builder, AudioPlayDestination::_GetDesc() );
    UnregisterProcedure( builder, FFMpegVideoDecoder::_GetDesc() );
    UnregisterProcedure( builder, FFMpegAudioDecoder::_GetDesc() );
}

