//
//  FilterShaderHDR.h
//  VideoPlayer
//
//  Created by wangyu on 15/9/28.
//  Copyright © 2015年 huangyi. All rights reserved.
//
//参考网址 http://dev.gameres.com/Program/Visual/3D/HDRTutorial/HDRTutorial.htm
#ifndef FilterShaderHDR_h
#define FilterShaderHDR_h

#include "IGraphicContext.h"

const UINT32 textureSize = 32;
const FLOAT32 lumAdapt = 1.5;
const FLOAT32 averageLum = 0.07;
class ShaderHDR : public Graphic::GraphicFilter
{
public:
    ShaderHDR();
    ~ShaderHDR();
    
    bool Initialize( Graphic::IGraphicContext* pGraphicContext, Graphic::UtilGraphicResourceController* resController);
    virtual VOID Render(Graphic::IGraphicContext *pGraphicContext, const Graphic::RenderContext *context, Graphic::TEXTURE_HANDLE pContent);
    inline VOID SetResourceController( Graphic::UtilGraphicResourceController* pResourceController ) { m_pResourceController = pResourceController;}
    
private:
#pragma mark - local params
    Graphic::UtilGraphicResourceController*     m_pResourceController;
    Graphic::Matrix16                           mtrx;
    Graphic::ViewPort viewport;
    Graphic::ViewPort viewportOri;
#pragma mark - shader program
    Graphic::SHADER_PROGRAM_HANDLE              m_baseProg;
    Graphic::SHADER_PROGRAM_HANDLE              m_toneProg;
    Graphic::SHADER_PROGRAM_HANDLE              m_bloomProg;
    Graphic::SHADER_PROGRAM_HANDLE              m_blurXProg;
    Graphic::SHADER_PROGRAM_HANDLE              m_blurYProg;
#pragma mark - render target
    Graphic::RENDER_TARGET_HANDLE               m_scale0Target;
    Graphic::RENDER_TARGET_HANDLE               m_scale1Target;
    Graphic::RENDER_TARGET_HANDLE               m_oldRenderTarget;
#pragma mark - texture
    Graphic::TEXTURE_HANDLE                     m_scale0;
    Graphic::TEXTURE_HANDLE                     m_scale1;
};
#endif /* FilterShaderHDR_hpp */
