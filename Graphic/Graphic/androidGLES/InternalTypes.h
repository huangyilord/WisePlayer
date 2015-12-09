#if !defined ( _INTERNAL_TYPES_H_ )
#define _INTERNAL_TYPES_H_

#include "GraphicTypeDef.h"
#include "Graphic/IGraphicContext.h"

namespace Graphic
{
    class GLESGraphicContext;

    class DECLSPEC_NOVTABLE IGLESSurface
    {
    public:
        IGLESSurface( UINT32 width, UINT32 height )
            : m_width( width ), m_height( height ) {}
        virtual VOID AdaptAsRenderBuffer() = 0;
        virtual VOID AdaptAsDepthStencil() = 0;
        virtual VOID Release() = 0;
    
        inline UINT32 GetWidth() const { return m_width; }
        inline UINT32 GetHeight() const { return m_height; }
    private:
        UINT32  m_width;
        UINT32  m_height;
    };

    class GLESRenderSurface
        : public IGLESSurface
    {
    public:
        GLESRenderSurface( UINT32 bufferID, UINT32 width, UINT32 height );
        ~GLESRenderSurface();

        virtual VOID AdaptAsRenderBuffer();
        virtual VOID AdaptAsDepthStencil();
        virtual VOID Release();

        inline UINT32 GetBufferID() const { return m_bufferID; }
    private:
        UINT32  m_bufferID;
    };

    class FakeGLESRenderSurface
        : public IGLESSurface
    {
    public:
        FakeGLESRenderSurface( UINT32 width, UINT32 height );
        ~FakeGLESRenderSurface();

        virtual VOID AdaptAsRenderBuffer();
        virtual VOID AdaptAsDepthStencil();
        virtual VOID Release();
    };

    class GLESTextureSurface
        : public IGLESSurface
    {
    public:
        GLESTextureSurface( UINT32 texID, UINT32 width, UINT32 height, UINT32 format );
        ~GLESTextureSurface();

        virtual VOID AdaptAsRenderBuffer();
        virtual VOID AdaptAsDepthStencil();
        virtual VOID Release();

        inline UINT32 GetTexID() const { return m_texID; }
        inline UINT32 GetFormat() const { return m_format; }
    private:
        UINT32  m_texID;
        UINT32  m_format;
    };

    class DECLSPEC_NOVTABLE GLESTexture
    {
    public:
        GLESTexture( UINT32 texID, UINT32 width, UINT32 height, UINT32 format );
        ~GLESTexture();

        inline GLESTextureSurface* GetSurface() { return &m_surface; }
        inline const GLESTextureSurface* GetSurface() const { return &m_surface; }
        inline UINT32 GetTexID() const { return m_surface.GetTexID(); }
        inline UINT32 GetPixelFormat() const { return m_surface.GetFormat(); }
    private:
        GLESTextureSurface  m_surface;
    };

    class DECLSPEC_NOVTABLE GLESFrameBuffer
    {
    public:
        GLESFrameBuffer( UINT32 ID );
        ~GLESFrameBuffer();

        inline IGLESSurface* GetDepthSurface() const { return m_pDepthSurface; }
        inline IGLESSurface* GetColorSurface() const { return m_pColorSurface; }
        inline UINT32 GetFrameBufferID() const { return m_id; }

        VOID SetDepthSurface( IGLESSurface* pSurface );
        VOID SetColorSurface( IGLESSurface* pSurface );
    private:
        UINT32              m_id;
        IGLESSurface*       m_pDepthSurface;
        IGLESSurface*       m_pColorSurface;
    };

    class DECLSPEC_NOVTABLE GLESVertexBufferFormat
    {
    public:
        typedef struct GLESVertexElementDesc
        {
            UINT16          index;
            UINT16          offset;
            UINT16          size;
            UINT16          type;
        } GLESVertexElementDesc;

        GLESVertexBufferFormat();
        ~GLESVertexBufferFormat();

        BOOL Create( const IGraphicContext::VertexElement* elements, UINT32 count );
        VOID Destroy();

        inline UINT32 GetElementCount() const { return m_elementCount; }
        inline const GLESVertexElementDesc& GetElement( UINT32 index ) const { return m_pElements[index]; }
        inline UINT32 GetStride() const { return m_stride; }
    private:
        GLESVertexElementDesc*      m_pElements;
        UINT32                      m_elementCount;
        UINT32                      m_stride;
    };

    class DECLSPEC_NOVTABLE GLESVertexBuffer
    {
    public:
        GLESVertexBuffer();
        ~GLESVertexBuffer();

        BOOL Create( UINT32 size );
        VOID Destroy();

        VOID* GetDataPointer();
        BOOL CommitBufferData();

        inline UINT32 GetID() const { return m_ID; }
        inline UINT32 GetSize() const { return m_size; }
    private:

        UINT32                      m_ID;
        UINT32                      m_size;
        BYTE*                       m_pointer;
    };
    
    class DECLSPEC_NOVTABLE GLESIndexBuffer
    {
    public:
        GLESIndexBuffer();
        ~GLESIndexBuffer();

        BOOL Create( UINT32 size );
        VOID Destroy();

        UINT16* GetDataPointer();
        BOOL CommitBufferData();

        inline UINT32 GetID() const { return m_ID; }
        inline UINT32 GetSize() const { return m_size; }
    private:

        UINT32                      m_ID;
        UINT32                      m_size;
        BYTE*                       m_pointer;
    };
}

#endif
