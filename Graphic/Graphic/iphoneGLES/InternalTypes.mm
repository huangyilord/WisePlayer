#include "InternalTypes.h"
#import <GLKit/GLKit.h>

#include "GLESGraphicContext.h"

namespace Graphic
{
    const CHAR* s_DefaultPixelShaderString =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
        "precision highp float;\n"
    "#else\n"
        "precision mediump float;\n"
    "#endif\n"
    "uniform sampler2D u_texture;"
    "varying mediump vec2 v_tex0;"
    "void main()"
    "{"
        "gl_FragColor = texture2D(u_texture, v_tex0);"
    "}";

    const CHAR* s_DefaultVertexShaderString =
    "uniform mat4 u_transform;"
    "uniform mat4 u_texTransform;"
    "attribute vec3 a_position;"
    "attribute vec2 a_tex0;"
    "varying mediump vec2 v_tex0;"
    "void main()"
    "{"
        "vec4 pos = vec4( a_position, 1 );"
        "gl_Position = u_transform * pos;"
        "vec4 tx = vec4( a_tex0, 0, 1 );"
        "v_tex0 = (u_texTransform * tx).xy;"
    "}";

    GLESRenderSurface::GLESRenderSurface( UINT32 bufferID, UINT32 width, UINT32 height )
        : IGLESSurface( width, height )
        , m_bufferID( bufferID )
    {
    }

    GLESRenderSurface::~GLESRenderSurface()
    {
    }
    
    VOID GLESRenderSurface::AdaptAsRenderBuffer()
    {
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_bufferID );
    }

    VOID GLESRenderSurface::AdaptAsDepthStencil()
    {
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_bufferID );
    }

    VOID GLESRenderSurface::Release()
    {
        if ( 0 != m_bufferID )
        {
            glDeleteRenderbuffers( 1, &m_bufferID );
            delete this;
        }
    }

    GLESTextureSurface::GLESTextureSurface( UINT32 texID, UINT32 width, UINT32 height, UINT32 format )
        : IGLESSurface( width, height )
        , m_texID( texID )
        , m_format( format )
    {
    }

    GLESTextureSurface::~GLESTextureSurface()
    {
    }

    VOID GLESTextureSurface::AdaptAsRenderBuffer()
    {
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texID, 0 );
    }

    VOID GLESTextureSurface::AdaptAsDepthStencil()
    {
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texID, 0 );
    }

    VOID GLESTextureSurface::Release()
    {
        // do nothing
        // only destroy with texture
    }

    GLESTexture::GLESTexture( UINT32 texID, UINT32 width, UINT32 height, UINT32 format )
        : m_surface( texID, width, height, format )
    {
    }

    GLESTexture::~GLESTexture()
    {
        UINT32 texID = m_surface.GetTexID();
        if ( 0 != texID )
        {
            glDeleteTextures( 1, &texID );
        }
    }

    GLESFrameBuffer::GLESFrameBuffer( UINT32 ID )
        : m_id( ID )
        , m_pDepthSurface( NULL )
        , m_pColorSurface( NULL )
    {
    }

    GLESFrameBuffer::~GLESFrameBuffer()
    {
        if ( 0 != m_id )
        {
            glDeleteFramebuffers( 1, &m_id );
        }
    }

    VOID GLESFrameBuffer::SetDepthSurface( IGLESSurface* pSurface )
    {
        m_pDepthSurface = pSurface;
        pSurface->AdaptAsDepthStencil();
    }

    VOID GLESFrameBuffer::SetColorSurface( IGLESSurface* pSurface )
    {
        m_pColorSurface = pSurface;
        pSurface->AdaptAsRenderBuffer();
    }

    GLESVertexBufferFormat::GLESVertexBufferFormat()
        : m_pElements( NULL )
        , m_elementCount( 0 )
        , m_stride( 0 )
    {
    }

    GLESVertexBufferFormat::~GLESVertexBufferFormat()
    {
        Destroy();
    }
    
    BOOL GLESVertexBufferFormat::Create( const IGraphicContext::VertexElement* elements, UINT32 count )
    {
        Destroy();
        m_pElements = new GLESVertexElementDesc[count];
        if ( NULL == m_pElements )
        {
            Destroy();
            return FALSE;
        }
        m_elementCount = count;
        for ( UINT32 i = 0; i < m_elementCount; ++i )
        {
            const IGraphicContext::VertexElement& vtxElement = elements[i];
            GLESVertexElementDesc& desc = m_pElements[i];
            desc.index = vtxElement.usage;
            desc.offset = vtxElement.offset;
            UINT32 stride = 0;
            switch ( vtxElement.dataType )
            {
            case IGraphicContext::DataType_Float:
                desc.type = GL_FLOAT;
                stride = 4;
                break;
            case IGraphicContext::DataType_Byte:
                desc.type = GL_UNSIGNED_BYTE;
                stride = 1;
                break;
            case IGraphicContext::DataType_Short:
                desc.type = GL_SHORT;
                stride = 2;
                break;
            default:
                Destroy();
                return FALSE;
            }
            desc.size = vtxElement.size;
            m_stride += stride * desc.size;
        }
        return TRUE;
    }

    VOID GLESVertexBufferFormat::Destroy()
    {
        if ( NULL != m_pElements )
        {
            delete [] m_pElements;
            m_pElements = NULL;
        }
        m_elementCount = 0;
        m_stride = 0;
    }

    GLESVertexBuffer::GLESVertexBuffer()
        : m_ID( 0 )
        , m_size( 0 )
        , m_pointer( NULL )
    {
    }

    GLESVertexBuffer::~GLESVertexBuffer()
    {
        Destroy();
    }

    BOOL GLESVertexBuffer::Create( UINT32 size )
    {
        Destroy();
        m_size = size;
        glGenBuffers( 1, &m_ID );
        return m_ID != 0;
    }

    VOID GLESVertexBuffer::Destroy()
    {
        if ( NULL != m_pointer )
        {
            delete [] m_pointer;
            m_pointer = NULL;
        }
        if ( 0 != m_ID )
        {
            glDeleteBuffers( 1, &m_ID );
            m_ID = 0;
        }
        m_size = 0;
    }

    VOID* GLESVertexBuffer::GetDataPointer()
    {
        if ( NULL == m_pointer )
        {
            m_pointer = new BYTE[m_size];
        }
        return m_pointer;
    }

    BOOL GLESVertexBuffer::CommitBufferData()
    {
        glBindBuffer( GL_ARRAY_BUFFER, m_ID );
        glBufferData( GL_ARRAY_BUFFER, m_size, m_pointer, GL_STATIC_DRAW );
        delete m_pointer;
        m_pointer = NULL;
        return glGetError() == GL_NO_ERROR;
    }

    GLESIndexBuffer::GLESIndexBuffer()
        : m_ID( 0 )
        , m_size( 0 )
        , m_pointer( NULL )
    {
    }

    GLESIndexBuffer::~GLESIndexBuffer()
    {
        Destroy();
    }

    BOOL GLESIndexBuffer::Create( UINT32 size )
    {
        Destroy();
        m_size = size;
        glGenBuffers( 1, &m_ID );
        return m_ID != 0;
    }

    VOID GLESIndexBuffer::Destroy()
    {
        if ( NULL != m_pointer )
        {
            delete [] m_pointer;
            m_pointer = NULL;
        }
        if ( 0 != m_ID )
        {
            glDeleteBuffers( 1, &m_ID );
            m_ID = 0;
        }
        m_size = 0;
    }

    UINT16* GLESIndexBuffer::GetDataPointer()
    {
        if ( NULL == m_pointer )
        {
            m_pointer = new BYTE[m_size];
        }
        return (UINT16*)m_pointer;
    }

    BOOL GLESIndexBuffer::CommitBufferData()
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ID );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_size, m_pointer, GL_STATIC_DRAW );
        delete m_pointer;
        m_pointer = NULL;
        return glGetError() == GL_NO_ERROR;
    }
}

