//
//  BlurEffect.cpp
//  VideoPlayer
//
//  Created by Yi Huang on 25/11/15.
//  Copyright © 2015 huangyi. All rights reserved.
//

#include "BlurEffect.h"

#include "GraphicResourceMgr.h"

static const char* s_vertexShader =
"attribute vec3 a_position;"
"attribute vec2 a_tex0;"
"varying mediump vec2 v_tex0;"
"void main(void)"
"{"
    "gl_Position = vec4( a_position.xy, 0.0, 1.0 );"
    "gl_Position = sign( gl_Position );"
    "v_tex0 = (vec2( gl_Position.x, gl_Position.y ) + vec2( 1.0 ) ) / vec2( 2.0 );"
"}";

static const char* s_pixelShader =
"#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
"precision highp float;\n"
"#else\n"
"precision mediump float;\n"
"#endif\n"
"uniform vec2 u_offset;"
"uniform sampler2D u_texture;"
"varying mediump vec2 v_tex0;"
"vec4 filter(mat3 t, sampler2D tex, vec2 texCoord)"
"{"
    "vec4 color = t[0][0] * texture2D(tex, texCoord - u_offset);"
    "color += t[0][1] * texture2D(tex, vec2(texCoord.x, texCoord.y - u_offset.y));"
    "color += t[0][2] * texture2D(tex, vec2(texCoord.x + u_offset.x, texCoord.y - u_offset.y));"
    "color += t[1][0] * texture2D(tex, vec2(texCoord.x - u_offset.x, texCoord.y));"
    "color += t[1][1] * texture2D(tex, texCoord);"
    "color += t[1][2] * texture2D(tex, vec2(texCoord.x + u_offset.x, texCoord.y));"
    "color += t[2][0] * texture2D(tex, vec2(texCoord.x - u_offset.x, texCoord.y + u_offset.y));"
    "color += t[2][1] * texture2D(tex, vec2(texCoord.x, texCoord.y + u_offset.y));"
    "color += t[2][2] * texture2D(tex, texCoord + u_offset);"
    "return color;"
"}"
"void main(void)"
"{"
    "mat3 t = mat3("
        "0.0625, 0.125, 0.0625,"
        "0.125, 0.25, 0.125,"
        "0.0625, 0.125, 0.0625"
    ");"
    "gl_FragColor = filter( t, u_texture, v_tex0 );"
"}";

BlurEffect::BlurEffect()
    : m_utilResController( NULL )
    , m_pixelShader( 0 )
    , m_vertexShader( 0 )
    , m_shaderProgram( 0 )
{
}

BlurEffect::~BlurEffect()
{
    Destroy( m_pContext );
}

BOOL BlurEffect::Initialize( Graphic::IGraphicContext* pGraphicContext, Graphic::UtilGraphicResourceController* resController )
{
    m_pContext = pGraphicContext;
    m_utilResController = resController;
    m_vertexShader = pGraphicContext->CreateVertexShader( s_vertexShader, (UINT32)strlen(s_vertexShader) );
    if ( 0 == m_vertexShader )
    {
        Destroy( pGraphicContext );
        return FALSE;
    }
    m_pixelShader = pGraphicContext->CreatePixelShader( s_pixelShader, (UINT32)strlen(s_pixelShader ) );
    if ( 0 == m_pixelShader )
    {
        Destroy( pGraphicContext );
        return FALSE;
    }
    m_shaderProgram = pGraphicContext->CreateShaderProgram( m_vertexShader, m_pixelShader );
    if ( 0 == m_shaderProgram )
    {
        Destroy( pGraphicContext );
        return FALSE;
    }
    m_offset.x = 0.005f;
    m_offset.y = 0.005f;
    return TRUE;
}

VOID BlurEffect::Destroy( Graphic::IGraphicContext* pGraphicContext )
{
    if ( 0 != m_shaderProgram )
    {
        pGraphicContext->DestroyShaderProgram( m_shaderProgram );
        m_shaderProgram = 0;
    }
    if ( 0 != m_pixelShader )
    {
        pGraphicContext->DestroyPixelShader( m_pixelShader );
        m_pixelShader = 0;
    }
    if ( 0 != m_pixelShader )
    {
        pGraphicContext->DestroyVertexShader( m_pixelShader );
        m_pixelShader = 0;
    }
    m_utilResController = NULL;
    m_pContext = NULL;
}

VOID BlurEffect::Render( Graphic::IGraphicContext* pGraphicContext, const Graphic::RenderContext* context, Graphic::TEXTURE_HANDLE pContent )
{
    pGraphicContext->SetShaderProgram( m_shaderProgram );
    pGraphicContext->SetVertexShaderConstantVector( pGraphicContext->GetVertexShaderConstantLocation( m_shaderProgram, "u_offset" ), &m_offset );
    pGraphicContext->SetTexture( 0, pContent );
    pGraphicContext->SetVertexBuffer( m_utilResController->GetScreenStrchQuadVtx(), m_utilResController->GetRestoredVertexBufferFormat( Graphic::UtilGraphicResourceController::FormatType_XYZ_T ) );
    pGraphicContext->DrawPrimitive( Graphic::IGraphicContext::Primitive_TriagnleStrip, 0, 4, 2 );
}