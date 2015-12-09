//
//  IOSMediaData.hpp
//  VideoPlayer
//
//  Created by DongRanRan on 15/11/16.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#ifndef IOSMediaData_hpp
#define IOSMediaData_hpp

#include "PlayerTypes.h"

extern "C"
{
#include "libavfilter/avcodec.h"
#include "libavformat/avformat.h"
}

#define IOS_VIDEO_PACKAGE_NAME   "IOS_VIDEO_PACKAGE"
#define IOS_AUDIO_PACKAGE_NAME   "IOS_AUDIO_PACKAGE"

extern const PlayerProcedureKey IOS_VIDEO_PACKAGE;
extern const PlayerProcedureKey IOS_AUDIO_PACKAGE;

#endif /* IOSMediaData_hpp */
