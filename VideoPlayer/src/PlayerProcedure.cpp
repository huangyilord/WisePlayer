//
//  PlayerProcedure.cpp
//  WisePlayer
//
//  Created by Yi Huang on 1/6/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#include "PlayerProcedure.h"


PlayerProcedure::PlayerProcedure( PlayerContext* context )
    : m_pPlayerContext( context )
{
    memset( &m_procedure, 0, sizeof(pipeline_procedure) );
}

PlayerProcedure::~PlayerProcedure()
{
    Destroy();
}

bool PlayerProcedure::Initialize()
{
    pipeline_procedure_config config;
    config.key_size = KEY_MAX_LENGTH;
    config.update_function = ProcedureTaskFunction;
    config.thread_pool = &m_pPlayerContext->processor_thread_pool;
    config.context = this;
    config.input_data_handler = ProcessInputFunction;
    if ( !pipeline_procedure_initialize( &m_procedure, &config ) )
    {
        return false;
    }
    return true;
}

void PlayerProcedure::Destroy()
{
    if ( m_procedure.input_event )
    {
        pipeline_procedure_destroy( &m_procedure );
    }
    m_pPlayerContext = NULL;
}

bool PlayerProcedure::Start()
{
    return pipeline_procedure_start( &m_procedure );
}

void PlayerProcedure::Stop()
{
    pipeline_procedure_stop( &m_procedure );
}

bool PlayerProcedure::AddInput( const PlayerProcedureKey& key )
{
    return pipeline_procedure_add_input( &m_procedure, &key ) != NULL;
}

void PlayerProcedure::RemoveInput( const PlayerProcedureKey& key )
{
    pipeline_procedure_remove_input( &m_procedure, &key );
}

bool PlayerProcedure::AddOutput( const PlayerProcedureKey& key )
{
    return pipeline_procedure_add_output( &m_procedure, &key ) != NULL;
}

void PlayerProcedure::RemoveOutput( const PlayerProcedureKey& key )
{
    pipeline_procedure_remove_output( &m_procedure, &key );
}

bool PlayerProcedure::ProcessAnyInput()
{
    return pipeline_procedure_handle_any_input( &m_procedure );
}

bool PlayerProcedure::TryProcessAnyInput()
{
    return pipeline_procedure_try_handle_any_input( &m_procedure );
}

bool PlayerProcedure::ProcessInput( const PlayerProcedureKey& key )
{
    return pipeline_procedure_handle_input( &m_procedure, &key );
}

bool PlayerProcedure::TryProcessInput( const PlayerProcedureKey& key )
{
    return pipeline_procedure_try_handle_input( &m_procedure, &key );
}

void* PlayerProcedure::GetOutputBuffer( const PlayerProcedureKey& key, size_t size )
{
    return pipeline_procedure_get_output_buffer( &m_procedure, &key, size );
}

void* PlayerProcedure::TryGetOutputBuffer( const PlayerProcedureKey& key, size_t size )
{
    return pipeline_procedure_try_get_output_buffer( &m_procedure, &key, size );
}

void PlayerProcedure::CommitOutputBuffer( const PlayerProcedureKey& key )
{
    pipeline_procedure_commit_output_buffer( &m_procedure, &key );
}

void PlayerProcedure::Update()
{
}

bool PlayerProcedure::ProcessInput( const PlayerProcedureKey&, void* )
{
    return TRUE;
}

boolean PlayerProcedure::ProcedureTaskFunction( pipeline_procedure*, void* context )
{
    PlayerProcedure* procedure = (PlayerProcedure*)context;
    procedure->Update();
    return true;
}

boolean PlayerProcedure::ProcessInputFunction( pipeline_procedure*, void* context, const void* key, void* data )
{
    PlayerProcedure* procedure = (PlayerProcedure*)context;
    PlayerProcedureKey* procKey = (PlayerProcedureKey*)key;
    return procedure->ProcessInput( *procKey, data );
}