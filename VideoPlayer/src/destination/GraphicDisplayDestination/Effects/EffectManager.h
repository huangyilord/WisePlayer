//
//  EffectManager.hpp
//  VideoPlayer
//
//  Created by Yi Huang on 25/11/15.
//  Copyright © 2015 huangyi. All rights reserved.
//

#if !defined ( _EFFECT_MANAGER_H_ )
#define _EFFECT_MANAGER_H_

#include "PlayerTypes.h"

class EffectManager
{
public:
    enum EffectType
    {
        EffectType_Mosaic = 0
        , EffectType_Fade
        , EffectType_Blur
        , EffectType_Sketch
        , EffectType_Sharpening
        , EffectType_HDR
    };

    static Graphic::GraphicFilter* CreateEffect( EffectType type, PlayerContext* pContext );
    static VOID ReleaseEffect( Graphic::GraphicFilter* pFilter, PlayerContext* pContext );
};

#endif /* _EFFECT_MANAGER_H_ */
