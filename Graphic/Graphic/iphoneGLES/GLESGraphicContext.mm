#include "GLESGraphicContext.h"

#import <UIKit/UIKit.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <QuartzCore/QuartzCore.h>

#include "InternalTypes.h"

namespace Graphic
{
    GLESGraphicContext::GLESGraphicContext()
        : m_pContext( NULL )
        , m_renderTarget( NULL )
        , m_depthSurface( NULL )
        , m_colorSurface( NULL )
        , m_pVertexFormat( NULL )
        , m_VertexAttributeFlag( 0 )
    {
    }

    GLESGraphicContext::~GLESGraphicContext()
    {
        Destroy();
    }

    BOOL GLESGraphicContext::Initialize( VOID* pView )
    {
        EAGLContext* pContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if ( nil == pContext )
        {
            return FALSE;
        }
        [EAGLContext setCurrentContext:pContext];
        m_pContext = pContext;

        // Create default framebuffer object.
        UINT32 frameBufferID = 0;
        glGenFramebuffers( 1, &frameBufferID );
        if ( 0 == frameBufferID )
        {
            Destroy();
            return FALSE;
        }
        GLESFrameBuffer* pFrameBuffer = new GLESFrameBuffer( frameBufferID );
        if ( NULL == pFrameBuffer )
        {
            glDeleteFramebuffers( 1, &frameBufferID );
            Destroy();
            return FALSE;
        }
        glBindFramebuffer( GL_FRAMEBUFFER, frameBufferID );

        // Create color render buffer and allocate backing store.
        UINT32 bufferID = 0;
        glGenRenderbuffers( 1, &bufferID );
        if ( 0 == bufferID )
        {
            Destroy();
            return FALSE;
        }
        UIView* view = (UIView*)pView;
        CGRect frame = view.frame;
        GLESRenderSurface* renderSurface = new GLESRenderSurface( bufferID, frame.size.width, frame.size.height );
        if ( NULL == renderSurface )
        {
            glDeleteRenderbuffers( 1, &bufferID );
            Destroy();
            return FALSE;
        }
        glBindRenderbuffer( GL_RENDERBUFFER, bufferID );
        [pContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:( id<EAGLDrawable> )view.layer];
        pFrameBuffer->SetColorSurface( renderSurface );
        m_colorSurface = renderSurface;

        // Create depth stencil
        glGenRenderbuffers( 1, &bufferID );
        if ( 0 == bufferID )
        {
            Destroy();
            return FALSE;
        }
        renderSurface = new GLESRenderSurface( bufferID, frame.size.width, frame.size.height );
        if ( NULL == renderSurface )
        {
            glDeleteRenderbuffers( 1, &bufferID );
            Destroy();
            return FALSE;
        }
        glBindRenderbuffer( GL_RENDERBUFFER, bufferID );
        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, view.frame.size.width, view.frame.size.height );
        pFrameBuffer->SetDepthSurface( renderSurface );
        m_depthSurface = renderSurface;

        m_renderTarget = pFrameBuffer;

        // Create default shader
        m_defaultVertexShader = CreateVertexShader( s_DefaultVertexShaderString, (UINT32)strlen(s_DefaultVertexShaderString) );
        if ( 0 == m_defaultVertexShader )
        {
            Destroy();
            return FALSE;
        }
        m_defaultPixelShader = CreatePixelShader( s_DefaultPixelShaderString, (UINT32)strlen(s_DefaultPixelShaderString) );
        if ( 0 == m_defaultPixelShader )
        {
            Destroy();
            return FALSE;
        }
        m_defaultShaderProgram = CreateShaderProgram( m_defaultVertexShader, m_defaultPixelShader );
        if ( 0 == m_defaultShaderProgram )
        {
            Destroy();
            return FALSE;
        }
        ResetShaderProgram();
        return TRUE;
    }

    VOID GLESGraphicContext::Destroy()
    {
        DestroyShaderProgram( m_defaultShaderProgram );
        DestroyPixelShader( m_defaultPixelShader );
        DestroyVertexShader( m_defaultVertexShader );
        if ( 0 != m_depthSurface )
        {
            delete (GLESRenderSurface*)m_depthSurface;
            m_depthSurface = 0;
        }

        if ( 0 != m_colorSurface )
        {
            delete (GLESRenderSurface*)m_colorSurface;
            m_colorSurface = 0;
        }

        if ( 0 != m_renderTarget )
        {
            delete (GLESFrameBuffer*)m_renderTarget;
            m_renderTarget = 0;
        }

        if ( NULL != m_pContext )
        {
            [(EAGLContext*)m_pContext release];
            m_pContext = NULL;
        }
    }

    BOOL GLESGraphicContext::AdjustFrameBuffer( VOID* pView )
    {
        EAGLContext* pContext = (EAGLContext*)m_pContext;
        GLESFrameBuffer* pFrameBuffer = (GLESFrameBuffer*)m_renderTarget;

        if ( NULL != m_colorSurface )
        {
            GLESRenderSurface* renderSurface = (GLESRenderSurface*)m_colorSurface;
            delete renderSurface;
            m_colorSurface = NULL;
        }
        if ( NULL != m_depthSurface )
        {
            GLESRenderSurface* renderSurface = (GLESRenderSurface*)m_depthSurface;
            delete renderSurface;
            m_depthSurface = NULL;
        }
        // Create color render buffer and allocate backing store.
        UINT32 bufferID = 0;
        glGenRenderbuffers( 1, &bufferID );
        if ( 0 == bufferID )
        {
            return FALSE;
        }
        UIView* view = (UIView*)pView;
        GLESRenderSurface* renderSurface = new GLESRenderSurface( bufferID, view.frame.size.width, view.frame.size.height );
        if ( NULL == renderSurface )
        {
            glDeleteRenderbuffers( 1, &bufferID );
            return FALSE;
        }
        glBindRenderbuffer( GL_RENDERBUFFER, bufferID );
        [pContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:( id<EAGLDrawable> )view.layer];
        pFrameBuffer->SetColorSurface( renderSurface );
        m_colorSurface = renderSurface;
        
        // Create depth stencil
        glGenRenderbuffers( 1, &bufferID );
        if ( 0 == bufferID )
        {
            return FALSE;
        }
        renderSurface = new GLESRenderSurface( bufferID, view.frame.size.width, view.frame.size.height );
        if ( NULL == renderSurface )
        {
            glDeleteRenderbuffers( 1, &bufferID );
            return FALSE;
        }
        glBindRenderbuffer( GL_RENDERBUFFER, bufferID );
        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, view.frame.size.width, view.frame.size.height );
        pFrameBuffer->SetDepthSurface( renderSurface );
        m_depthSurface = renderSurface;

        return TRUE;
    }

    VERTEX_BUFFER_FORMAT_HANDLE GLESGraphicContext::CreateVertexBufferFormat( const VertexElement* elements, UINT32 count )
    {
        GLESVertexBufferFormat* pFormat = new GLESVertexBufferFormat();
        if ( NULL == pFormat )
        {
            return NULL;
        }
        if ( !pFormat->Create(elements, count) )
        {
            delete pFormat;
            return NULL;
        }
        return pFormat;
    }

    VOID GLESGraphicContext::DestroyVertexBufferFormat( VERTEX_BUFFER_FORMAT_HANDLE format )
    {
        GLESVertexBufferFormat* pFormat = (GLESVertexBufferFormat*)format;
        delete pFormat;
    }

    VERTEX_BUFFER_HANDLE GLESGraphicContext::CreateVertexBuffer( UINT32 size )
    {
        GLESVertexBuffer* pBuffer = new GLESVertexBuffer();
        if ( NULL == pBuffer )
        {
            return NULL;
        }
        if ( !pBuffer->Create( size ) )
        {
            delete pBuffer;
            return NULL;
        }
        return pBuffer;
    }

    VOID* GLESGraphicContext::GetVertexBufferPointer( VERTEX_BUFFER_HANDLE vtxHandle )
    {
        GLESVertexBuffer* pBuffer = (GLESVertexBuffer*)vtxHandle;
        return pBuffer->GetDataPointer();
    }

    BOOL GLESGraphicContext::CommitVertexBuffer( VOID* pBuffer, VERTEX_BUFFER_HANDLE vtxHandle )
    {
        GLESVertexBuffer* pVtxBuffer = (GLESVertexBuffer*)vtxHandle;
        return pVtxBuffer->CommitBufferData();
    }

    VOID GLESGraphicContext::DestroyVertexBuffer( VERTEX_BUFFER_HANDLE vtxHandle )
    {
        GLESVertexBuffer* pVtxBuffer = (GLESVertexBuffer*)vtxHandle;
        delete pVtxBuffer;
    }

    INDEX_BUFFER_HANDLE GLESGraphicContext::CreateIndexBuffer( UINT32 size )
    {
        GLESIndexBuffer* pBuffer = new GLESIndexBuffer();
        if ( NULL == pBuffer )
        {
            return NULL;
        }
        if ( !pBuffer->Create( size ) )
        {
            delete pBuffer;
            return NULL;
        }
        return pBuffer;
    }

    UINT16* GLESGraphicContext::GetIndexBufferPointer( INDEX_BUFFER_HANDLE idxHandle )
    {
        GLESIndexBuffer* pBuffer = (GLESIndexBuffer*)idxHandle;
        return pBuffer->GetDataPointer();
    }

    BOOL GLESGraphicContext::CommitIndexBuffer( UINT16* pBuffer, INDEX_BUFFER_HANDLE idxHandle )
    {
        GLESIndexBuffer* pIdxBuffer = (GLESIndexBuffer*)idxHandle;
        return pIdxBuffer->CommitBufferData();
    }

    VOID GLESGraphicContext::DestroyIndexBuffer( INDEX_BUFFER_HANDLE idxHandle )
    {
        GLESIndexBuffer* pIdxBuffer = (GLESIndexBuffer*)idxHandle;
        delete pIdxBuffer;
    }

    SURFACE_DATA_HANDLE GLESGraphicContext::GetRenderTargetData( UINT32 x, UINT32 y, UINT32 width, UINT32 height )
    {
        UINT8* data = new UINT8[width * height * sizeof(UINT32)];
        if ( NULL == data )
        {
            return NULL;
        }
        glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        return data;
    }

    VOID* GLESGraphicContext::GetSurfaceDataPointer( SURFACE_DATA_HANDLE dataHandle )
    {
        return dataHandle;
    }

    VOID GLESGraphicContext::ReleaseSurfaceData( SURFACE_DATA_HANDLE dataHandle )
    {
        delete [] (UINT8*)dataHandle;
    }

    VOID GLESGraphicContext::GetSurfaceSize( SURFACE_HANDLE surface, UINT32 &width, UINT32 &height )
    {
        IGLESSurface* pRenderSurface = (IGLESSurface*)surface;
        width = pRenderSurface->GetWidth();
        height = pRenderSurface->GetHeight();
    }

    TEXTURE_HANDLE GLESGraphicContext::CreateTexture( UINT32 width, UINT32 height )
    {
        UINT32 texID = 0;
        glGenTextures( 1, &texID );
        if ( 0 == texID )
        {
            return NULL;
        }
        GLESTexture* pTexture = new GLESTexture( texID, width, height, GL_UNSIGNED_BYTE );
        if ( NULL == pTexture )
        {
            glDeleteTextures( 1, &texID );
            return NULL;
        }
        glBindTexture( GL_TEXTURE_2D, texID );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        return pTexture;
    }

    TEXTURE_HANDLE GLESGraphicContext::CreateTexture( const VOID* data, UINT32 width, UINT32 height, PixelType pixelType )
    {
        UINT32 pixelFormat = GL_UNSIGNED_BYTE;
        switch ( pixelType )
        {
            case PixelType_32_8888:
                pixelFormat = GL_UNSIGNED_BYTE;
                break;
            case PixelType_16_4444:
                pixelFormat = GL_UNSIGNED_SHORT_4_4_4_4;
                break;
            case PixelType_16_565:
                pixelFormat = GL_UNSIGNED_SHORT_5_6_5;
                break;
            case PixelType_16_5551:
                pixelFormat = GL_UNSIGNED_SHORT_5_5_5_1;
                break;
            default:
                return NULL;
        }
        UINT32 texID = 0;
        glGenTextures( 1, &texID );
        if ( 0 == texID )
        {
            return NULL;
        }
        GLESTexture* pTexture = new GLESTexture( texID, width, height, pixelFormat );
        if ( NULL == pTexture )
        {
            glDeleteTextures( 1, &texID );
            return NULL;
        }
        glBindTexture( GL_TEXTURE_2D, texID );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, pixelFormat, data );                        // Build Texture Using Information In data
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        return pTexture;
    }

    VOID GLESGraphicContext::TextureSubImage( TEXTURE_HANDLE texture, const VOID* data, UINT32 width, UINT32 height, UINT32 x, UINT32 y )
    {
        GLESTexture* pTexture = (GLESTexture*)texture;
        glBindTexture( GL_TEXTURE_2D, pTexture->GetTexID() );
        glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, pTexture->GetPixelFormat(), data );
    }

    VOID GLESGraphicContext::DestroyTexture( TEXTURE_HANDLE texture )
    {
        GLESTexture* pTexture = (GLESTexture*)texture;
        delete pTexture;
    }

    VOID GLESGraphicContext::EnableTexture( UINT32 stage )
    {
        glActiveTexture( GL_TEXTURE0 + stage );
        glEnable( GL_TEXTURE_2D );
    }

    VOID GLESGraphicContext::DisableTexture( UINT32 stage )
    {
        glActiveTexture( GL_TEXTURE0 + stage );
        glDisable( GL_TEXTURE_2D );
    }

    VOID GLESGraphicContext::SetTexture( UINT32 stage, TEXTURE_HANDLE texture )
    {
        UINT32 texID = 0;
        if ( NULL != texture )
        {
            GLESTexture* pTexture = (GLESTexture*)texture;
            texID = pTexture->GetTexID();
        }
        glActiveTexture( GL_TEXTURE0 + stage );
        glBindTexture( GL_TEXTURE_2D, texID );
    }

    SURFACE_HANDLE GLESGraphicContext::GetSurfaceFromTexture( TEXTURE_HANDLE texture )
    {
        GLESTexture* pTexture = (GLESTexture*)texture;
        return pTexture->GetSurface();
    }

    RENDER_TARGET_HANDLE GLESGraphicContext::CreateRenderTarget()
    {
        UINT32 bufferID = 0;
        glGenFramebuffers( 1, &bufferID );
        if ( 0 == bufferID )
        {
            return NULL;
        }
        GLESFrameBuffer* pBuffer = new GLESFrameBuffer( bufferID );
        if ( NULL == pBuffer )
        {
            glDeleteFramebuffers( 1, &bufferID );
            return NULL;
        }
        return pBuffer;
    }

    VOID GLESGraphicContext::DestroyRenderTarget( RENDER_TARGET_HANDLE target )
    {
        GLESFrameBuffer* pFrameBuffer = (GLESFrameBuffer*)target;
        delete pFrameBuffer;
    }

    VOID GLESGraphicContext::SetRenderTarget( RENDER_TARGET_HANDLE target )
    {
        if ( m_renderTarget != target )
        {
            GLESFrameBuffer* pFrameBuffer = (GLESFrameBuffer*)target;
            glBindFramebuffer( GL_FRAMEBUFFER, pFrameBuffer->GetFrameBufferID() );
            m_renderTarget = target;
        }
    }

    RENDER_TARGET_HANDLE GLESGraphicContext::GetRenderTarget()
    {
        return m_renderTarget;
    }

    SURFACE_HANDLE GLESGraphicContext::CreateDepthStencilSurface( UINT32 width, UINT32 height )
    {
        UINT32 bufferID = 0;
        glGenRenderbuffers( 1, &bufferID );
        if ( 0 == bufferID )
        {
            return NULL;
        }
        GLESRenderSurface* renderSurface = new GLESRenderSurface( bufferID, width, height );
        if ( NULL == renderSurface )
        {
            glDeleteRenderbuffers( 1, &bufferID );
            return NULL;
        }
        glBindRenderbuffer( GL_RENDERBUFFER, bufferID );
        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, width, height );
        return renderSurface;
    }

    VOID GLESGraphicContext::DestroyDepthStencilSurface( SURFACE_HANDLE surface )
    {
        GLESRenderSurface* pRenderSurface = (GLESRenderSurface*)surface;
        pRenderSurface->Release();
        delete pRenderSurface;
    }

    VOID GLESGraphicContext::SetDepthStencilSurface( RENDER_TARGET_HANDLE target, SURFACE_HANDLE surface )
    {
        GLESFrameBuffer* pFrameBuffer = (GLESFrameBuffer*)target;
        glBindFramebuffer( GL_FRAMEBUFFER, pFrameBuffer->GetFrameBufferID() );
        pFrameBuffer->SetDepthSurface( (IGLESSurface*)surface );
        if ( NULL != m_renderTarget && target != m_renderTarget )
        {
            GLESFrameBuffer* pCurrentFrameBuffer = (GLESFrameBuffer*)m_renderTarget;
            glBindFramebuffer( GL_FRAMEBUFFER, pCurrentFrameBuffer->GetFrameBufferID() );
        }
    }

    SURFACE_HANDLE GLESGraphicContext::GetDepthStencilSurface( RENDER_TARGET_HANDLE target )
    {
        GLESFrameBuffer* pFrameBuffer = (GLESFrameBuffer*)target;
        return pFrameBuffer->GetDepthSurface();
    }

    VOID GLESGraphicContext::SetColorSurface( RENDER_TARGET_HANDLE target, SURFACE_HANDLE surface )
    {
        GLESFrameBuffer* pFrameBuffer = (GLESFrameBuffer*)target;
        glBindFramebuffer( GL_FRAMEBUFFER, pFrameBuffer->GetFrameBufferID() );
        pFrameBuffer->SetColorSurface( (IGLESSurface*)surface );
        if ( NULL != m_renderTarget && target != m_renderTarget )
        {
            GLESFrameBuffer* pCurrentFrameBuffer = (GLESFrameBuffer*)m_renderTarget;
            glBindFramebuffer( GL_FRAMEBUFFER, pCurrentFrameBuffer->GetFrameBufferID() );
        }
    }

    SURFACE_HANDLE GLESGraphicContext::GetColorSurface( RENDER_TARGET_HANDLE target )
    {
        GLESFrameBuffer* pFrameBuffer = (GLESFrameBuffer*)target;
        return pFrameBuffer->GetColorSurface();
    }

    VOID GLESGraphicContext::EnableDepthTest( BOOL bEnable )
    {
        if ( bEnable )
        {
            glEnable( GL_DEPTH_TEST );
        }
        else
        {
            glDisable( GL_DEPTH_TEST );
        }
    }

    VOID GLESGraphicContext::SetDepthTestFunc( DepthTestFunction func )
    {
        static const UINT32 s_GLDepthFunc[] =
        {
            GL_NEVER
            , GL_LESS
            , GL_EQUAL
            , GL_LEQUAL
            , GL_GREATER
            , GL_NOTEQUAL
            , GL_GEQUAL
            , GL_ALWAYS
        };
        glDepthFunc( s_GLDepthFunc[func] );
    }

    VOID GLESGraphicContext::DepthBufferWriteEnable( BOOL bEnable )
    {
        glDepthMask( bEnable );
    }

    VOID GLESGraphicContext::ColorWriteEnable( BOOL bRedEnable, BOOL bGreenEnable, BOOL bBlueEnable, BOOL bAlphaEnable )
    {
        glColorMask( bRedEnable, bGreenEnable, bBlueEnable, bAlphaEnable );
    }

    VOID GLESGraphicContext::EnableBlend( BOOL bEnable )
    {
        if ( bEnable )
        {
            glEnable( GL_BLEND );
        }
        else
        {
            glDisable( GL_BLEND );
        }
    }

    VOID GLESGraphicContext::SetBlendFunc( BlendFunction src, BlendFunction dst )
    {
        static const UINT32 s_GLBlendFunc[] = 
        {
            GL_ZERO
            , GL_ONE
            , GL_SRC_COLOR
            , GL_ONE_MINUS_SRC_COLOR
            , GL_SRC_ALPHA
            , GL_ONE_MINUS_SRC_ALPHA
            , GL_DST_COLOR
            , GL_ONE_MINUS_DST_COLOR
            , GL_DST_ALPHA
            , GL_ONE_MINUS_DST_ALPHA
        };
        glBlendFunc( s_GLBlendFunc[src], s_GLBlendFunc[dst] );
    }

    VOID GLESGraphicContext::SetCullMode( CullMode mode )
    {
        const static UINT32 s_GLCullMode[] = {
            GL_BACK
            , GL_FRONT
        };
	    glCullFace( s_GLCullMode[mode] );
    }

    VOID GLESGraphicContext::EnableCullFace( BOOL bEnable )
    {
        if ( bEnable )
        {
            glEnable( GL_CULL_FACE );
        }
        else
        {
			glDisable( GL_CULL_FACE );
        }
    }

    VOID GLESGraphicContext::EnableStencilTest( BOOL bEnable )
    {
        if ( bEnable )
        {
            glEnable( GL_STENCIL_TEST );
        }
        else
        {
            glDisable( GL_STENCIL_TEST );
        }
    }

    VOID GLESGraphicContext::SetStencilOperation( StencilOperation fail, StencilOperation zFail, StencilOperation pass )
    {
        static const UINT32 s_GLStencilOp[] =
        {
            GL_KEEP
            , GL_ZERO
            , GL_REPLACE
            , GL_INCR
            , GL_DECR
            , GL_INVERT
        };
        glStencilOp( s_GLStencilOp[fail], s_GLStencilOp[zFail], s_GLStencilOp[pass] );
    }

    VOID GLESGraphicContext::SetStencilTestFunc( StencilTestFunction func, UINT32 ref, UINT32 mask )
    {
        static const UINT32 s_GLStencilFunc[] =
        {
            GL_NEVER
            , GL_LESS
            , GL_EQUAL
            , GL_LEQUAL
            , GL_GREATER
            , GL_NOTEQUAL
            , GL_GEQUAL
            , GL_ALWAYS
        };
        glStencilFunc( s_GLStencilFunc[func], ref, mask );
    }

    VOID GLESGraphicContext::SetStencilWriteMask( UINT32 mask )
    {
        glStencilMask( mask );
    }

    VOID GLESGraphicContext::SetViewPort( const ViewPort& viewport )
    {
        m_viewport = viewport;
        glViewport( m_viewport.uMinX, m_viewport.uMinY, m_viewport.uWidth, m_viewport.uHeight );
    }

    VOID GLESGraphicContext::GetViewPort( ViewPort& viewport )
    {
        viewport = m_viewport;
    }

    VOID GLESGraphicContext::SetVertexBuffer( VERTEX_BUFFER_HANDLE vtxHandle, VERTEX_BUFFER_FORMAT_HANDLE format )
    {
        GLESVertexBuffer* pVtxBuffer = (GLESVertexBuffer*)vtxHandle;
        glBindBuffer( GL_ARRAY_BUFFER, pVtxBuffer->GetID() );
        m_pVertexFormat = format;
        if ( NULL != m_pVertexFormat )
        {
            UINT32 vertexAttributeFlag = 0;
            GLESVertexBufferFormat* pFormat = (GLESVertexBufferFormat*)format;
            UINT32 count = pFormat->GetElementCount();
            for ( UINT32 i = 0; i < count; ++i )
            {
                const GLESVertexBufferFormat::GLESVertexElementDesc& desc = pFormat->GetElement( i );
                glVertexAttribPointer(desc.index, desc.size, desc.type, GL_FALSE, pFormat->GetStride(), (GLvoid*)(UINT32)desc.offset);
                vertexAttributeFlag |= (1 << desc.index);
            }
            if ( m_VertexAttributeFlag != vertexAttributeFlag )
            {
                for ( UINT32 i = 0; i < DataUsage_Num; ++i )
                {
                    if ( (m_VertexAttributeFlag & (1 << i)) != (vertexAttributeFlag & (1 << i)))
                    {
                        if ( vertexAttributeFlag & (1 << i) )
                        {
                            glEnableVertexAttribArray( i );
                        }
                        else
                        {
                            glDisableVertexAttribArray( i );
                        }
                    }
                }
                m_VertexAttributeFlag = vertexAttributeFlag;
            }
        }
    }

    VOID GLESGraphicContext::SetIndexBuffer( INDEX_BUFFER_HANDLE idxHandle )
    {
        GLESIndexBuffer* pIdxBuffer = (GLESIndexBuffer*)idxHandle;
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pIdxBuffer->GetID() );
    }

    static const UINT32 s_GLPrimitiveType[] =
    {
        GL_TRIANGLES
        , GL_TRIANGLE_STRIP
        , GL_TRIANGLE_FAN
    };
    VOID GLESGraphicContext::DrawPrimitive( PrimitiveType type, UINT32 startVtx, UINT32 vtxCount, UINT32 primitiveCount )
    {
        glDrawArrays( s_GLPrimitiveType[type], startVtx, vtxCount );
    }

    VOID GLESGraphicContext::DrawIndexedPrimitive( PrimitiveType type, UINT32 startVtx, UINT32 vtxCount, UINT32 startIdx, UINT32 idxCount, UINT32 primitiveCount )
    {
        glDrawElements( s_GLPrimitiveType[type], idxCount, GL_UNSIGNED_SHORT, NULL );
    }

    BOOL GLESGraphicContext::BeginScene()
    {
        return TRUE;
    }

    VOID GLESGraphicContext::EndScene()
    {
    }

    VOID GLESGraphicContext::Clear( UINT32 flags, ColorF_RGBA& color, FLOAT32 depth, DWORD stencil )
    {
        UINT32 glClearFlag = 0;
        if ( flags & Clear_Color )
        {
            glClearFlag |= GL_COLOR_BUFFER_BIT;
            glClearColor( color.r, color.g, color.b, color.a );
        }
        if ( flags & Clear_Depth )
        {
            glClearFlag |= GL_DEPTH_BUFFER_BIT;
            glClearDepthf( depth );
        }
        if ( flags & Clear_Stencil )
        {
            glClearFlag |= GL_STENCIL_BUFFER_BIT;
            glClearStencil( stencil );
        }
        glClear( glClearFlag );
    }

    VOID GLESGraphicContext::Present()
    {
        GLESFrameBuffer* pFrameBuffer = (GLESFrameBuffer*)m_renderTarget;
        GLESRenderSurface* pRenderSurface = (GLESRenderSurface*)pFrameBuffer->GetColorSurface();
        glBindRenderbuffer( GL_RENDERBUFFER, pRenderSurface->GetBufferID() );
        [(EAGLContext*)m_pContext presentRenderbuffer:GL_RENDERBUFFER];
    }

    VERTEX_SHADER_HANDLE GLESGraphicContext::CreateVertexShader( const char* input, UINT32 len )
    {
        VERTEX_SHADER_HANDLE shader = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( shader, 1, &input, NULL );
        glCompileShader( shader );
        
#if defined ( DEBUG )
        GLint logLength;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength);
        if ( logLength > 0 )
        {
            GLchar *log = (GLchar *)malloc( logLength + 1 );
            glGetShaderInfoLog( shader, logLength, &logLength, log );
            NSLog( @"Shader compile log:\n%s", log );
            free( log );
        }
#endif
        GLint status;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &status);
        if ( status == 0 )
        {
            glDeleteShader( shader );
            shader = 0;
        }
        return shader;
    }

    VERTEX_SHADER_HANDLE GLESGraphicContext::CreateVertexShader( const void* pBuffer, UINT32 size )
    {
        VERTEX_SHADER_HANDLE shader = 0;
        glShaderBinary( 1, &shader, GL_VERTEX_SHADER, pBuffer, size );
        return shader;
    }

    VOID GLESGraphicContext::DestroyVertexShader( VERTEX_SHADER_HANDLE shader )
    {
        glDeleteShader( shader );
    }

    PIXEL_SHADER_HANDLE GLESGraphicContext::CreatePixelShader( const char* input, UINT32 len )
    {
        PIXEL_SHADER_HANDLE shader = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( shader, 1, &input, NULL );
        glCompileShader( shader );

#if defined ( DEBUG )
        GLint logLength;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength);
        if ( logLength > 0 )
        {
            GLchar *log = (GLchar *)malloc( logLength + 1 );
            glGetShaderInfoLog( shader, logLength, &logLength, log );
            NSLog( @"Shader compile log:\n%s", log );
            free(log);
        }
#endif
        GLint status;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &status );
        if ( status == 0 )
        {
            glDeleteShader( shader );
            shader = 0;
        }
        return shader;
    }

    PIXEL_SHADER_HANDLE GLESGraphicContext::CreatePixelShader( const void* pBuffer, UINT32 size )
    {
        PIXEL_SHADER_HANDLE shader = 0;
        glShaderBinary( 1, &shader, GL_FRAGMENT_SHADER, pBuffer, size );
        return shader;
    }

    VOID GLESGraphicContext::DestroyPixelShader( PIXEL_SHADER_HANDLE shader )
    {
        glDeleteShader( shader );
    }

    SHADER_PROGRAM_HANDLE GLESGraphicContext::CreateShaderProgram( VERTEX_SHADER_HANDLE vtxShader, PIXEL_SHADER_HANDLE pxlShader )
    {
        SHADER_PROGRAM_HANDLE program = glCreateProgram();
        if ( 0 == program )
        {
            return 0;
        }
        glAttachShader( program, vtxShader );
        glAttachShader( program, pxlShader );

        static const char* s_attributeNames[DataUsage_Num] =
        {
            "a_position"
            , "a_blendWeight"
            , "a_blendIndex"
            , "a_normal"
            , "a_color0"
            , "a_color1"
            , "a_tex0"
            , "a_tex1"
        };

        for ( UINT32 i = 0; i < IGraphicContext::DataUsage_Num; ++i )
        {
            glBindAttribLocation( program, i, s_attributeNames[i] );
        }
        glLinkProgram( program );
#if defined(DEBUG)
        GLint logLength;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logLength );
        if ( logLength > 0 )
        {
            GLchar *log = (GLchar*)malloc( logLength + 1 );
            glGetProgramInfoLog( program, logLength, &logLength, log );
            NSLog( @"Program link log:\n%s", log );
            free( log );
        }
#endif
        GLint status;
        glGetProgramiv( program, GL_LINK_STATUS, &status );
        if ( status == 0 )
        {
            glDeleteProgram( program );
            return 0;
        }
        return program;
    }

    VOID GLESGraphicContext::SetShaderProgram( SHADER_PROGRAM_HANDLE program )
    {
        m_currentShaderProgram = program;
        glUseProgram( program );
    }

    SHADER_PROGRAM_HANDLE GLESGraphicContext::GetShaderProgram()
    {
        return m_currentShaderProgram;
    }

    VOID GLESGraphicContext::ResetShaderProgram()
    {
        m_currentShaderProgram = m_defaultShaderProgram;
        glUseProgram( m_defaultShaderProgram );
    }

    VOID GLESGraphicContext::DestroyShaderProgram( SHADER_PROGRAM_HANDLE program )
    {
        glDeleteProgram( program );
    }

    UINT32 GLESGraphicContext::GetVertexShaderConstantLocation( SHADER_PROGRAM_HANDLE program, const char* name )
    {
        return glGetUniformLocation( program, name );
    }

    VOID GLESGraphicContext::SetVertexShaderConstantFloat( UINT32 location, FLOAT32 value )
    {
        glUniform1f( location, value );
    }

    VOID GLESGraphicContext::SetVertexShaderConstantFloatArray( UINT32 location, const FLOAT32* value, UINT32 count )
    {
        glUniform1fv( location, count, value );
    }

    VOID GLESGraphicContext::SetVertexShaderConstantInt( UINT32 location, INT32 value )
    {
        glUniform1i( location, value );
    }

    VOID GLESGraphicContext::SetVertexShaderConstantIntArray( UINT32 location, const INT32* value, UINT32 count )
    {
        glUniform1iv( location, count, (const GLint*)value );
    }

    VOID GLESGraphicContext::SetVertexShaderConstantMatrix( UINT32 location, const Matrix16* value )
    {
        glUniformMatrix4fv( location, 1, GL_FALSE, (const GLfloat*)value );
    }

    VOID GLESGraphicContext::SetVertexShaderConstantMatrix( UINT32 location, const Matrix9* value )
    {
        glUniformMatrix3fv( location, 1, GL_FALSE, (const GLfloat*)value );
    }

    VOID GLESGraphicContext::SetVertexShaderConstantMatrixArray( UINT32 location, const Matrix16* value, UINT32 count )
    {
        glUniformMatrix4fv( location, count, GL_FALSE, (const GLfloat*)value );
    }

    VOID GLESGraphicContext::SetVertexShaderConstantVector( UINT32 location, const Vector4* value )
    {
        glUniform4f( location, value->x, value->y, value->z, value->w );
    }

    VOID GLESGraphicContext::SetVertexShaderConstantVectorArray( UINT32 location, const Vector4* value, UINT32 count )
    {
        glUniform4fv( location, count, (const GLfloat*)value );
    }

    VOID GLESGraphicContext::SetVertexShaderConstantVector( UINT32 location, const Vector3* value )
    {
        glUniform3f( location, value->x, value->y, value->z );
    }

    VOID GLESGraphicContext::SetVertexShaderConstantVector( UINT32 location, const Vector2* value )
    {
        glUniform2f( location, value->x, value->y );
    }

    UINT32 GLESGraphicContext::GetPixelShaderConstantLocation( SHADER_PROGRAM_HANDLE program, const char* name )
    {
        return glGetUniformLocation( program, name );
    }
    
    VOID GLESGraphicContext::SetPixelShaderConstantFloat( UINT32 location, FLOAT32 value )
    {
        glUniform1f( location, value );
    }
    
    VOID GLESGraphicContext::SetPixelShaderConstantFloatArray( UINT32 location, const FLOAT32* value, UINT32 count )
    {
        glUniform1fv( location, count, value );
    }
    
    VOID GLESGraphicContext::SetPixelShaderConstantInt( UINT32 location, INT32 value )
    {
        glUniform1i( location, value );
    }
    
    VOID GLESGraphicContext::SetPixelShaderConstantIntArray( UINT32 location, const INT32* value, UINT32 count )
    {
        glUniform1iv( location, count, (const GLint*)value );
    }
    
    VOID GLESGraphicContext::SetPixelShaderConstantMatrix( UINT32 location, const Matrix16* value )
    {
        glUniformMatrix4fv( location, 1, GL_FALSE, (const GLfloat*)value );
    }
    
    VOID GLESGraphicContext::SetPixelShaderConstantMatrix( UINT32 location, const Matrix9* value )
    {
        glUniformMatrix3fv( location, 1, GL_FALSE, (const GLfloat*)value );
    }
    
    VOID GLESGraphicContext::SetPixelShaderConstantMatrixArray( UINT32 location, const Matrix16* value, UINT32 count )
    {
        glUniformMatrix4fv( location, count, GL_FALSE, (const GLfloat*)value );
    }
    
    VOID GLESGraphicContext::SetPixelShaderConstantVector( UINT32 location, const Vector4* value )
    {
        glUniform4f( location, value->x, value->y, value->z, value->w );
    }
    
    VOID GLESGraphicContext::SetPixelShaderConstantVectorArray( UINT32 location, const Vector4* value, UINT32 count )
    {
        glUniform4fv( location, count, (const GLfloat*)value );
    }
    
    VOID GLESGraphicContext::SetPixelShaderConstantVector( UINT32 location, const Vector3* value )
    {
        glUniform3f( location, value->x, value->y, value->z );
    }
    
    VOID GLESGraphicContext::SetPixelShaderConstantVector( UINT32 location, const Vector2* value )
    {
        glUniform2f( location, value->x, value->y );
    }

    VOID GLESGraphicContext::SetShaderTexture( SHADER_PROGRAM_HANDLE program, const CHAR* name, UINT32 stage, TEXTURE_HANDLE texture )
    {
        UINT32 location = glGetUniformLocation( program, name );
        glUniform1i( location, 0 );
        GLESTexture* pTexture = (GLESTexture*)texture;
        UINT32 texID = pTexture->GetTexID();
        glActiveTexture( GL_TEXTURE0 + stage );
        glBindTexture( GL_TEXTURE_2D, texID );
    }

    VOID GLESGraphicContext::Release()
    {
        delete this;
    }

    VOID GLESGraphicContext::EnsureThreadContext()
    {
        [EAGLContext setCurrentContext:(EAGLContext*)m_pContext];
    }
}
