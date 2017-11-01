//
//  PlayerDataSourceSelector.hpp
//  VideoPlayer
//
//  Created by Yi Huang on 10/11/15.
//  Copyright © 2015 huangyi. All rights reserved.
//

#if !defined ( _PLAYER_DATA_SOURCE_SELECTOR_H_ )
#define _PLAYER_DATA_SOURCE_SELECTOR_H_

#include "PlayerTypes.h"

class PlayerDataSourceSelector
{
public:
    static const ProcedureDesc& SelectDataSource( const char* URL );
};

#endif /* _PLAYER_DATA_SOURCE_SELECTOR_H_ */
