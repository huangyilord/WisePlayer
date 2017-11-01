//
//  SharpeningEffect.h
//  VideoPlayer
//
//  Created by Yi Huang on 25/11/15.
//  Copyright © 2015 huangyi. All rights reserved.
//

#if !defined ( _SHARPENING_EFFECT_H_ )
#define _SHARPENING_EFFECT_H_

#include "IGraphicContext.h"

class SharpeningEffect
    : public Graphic::GraphicFilter
{
public:
    SharpeningEffect();
    virtual ~SharpeningEffect();

    BOOL Initialize( Graphic::IGraphicContext* pGraphicContext, Graphic::UtilGraphicResourceController* resController );
    VOID Destroy( Graphic::IGraphicContext* pGraphicContext );

    virtual VOID Render( Graphic::IGraphicContext* pGraphicContext, const Graphic::RenderContext* context, Graphic::TEXTURE_HANDLE pContent );
private:
    Graphic::IGraphicContext*               m_pContext;
    Graphic::UtilGraphicResourceController* m_utilResController;
    Graphic::PIXEL_SHADER_HANDLE            m_pixelShader;
    Graphic::VERTEX_SHADER_HANDLE           m_vertexShader;
    Graphic::SHADER_PROGRAM_HANDLE          m_shaderProgram;
    Graphic::Vector2                        m_offset;
};

#endif /* _SHARPENING_EFFECT_H_ */
