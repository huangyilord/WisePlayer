//
//  PlayerDestination.h
//  WisePlayer
//
//  Created by Yi Huang on 8/6/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#if !defined ( _PLAYER_DESTINATION_H_ )
#define _PLAYER_DESTINATION_H_

#include "PlayerProcedure.h"

typedef struct PlayerDestinationContext
{
    uint32      timeInterval;
    int64       totalTimeElapst;
    uint32      lastUpdateTime;
} PlayerDestinationContext;

class PlayerDestination
    : public PlayerProcedure
{
public:
    PlayerDestination( PlayerContext* context );
    virtual ~PlayerDestination();

    bool Initialize();
    void Destroy();
protected:
    /**
     *  Update status.
     *
     *  @param context:     The destination context.
     */
    virtual bool UpdateContent( const PlayerDestinationContext& context );

    int64 GetProgress() const;
    void SetProgress( int64 progress );

    // TaskProcedure
    virtual void Update();
private:
    PlayerDestinationContext            m_destinationContext;
    int64                               m_progress;
};

#endif /* defined(_PLAYER_DESTINATION_H_) */
