//
//  TaskProcedure.h
//  WisePlayer
//
//  Created by Yi Huang on 1/6/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#if !defined ( _TASK_PROCEDURE_H_ )
#define _TASK_PROCEDURE_H_

#include "PlayerTypes.h"

class PlayerProcedure
{
public:
    PlayerProcedure( PlayerContext* context );
    virtual ~PlayerProcedure();

    bool Initialize();
    void Destroy();

    bool Start();
    void Stop();
protected:
    bool AddInput( const PlayerProcedureKey& key );
    void RemoveInput( const PlayerProcedureKey& key );
    bool AddOutput( const PlayerProcedureKey& key );
    void RemoveOutput( const PlayerProcedureKey& key );

    bool ProcessAnyInput();
    bool TryProcessAnyInput();
    bool ProcessInput( const PlayerProcedureKey& key );
    bool TryProcessInput( const PlayerProcedureKey& key );

    void* GetOutputBuffer( const PlayerProcedureKey& key, size_t size );
    void* TryGetOutputBuffer( const PlayerProcedureKey& key, size_t size );
    void CommitOutputBuffer( const PlayerProcedureKey& key );

    virtual void Update();
    virtual bool ProcessInput( const PlayerProcedureKey& key, void* data );
protected:
    virtual const ProcedureDesc& GetDesc() = 0;

    pipeline_procedure          m_procedure;
    PlayerContext*              m_pPlayerContext;

private:
    static boolean ProcedureTaskFunction( pipeline_procedure* procedure, void* context );
    static boolean ProcessInputFunction( pipeline_procedure* procedure, void* context, const void* key, void* data );
};

#define TO_STR( __x__ ) #__x__

#define PROCEDURE_DESC_START( __class_name__ ) \
    typedef __class_name__ InstanceType; \
    public: \
        static const ProcedureDesc& _GetDesc() \
        {
#define PROCEDURE_INPUT_START() \
            static const PlayerProcedureKey INPUTS[] = {

#define PROCEDURE_INPUT( __name__ ) \
                { __name__ },

#define PROCEDURE_INPUT_END() \
                {0} };

#define PROCEDURE_OUTPUT_START() \
            static const PlayerProcedureKey OUTPUTS[] = {

#define PROCEDURE_OUTPUT( __name__ ) \
                { __name__ },

#define PROCEDURE_OUTPUT_END() \
                {0} };

#define PROCEDURE_DESC_END( __name__ ) \
            static const ProcedureDesc DESC = { \
                { TO_STR(PROC_##__name__) } \
                , INPUTS \
                , OUTPUTS \
                , _CreateProcedure \
                , _DestroyProcedure \
            }; \
            return DESC; \
        } \
        static pipeline_procedure* _CreateProcedure( void* context ) \
        { \
            PlayerContext* pContext = (PlayerContext*)context; \
            InstanceType* instance = new(std::nothrow) InstanceType( pContext ); \
            if ( NULL == instance ) \
            { \
                return NULL; \
            } \
            if ( !instance->Initialize() ) \
            { \
                delete instance; \
                return NULL; \
            } \
            const PlayerProcedureKey* key = _GetDesc().inputs; \
            while ( key->name[0] ) \
            { \
                if ( !pipeline_procedure_add_input( &instance->m_procedure, key ) ) \
                { \
                    delete instance; \
                    return NULL; \
                } \
                ++ key; \
            } \
            key = _GetDesc().outputs; \
            while ( key->name[0] ) \
            { \
                if ( !pipeline_procedure_add_output( &instance->m_procedure, key ) ) \
                { \
                    delete instance; \
                    return NULL; \
                } \
                ++ key; \
            } \
            return &instance->m_procedure; \
        } \
        static void _DestroyProcedure( pipeline_procedure* procecure, void* ) \
        { \
            InstanceType* instance = (InstanceType*)procecure->config.context; \
            delete instance; \
        } \
    protected: \
        virtual const ProcedureDesc& GetDesc() { return _GetDesc(); }
#endif /* defined(_TASK_PROCEDURE_H_) */
