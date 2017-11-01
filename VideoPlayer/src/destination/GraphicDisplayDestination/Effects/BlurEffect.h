//
//  BlurEffect.h
//  VideoPlayer
//
//  Created by Yi Huang on 25/11/15.
//  Copyright © 2015 huangyi. All rights reserved.
//

#if !defined ( _BLUREFFECT_H_ )
#define _BLUREFFECT_H_

#include "IGraphicContext.h"

class BlurEffect
    : public Graphic::GraphicFilter
{
public:
    BlurEffect();
    virtual ~BlurEffect();
    
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

#endif /* _BLUREFFECT_H_ */
