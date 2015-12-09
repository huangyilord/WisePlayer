//
//  PlayerControl.h
//  VideoPlayer
//
//  Created by Yi Huang on 21/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _PLAYER_CONTROL_H_ )
#define _PLAYER_CONTROL_H_

#include "PlayerTypes.h"
extern "C"
{
#include "red_black_tree.h"
}

class PlayerStatusListener;

class PlayerControl
{
public:
    enum Status
    {
        Status_Stop
        , Status_Playing
        , Status_Pause
        , Status_Pending
    };
public:
    PlayerControl();
    ~PlayerControl();

    bool Initialize( PlayerContext* pContext );
    void Destroy();

    bool PrepareToStart();
    bool Start();
    void Pause();
    void Stop();

    Status GetStatus() const;

    void Update( uint32 interval );
    int64 GetProgress();
    int64 GetDuration();

    bool RegisterDestination( const PlayerProcedureKey& key );
    void UnregisterDestination( const PlayerProcedureKey& key );
    void ReportProgress( const PlayerProcedureKey& key, int64 timeStamp );

    inline void SetStatusListener( PlayerStatusListener* pListener ) { m_pStatusListener = pListener; }

    // only for internal useage
    void StopAsynchronize();
    void SetProgress( int64 progress );
    void SetDuration( int64 duration );
private:
    static uint32 TaskFunction( void* user_data, memory_pool* pool );
    static int32 ProgressCompare( red_black_tree* tree, const void* element1, const void* element2 );

    typedef struct ProgressInfo
    {
        int64               progress;
        PlayerProcedureKey  key;
    } ProgressInfo;

    typedef enum InternalStatus
    {
        InternalStatus_Stop
        , InternalStatus_Playing
        , InternalStatus_Pause
    } InternalStatus;

    PlayerContext*                  m_pContext;
    PlayerStatusListener*           m_pStatusListener;
    task_processor_thread*          m_thread;
    task_queue                      m_queue;
    task                            m_task;
    Status                          m_status;
    float32                         m_speed;

    memory_pool                     m_mempool;
    red_black_tree                  m_destinationProgress;
    hash_map                        m_destinations;
    int64                           m_progress;
    int64                           m_duration;
    volatile uint32                 m_lastProgress;
    int64                           m_baseProgress;

    event_type                      m_endEvent;
    mutex_type                      m_mutex;
    volatile InternalStatus         m_internalStatus;
};

class PlayerStatusListener
{
public:
    virtual void OnStatusChanged( PlayerControl::Status status ) = 0;
};

#endif /* defined(_PLAYER_CONTROL_H_) */
