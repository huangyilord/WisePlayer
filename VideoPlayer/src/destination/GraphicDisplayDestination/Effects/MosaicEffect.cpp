//
//  MosaicEffect.cpp
//  VideoPlayer
//
//  Created by Yi Huang on 25/11/15.
//  Copyright © 2015 huangyi. All rights reserved.
//

#include "MosaicEffect.h"

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
    "uniform vec2 u_size;"
    "uniform sampler2D u_texture;"
    "varying mediump vec2 v_tex0;"
    "void main(void)"
    "{"
        "vec2 mosaic = vec2( int(v_tex0.x / u_size.x), int(v_tex0.y / u_size.y) );"
        "mosaic = mosaic * u_size;"
        "gl_FragColor = texture2D(u_texture, mosaic);"
    "}";

MosaicEffect::MosaicEffect()
    : m_utilResController( NULL )
    , m_pixelShader( 0 )
    , m_vertexShader( 0 )
    , m_shaderProgram( 0 )
{
    
}

MosaicEffect::~MosaicEffect()
{
    Destroy( m_pContext );
}

BOOL MosaicEffect::Initialize( Graphic::IGraphicContext* pGraphicContext, Graphic::UtilGraphicResourceController* resController )
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
    m_size.x = 0.02;
    m_size.y = 0.02;
    return TRUE;
}

VOID MosaicEffect::Destroy( Graphic::IGraphicContext* pGraphicContext )
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

VOID MosaicEffect::Render( Graphic::IGraphicContext* pGraphicContext, const Graphic::RenderContext* context, Graphic::TEXTURE_HANDLE pContent )
{
    pGraphicContext->SetShaderProgram( m_shaderProgram );
    pGraphicContext->SetVertexShaderConstantVector( pGraphicContext->GetVertexShaderConstantLocation( m_shaderProgram, "u_size" ), &m_size );
    pGraphicContext->SetTexture( 0, pContent );
    pGraphicContext->SetVertexBuffer( m_utilResController->GetScreenStrchQuadVtx(), m_utilResController->GetRestoredVertexBufferFormat( Graphic::UtilGraphicResourceController::FormatType_XYZ_T ) );
    pGraphicContext->DrawPrimitive( Graphic::IGraphicContext::Primitive_TriagnleStrip, 0, 4, 2 );
}