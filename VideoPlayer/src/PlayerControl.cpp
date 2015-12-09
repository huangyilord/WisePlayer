//
//  PlayerControl.cpp
//  VideoPlayer
//
//  Created by Yi Huang on 21/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#include "PlayerControl.h"

extern "C"
{
#include "common_time.h"
#include "common_debug.h"
#include "atomic.h"
}

#define TIME_TOLERANCE      300

PlayerControl::PlayerControl()
    : m_pContext( NULL )
    , m_pStatusListener( NULL )
    , m_thread( NULL )
    , m_status( Status_Stop )
    , m_speed( 1.f )
    , m_progress( 0 )
    , m_duration( 0 )
    , m_lastProgress( 0 )
    , m_baseProgress( 0 )
    , m_internalStatus( InternalStatus_Stop )
{
    memset( &m_queue, 0, sizeof(task_queue) );
    memset( &m_task, 0, sizeof(task) );
    memset( &m_mempool, 0, sizeof(memory_pool) );
    memset( &m_destinations, 0, sizeof(hash_map) );
    memset( &m_destinationProgress, 0, sizeof(red_black_tree) );
}

PlayerControl::~PlayerControl()
{
    Destroy();
}

bool PlayerControl::Initialize( PlayerContext* pContext )
{
    m_pContext = pContext;
    m_endEvent = synchronize_create_event();
    if ( !m_endEvent )
    {
        Destroy();
        return false;
    }
    m_mutex = synchronize_create_mutex();
    if ( !m_mutex )
    {
        Destroy();
        return false;
    }
    memory_pool_initialize( &m_mempool, 128, 1 );
    if ( !red_black_tree_initialize( &m_destinationProgress, sizeof(ProgressInfo*), &m_mempool ) )
    {
        Destroy();
        return false;
    }
    m_destinationProgress.compare_fun = ProgressCompare;
    if ( !hash_map_initialize( &m_destinations, sizeof(PlayerProcedureKey), sizeof(ProgressInfo), &m_mempool ) )
    {
        Destroy();
        return false;
    }
    m_thread = task_processor_thread_pool_get( &pContext->processor_thread_pool );
    if ( !m_thread )
    {
        Destroy();
        return false;
    }
    if ( !task_queue_initialize( &m_queue ) || !task_processor_add_task_queue( task_processor_thread_get_processor( m_thread ), &m_queue ) )
    {
        Destroy();
        return false;
    }
    if ( !task_initialize( &m_task, TaskFunction, this ) )
    {
        Destroy();
        return false;
    }
    m_internalStatus = InternalStatus_Stop;
    m_status = Status_Stop;
    m_progress = 0;
    m_lastProgress = 0;
    return true;
}

void PlayerControl::Destroy()
{
    Stop();

    task_destroy( &m_task );
    if ( m_thread )
    {
        task_processor_clear_task_queue( task_processor_thread_get_processor( m_thread ) );
        task_queue_destroy( &m_queue );
        task_processor_thread_pool_put( &m_pContext->processor_thread_pool, m_thread );
        m_thread = NULL;
    }
    hash_map_destroy( &m_destinations );
    red_black_tree_destroy( &m_destinationProgress );
    memory_pool_destroy( &m_mempool );
    if ( m_mutex )
    {
        synchronize_destroy_mutex( m_mutex );
        m_mutex = NULL;
    }
    if ( m_endEvent )
    {
        synchronize_destroy_event( m_endEvent );
        m_endEvent = NULL;
    }
    m_pContext = NULL;
}

bool PlayerControl::Start()
{
    if ( m_internalStatus != InternalStatus_Playing )
    {
        m_internalStatus = InternalStatus_Playing;
        if ( !task_queue_add_task( &m_queue, &m_task ) )
        {
            m_status = Status_Stop;
            return false;
        }
    }
    return true;
}

void PlayerControl::Pause()
{
    if ( m_internalStatus == InternalStatus_Playing )
    {
        m_internalStatus = InternalStatus_Pause;
        synchronize_wait_for_event( m_endEvent );
    }
}

void PlayerControl::Stop()
{
    if ( m_internalStatus != InternalStatus_Stop )
    {
        if ( m_internalStatus == InternalStatus_Playing )
        {
            m_internalStatus = InternalStatus_Stop;
            synchronize_wait_for_event( m_endEvent );
        }
        m_progress = 0;
    }
}

PlayerControl::Status PlayerControl::GetStatus() const
{
    return m_status;
}

void PlayerControl::Update( uint32 interval )
{
    if ( m_internalStatus == InternalStatus_Playing )
    {
        interval = interval * m_speed;
        if ( m_status == Status_Playing )
        {
            m_progress += interval;
            uint32 progress = (uint32)(m_progress - m_baseProgress);
            if ( m_lastProgress < (progress - TIME_TOLERANCE) )
            {
                m_status = Status_Pending;
                m_pStatusListener->OnStatusChanged( m_status );
            }
        }
        else
        {
            uint32 progress = (uint32)(m_progress - m_baseProgress);
            if ( m_lastProgress >= progress )
            {
                m_status = Status_Playing;
                m_pStatusListener->OnStatusChanged( m_status );
            }
        }
    }
}

int64 PlayerControl::GetProgress()
{
    return m_progress;
}

int64 PlayerControl::GetDuration()
{
    return m_duration;
}

bool PlayerControl::RegisterDestination( const PlayerProcedureKey& key )
{
    bool ret = true;
    ProgressInfo* pProgress = NULL;
    ProgressInfo progress = { 0 };
    synchronize_lock_mutex( m_mutex );
    progress.progress = 0;
    progress.key = key;
    do
    {
        if ( !hash_map_insert( &m_destinations, &key, &progress ) )
        {
            ret = false;
            break;
        }
        pProgress = (ProgressInfo*)hash_map_get( &m_destinations, &key );
        ASSERT( NULL != pProgress );
        if ( !red_black_tree_insert( &m_destinationProgress, &pProgress ) )
        {
            hash_map_remove( &m_destinations, &key );
            ret = false;
            break;
        }
    } while ( false );
    synchronize_unlock_mutex( m_mutex );
    return true;
}

void PlayerControl::UnregisterDestination( const PlayerProcedureKey& key )
{
    ProgressInfo* pProgress = NULL;
    synchronize_lock_mutex( m_mutex );
    pProgress = (ProgressInfo*)hash_map_get( &m_destinations, &key );
    if ( NULL != pProgress )
    {
        red_black_tree_remove( &m_destinationProgress, &pProgress );
        hash_map_remove( &m_destinations, &key );
    }
    synchronize_unlock_mutex( m_mutex );
}

void PlayerControl::ReportProgress( const PlayerProcedureKey& key, int64 timeStamp )
{
    ProgressInfo* pProgress = NULL;
    synchronize_lock_mutex( m_mutex );
    pProgress = (ProgressInfo*)hash_map_get( &m_destinations, &key );
    if ( NULL != pProgress )
    {
        red_black_tree_remove( &m_destinationProgress, &pProgress );
        pProgress->progress = timeStamp;
        VERIFY(red_black_tree_insert( &m_destinationProgress, &pProgress ));
    }
    red_black_tree_iterator iter;
    RED_BLACK_TREE_ITERATOR_START( iter, &m_destinationProgress );
    pProgress = *(ProgressInfo**)RED_BLACK_TREE_RITERATOR_GET( iter );
    m_lastProgress = (uint32)(pProgress->progress - m_baseProgress);
    synchronize_unlock_mutex( m_mutex );
}

void PlayerControl::StopAsynchronize()
{
    m_internalStatus = InternalStatus_Stop;
}

void PlayerControl::SetProgress( int64 progress )
{
    (void)atomic_exchange_u64( (uint64 volatile *)&m_progress, progress );
}

void PlayerControl::SetDuration( int64 duration )
{
    m_duration = duration;
}

uint32 PlayerControl::TaskFunction( void* user_data, memory_pool* pool )
{
    PlayerControl* pControl = (PlayerControl*)user_data;
    pControl->m_status = Status_Pending;
    uint32 lastTickCount = time_get_tick_count();
    while ( pControl->m_internalStatus == InternalStatus_Playing )
    {
        uint32 tickCount = time_get_tick_count();
        pControl->Update( tickCount - lastTickCount );
        lastTickCount = tickCount;
        thread_sleep( 10 );
    }
    pControl->m_status = pControl->m_internalStatus == InternalStatus_Stop ? Status_Stop : Status_Pause;
    pControl->m_pStatusListener->OnStatusChanged( pControl->m_status );
    synchronize_set_event( pControl->m_endEvent );
    return 0;
}

int32 PlayerControl::ProgressCompare( red_black_tree* tree, const void* element1, const void* element2 )
{
    const ProgressInfo* progress1 = *(ProgressInfo**)element1;
    const ProgressInfo* progress2 = *(ProgressInfo**)element2;
    return (int32)(progress1->progress - progress2->progress);
}

