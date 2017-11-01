//
//  FilterShaderHDR.cpp
//  VideoPlayer
//
//  Created by wangyu on 15/9/28.
//  Copyright © 2015年 huangyi. All rights reserved.
//

#include "FilterShaderHDR.h"
#include "GraphicTypes.h"
#include "GraphicMath.h"
#include "ShaderCode_HDR.h"
#include "IGraphicContext.h"
#include "GraphicResourceMgr.h"

#pragma mark - ShaderHDR
ShaderHDR::ShaderHDR()
{
    
}

ShaderHDR::~ShaderHDR()
{
    
}

bool ShaderHDR::Initialize( Graphic::IGraphicContext* pGraphicContext, Graphic::UtilGraphicResourceController* resController)
{
    //八分之一纹理
    m_scale0 = pGraphicContext->CreateTexture(textureSize, textureSize);
    Graphic::SURFACE_HANDLE scaledepth = pGraphicContext->CreateDepthStencilSurface(textureSize, textureSize);
    m_scale0Target = pGraphicContext->CreateRenderTarget();
    pGraphicContext->SetDepthStencilSurface(m_scale0Target, scaledepth);
    pGraphicContext->SetColorSurface(m_scale0Target, pGraphicContext->GetSurfaceFromTexture(m_scale0));
    
    m_scale1 = pGraphicContext->CreateTexture(textureSize, textureSize);
    m_scale1Target = pGraphicContext->CreateRenderTarget();
    pGraphicContext->SetDepthStencilSurface(m_scale1Target, scaledepth);
    pGraphicContext->SetColorSurface(m_scale1Target, pGraphicContext->GetSurfaceFromTexture(m_scale1));
    
    //创建shader
    //baseShader,只负责填充
    Graphic::VERTEX_SHADER_HANDLE vtxShader = pGraphicContext->CreateVertexShader( s_DefaultVertexShaderString, (UINT32)strlen(s_DefaultVertexShaderString));
    if ( 0 == vtxShader )
    {
        return false;
    }
    
    Graphic::PIXEL_SHADER_HANDLE basePxlShader = pGraphicContext->CreatePixelShader(s_DefaultPixelShaderString, (UINT32)strlen(s_DefaultPixelShaderString));
    if ( 0 == basePxlShader )
    {
        return false;
    }
    m_baseProg = pGraphicContext->CreateShaderProgram(vtxShader, basePxlShader);
    
    Graphic::PIXEL_SHADER_HANDLE bloomPxlShader = pGraphicContext->CreatePixelShader(s_bloomPixelShaderString, (UINT32)strlen(s_bloomPixelShaderString));
    if ( 0 == bloomPxlShader )
    {
        return false;
    }
    m_bloomProg = pGraphicContext->CreateShaderProgram(vtxShader, bloomPxlShader);
    
    //高斯模糊
    Graphic::PIXEL_SHADER_HANDLE blurXPxlShader = pGraphicContext->CreatePixelShader(s_blurXPixelShaderString, (UINT32)strlen(s_blurXPixelShaderString));
    if ( 0 == blurXPxlShader )
    {
        return false;
    }
    m_blurXProg = pGraphicContext->CreateShaderProgram(vtxShader, blurXPxlShader);
    Graphic::PIXEL_SHADER_HANDLE blurYPxlShader = pGraphicContext->CreatePixelShader(s_blurYPixelShaderString, (UINT32)strlen(s_blurYPixelShaderString));
    if ( 0 == blurYPxlShader )
    {
        return false;
    }
    m_blurYProg = pGraphicContext->CreateShaderProgram(vtxShader, blurYPxlShader);
    
    // 合成
    Graphic::PIXEL_SHADER_HANDLE tonePxlShader = pGraphicContext->CreatePixelShader(s_toneMappingPixelShaderString, (UINT32)strlen(s_toneMappingPixelShaderString));
    if ( 0 == tonePxlShader )
    {
        return false;
    }
    m_toneProg = pGraphicContext->CreateShaderProgram(vtxShader, tonePxlShader);
    
    //设置filtercontainer的resController，必须
    SetResourceController(resController);
    return true;
}

VOID ShaderHDR::Render(Graphic::IGraphicContext *pGraphicContext, const Graphic::RenderContext *context, Graphic::TEXTURE_HANDLE pContent)
{
    //获得当前的ViewPort
    pGraphicContext->GetViewPort(viewportOri);
    //保留渲染目标
    m_oldRenderTarget = pGraphicContext->GetRenderTarget();
    //*******填充八分之一纹理
    pGraphicContext->SetRenderTarget(m_scale0Target);
    viewport.uHeight = textureSize;
    viewport.uWidth = textureSize;
    pGraphicContext->SetViewPort(viewport);
    pGraphicContext->SetShaderProgram(m_baseProg);
    Graphic::MatrixIdentity( &mtrx);
    pGraphicContext->SetTexture(0, pContent);
    pGraphicContext->SetVertexShaderConstantMatrix( pGraphicContext->GetVertexShaderConstantLocation( m_baseProg, "u_transform" ), &mtrx );
    pGraphicContext->SetVertexShaderConstantMatrix( pGraphicContext->GetVertexShaderConstantLocation( m_baseProg, "u_texTransform" ), &mtrx );
    pGraphicContext->SetVertexBuffer( m_pResourceController->GetScreenStrchQuadVtx(), m_pResourceController->GetRestoredVertexBufferFormat( Graphic::UtilGraphicResourceController::FormatType_XYZ_T ) );
    pGraphicContext->DrawPrimitive( Graphic::IGraphicContext::Primitive_TriagnleStrip, 0, 4, 2 );
    
    //bloom图层
    pGraphicContext->SetRenderTarget(m_scale1Target);
    viewport.uHeight = textureSize;
    viewport.uWidth = textureSize;
    pGraphicContext->SetViewPort(viewport);
    pGraphicContext->SetShaderProgram(m_bloomProg);
    Graphic::MatrixIdentity( &mtrx);
    pGraphicContext->SetVertexShaderConstantMatrix( pGraphicContext->GetVertexShaderConstantLocation( m_bloomProg, "u_texTransform" ), &mtrx );
    Graphic::MatrixScale( &mtrx, 1, -1, 1);
    pGraphicContext->SetVertexShaderConstantMatrix( pGraphicContext->GetVertexShaderConstantLocation( m_bloomProg, "u_transform" ), &mtrx );
    pGraphicContext->SetPixelShaderConstantInt( pGraphicContext->GetPixelShaderConstantLocation(m_bloomProg, "imgH"),textureSize);
    pGraphicContext->SetPixelShaderConstantInt( pGraphicContext->GetPixelShaderConstantLocation(m_bloomProg, "imgW"),textureSize);
    pGraphicContext->SetPixelShaderConstantFloat( pGraphicContext->GetPixelShaderConstantLocation(m_bloomProg, "AveLum"),averageLum);
    pGraphicContext->SetTexture(0, m_scale0);
    pGraphicContext->SetVertexBuffer( m_pResourceController->GetScreenStrchQuadVtx(), m_pResourceController->GetRestoredVertexBufferFormat( Graphic::UtilGraphicResourceController::FormatType_XYZ_T ) );
    pGraphicContext->DrawPrimitive( Graphic::IGraphicContext::Primitive_TriagnleStrip, 0, 4, 2 );
    
    //高斯模糊
    //X轴方向
    pGraphicContext->SetRenderTarget(m_scale0Target);
    viewport.uHeight = textureSize;
    viewport.uWidth = textureSize;
    pGraphicContext->SetViewPort(viewport);
    pGraphicContext->SetShaderProgram(m_blurXProg);
    Graphic::MatrixIdentity( &mtrx);
    pGraphicContext->SetVertexShaderConstantMatrix( pGraphicContext->GetVertexShaderConstantLocation( m_blurXProg, "u_texTransform" ), &mtrx );
    Graphic::MatrixScale( &mtrx, 1, -1, 1);
    pGraphicContext->SetVertexShaderConstantMatrix( pGraphicContext->GetVertexShaderConstantLocation( m_blurXProg, "u_transform" ), &mtrx );
    pGraphicContext->SetPixelShaderConstantInt( pGraphicContext->GetPixelShaderConstantLocation(m_blurXProg, "imgW"),textureSize);
    pGraphicContext->SetTexture(0, m_scale1);
    pGraphicContext->SetVertexBuffer( m_pResourceController->GetScreenStrchQuadVtx(), m_pResourceController->GetRestoredVertexBufferFormat( Graphic::UtilGraphicResourceController::FormatType_XYZ_T ) );
    pGraphicContext->DrawPrimitive( Graphic::IGraphicContext::Primitive_TriagnleStrip, 0, 4, 2 );

    //Y轴方向
    pGraphicContext->SetRenderTarget(m_scale1Target);
    viewport.uHeight = textureSize;
    viewport.uWidth = textureSize;
    pGraphicContext->SetViewPort(viewport);
    pGraphicContext->SetShaderProgram(m_blurYProg);
    Graphic::MatrixIdentity( &mtrx);
    pGraphicContext->SetVertexShaderConstantMatrix( pGraphicContext->GetVertexShaderConstantLocation( m_blurYProg, "u_texTransform" ), &mtrx );
    Graphic::MatrixScale( &mtrx, 1, -1, 1);
    pGraphicContext->SetVertexShaderConstantMatrix( pGraphicContext->GetVertexShaderConstantLocation( m_blurYProg, "u_transform" ), &mtrx );
    pGraphicContext->SetPixelShaderConstantInt( pGraphicContext->GetPixelShaderConstantLocation(m_blurYProg, "imgH"),textureSize);
    pGraphicContext->SetTexture(0, m_scale0);
    pGraphicContext->SetVertexBuffer( m_pResourceController->GetScreenStrchQuadVtx(), m_pResourceController->GetRestoredVertexBufferFormat( Graphic::UtilGraphicResourceController::FormatType_XYZ_T ) );
    pGraphicContext->DrawPrimitive( Graphic::IGraphicContext::Primitive_TriagnleStrip, 0, 4, 2 );

    //tone mapping
    pGraphicContext->SetRenderTarget(m_oldRenderTarget);
    pGraphicContext->SetShaderProgram(m_toneProg);
    pGraphicContext->SetViewPort(viewportOri);
    Graphic::MatrixIdentity( &mtrx);
    pGraphicContext->SetVertexShaderConstantMatrix( pGraphicContext->GetVertexShaderConstantLocation( m_toneProg, "u_texTransform" ), &mtrx );
    Graphic::MatrixScale( &mtrx, 1, -1, 1);
    pGraphicContext->SetVertexShaderConstantMatrix( pGraphicContext->GetVertexShaderConstantLocation( m_toneProg, "u_transform" ), &mtrx );
    pGraphicContext->SetPixelShaderConstantFloat( pGraphicContext->GetPixelShaderConstantLocation(m_toneProg, "Key"),1);
    pGraphicContext->SetPixelShaderConstantFloat( pGraphicContext->GetPixelShaderConstantLocation(m_toneProg, "AveLum"),lumAdapt);
    pGraphicContext->SetShaderTexture( m_toneProg, "gauss_texture", 1, m_scale1);
    pGraphicContext->SetShaderTexture( m_toneProg, "u_texture", 0, pContent);
    pGraphicContext->SetVertexBuffer( m_pResourceController->GetScreenStrchQuadVtx(), m_pResourceController->GetRestoredVertexBufferFormat( Graphic::UtilGraphicResourceController::FormatType_XYZ_T ) );
    pGraphicContext->DrawPrimitive( Graphic::IGraphicContext::Primitive_TriagnleStrip, 0, 4, 2 );
}