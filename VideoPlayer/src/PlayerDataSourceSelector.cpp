//
//  PlayerDataSourceSelector.cpp
//  VideoPlayer
//
//  Created by Yi Huang on 10/11/15.
//  Copyright © 2015 huangyi. All rights reserved.
//

#include "PlayerDataSourceSelector.h"

#include "source/FFMpegStream.h"

const ProcedureDesc& PlayerDataSourceSelector::SelectDataSource( const char* URL )
{
    return FFMpegStream::_GetDesc();
}