//
//  EffectManager.cpp
//  VideoPlayer
//
//  Created by Yi Huang on 25/11/15.
//  Copyright © 2015 huangyi. All rights reserved.
//

#include "EffectManager.h"

#include "MosaicEffect.h"
#include "FadeEffect.h"
#include "SketchEffect.h"
#include "BlurEffect.h"
#include "SharpeningEffect.h"
#include "FilterShaderHDR.h"

Graphic::GraphicFilter* EffectManager::CreateEffect( EffectType type, PlayerContext* pContext )
{
    switch ( type )
    {
        case EffectType_Mosaic:
        {
            MosaicEffect* pMosaic = new MosaicEffect();
            if ( NULL == pMosaic )
            {
                return NULL;
            }
            if ( !pMosaic->Initialize( pContext->pGraphicContext, (Graphic::UtilGraphicResourceController*)pContext->pResourceManager->GetController(Graphic::GraphicResourceController::GRCID_UTIL) ) )
            {
                delete pMosaic;
                return NULL;
            }
            return pMosaic;
        }
        case EffectType_Fade:
        {
            FadeEffect* pFade = new FadeEffect();
            if ( NULL == pFade )
            {
                return NULL;
            }
            if ( !pFade->Initialize( pContext->pGraphicContext, (Graphic::UtilGraphicResourceController*)pContext->pResourceManager->GetController(Graphic::GraphicResourceController::GRCID_UTIL) ) )
            {
                delete pFade;
                return NULL;
            }
            return pFade;
        }
        case EffectType_Blur:
        {
            BlurEffect* pBlur = new BlurEffect();
            if ( NULL == pBlur )
            {
                return NULL;
            }
            if ( !pBlur->Initialize( pContext->pGraphicContext, (Graphic::UtilGraphicResourceController*)pContext->pResourceManager->GetController(Graphic::GraphicResourceController::GRCID_UTIL) ) )
            {
                delete pBlur;
                return NULL;
            }
            return pBlur;
        }
        case EffectType_Sketch:
        {
            SketchEffect* pSketch = new SketchEffect();
            if ( NULL == pSketch )
            {
                return NULL;
            }
            if ( !pSketch->Initialize( pContext->pGraphicContext, (Graphic::UtilGraphicResourceController*)pContext->pResourceManager->GetController(Graphic::GraphicResourceController::GRCID_UTIL) ) )
            {
                delete pSketch;
                return NULL;
            }
            return pSketch;
        }
        case EffectType_Sharpening:
        {
            SharpeningEffect* pSharpening = new SharpeningEffect();
            if ( NULL == pSharpening )
            {
                return NULL;
            }
            if ( !pSharpening->Initialize( pContext->pGraphicContext, (Graphic::UtilGraphicResourceController*)pContext->pResourceManager->GetController(Graphic::GraphicResourceController::GRCID_UTIL) ) )
            {
                delete pSharpening;
                return NULL;
            }
            return pSharpening;
        }
        case EffectType_HDR:
        {
            ShaderHDR* hdr = new ShaderHDR();
            if ( NULL == hdr )
            {
                return NULL;
            }
            if ( !hdr->Initialize( pContext->pGraphicContext, (Graphic::UtilGraphicResourceController*)pContext->pResourceManager->GetController(Graphic::GraphicResourceController::GRCID_UTIL) ) )
            {
                delete hdr;
                return NULL;
            }
            return hdr;
        }
    }
}

VOID EffectManager::ReleaseEffect( Graphic::GraphicFilter* pFilter, PlayerContext* pContext )
{
    delete pFilter;
}