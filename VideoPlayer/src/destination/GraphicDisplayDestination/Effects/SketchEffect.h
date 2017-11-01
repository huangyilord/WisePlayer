//
//  SketchEffect.hpp
//  VideoPlayer
//
//  Created by Yi Huang on 25/11/15.
//  Copyright © 2015 huangyi. All rights reserved.
//

#if !defined ( _SKETCH_EFFECT_H_ )
#define _SKETCH_EFFECT_H_

#include "IGraphicContext.h"

class SketchEffect
    : public Graphic::GraphicFilter
{
public:
    SketchEffect();
    virtual ~SketchEffect();
    
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

#endif /* _SKETCH_EFFECT_H_ */
