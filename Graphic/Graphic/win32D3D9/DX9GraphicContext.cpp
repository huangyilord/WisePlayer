#include "DX9GraphicContext.h"

#include <windows.h>
#include "InternalTypes.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

namespace Graphic
{
	DX9GraphicContext::Direct9 DX9GraphicContext::s_Direct;

	DX9GraphicContext::Direct9::Direct9()
	{
		m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	}
	DX9GraphicContext::Direct9::~Direct9()
	{
		if ( m_pD3D != NULL )
		{
			m_pD3D->Release();
		}
	}

    DX9GraphicContext::DX9GraphicContext()
        : m_pD3DDevice( NULL )
        , m_pRenderTarget( NULL )
        , m_currentShaderProgram( NULL )
    {
    }

    DX9GraphicContext::~DX9GraphicContext()
    {
        Destroy();
    }

    BOOL DX9GraphicContext::Initialize( HWND hWnd )
    {
        m_hWnd = hWnd;
		ZeroMemory( &m_D3DParameter, sizeof( D3DPRESENT_PARAMETERS ) );
        RECT clientRect;
        GetClientRect( hWnd, &clientRect );
        m_D3DParameter.BackBufferHeight = (UINT)((clientRect.bottom - clientRect.top));
        m_D3DParameter.BackBufferWidth = (UINT)((clientRect.right - clientRect.left));
		m_D3DParameter.Windowed = TRUE;
		m_D3DParameter.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_D3DParameter.BackBufferFormat = D3DFMT_UNKNOWN;
		m_D3DParameter.EnableAutoDepthStencil = TRUE;
		m_D3DParameter.AutoDepthStencilFormat = D3DFMT_D24X8;

        HRESULT result = s_Direct.GetDirectHandler()->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &m_D3DParameter, &m_pD3DDevice );
		if ( FAILED( result ) )
		{
			return FALSE;
		}

        RenderTarget* pTarget = new RenderTarget;
        if ( NULL == pTarget )
        {
            m_pD3DDevice->Release();
            m_pD3DDevice = NULL;
            return FALSE;
        }
        m_pD3DDevice->GetRenderTarget( 0, &pTarget->renderSurface );
        m_pD3DDevice->GetDepthStencilSurface( &pTarget->depthStencil );
        m_pRenderTarget = pTarget;

        m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
        m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		m_pD3DDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESS );
		m_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
        m_pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		m_pD3DDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

        m_viewport.uMinX = m_viewport.uMinY = 0;
        m_viewport.uWidth = m_D3DParameter.BackBufferWidth;
        m_viewport.uHeight = m_D3DParameter.BackBufferHeight;

        m_currentShaderProgram = NULL;
		return TRUE;
    }

    VOID DX9GraphicContext::Destroy()
    {
        if ( NULL != m_pRenderTarget )
        {
            RenderTarget* pTarget = (RenderTarget*)m_pRenderTarget;
            delete pTarget;
        }
        if ( m_pD3DDevice != NULL )
        {
            m_pD3DDevice->Release();
            m_pD3DDevice = NULL;
        }
    }

    VERTEX_BUFFER_FORMAT_HANDLE DX9GraphicContext::CreateVertexBufferFormat( const VertexElement* elements, UINT32 count )
    {
        VertexBufferFormat* pFormat = new VertexBufferFormat;
        if ( NULL == pFormat )
        {
            return NULL;
        }
        D3DVERTEXELEMENT9* pD3DVtxElements = new D3DVERTEXELEMENT9[count + 1];
        if ( NULL == pD3DVtxElements )
        {
            delete pFormat;
            return NULL;
        }
        memset( pD3DVtxElements, 0, sizeof(D3DVERTEXELEMENT9) * (count + 1) );

        static const D3DDECLUSAGE s_usage[] =
        {
            D3DDECLUSAGE_POSITION
            , D3DDECLUSAGE_BLENDWEIGHT
            , D3DDECLUSAGE_BLENDINDICES
            , D3DDECLUSAGE_NORMAL
            , D3DDECLUSAGE_COLOR
            , D3DDECLUSAGE_COLOR
            , D3DDECLUSAGE_TEXCOORD
            , D3DDECLUSAGE_TEXCOORD
        };

        pFormat->stride = 0;
        for ( UINT32 i = 0; i < count; ++i )
        {
            D3DVERTEXELEMENT9& d3dElement = pD3DVtxElements[i];
            const VertexElement& element = elements[i];
            d3dElement.Stream = 0;
            d3dElement.Offset = element.offset;
            switch ( element.dataType )
            {
                case DataType_Float:
                    d3dElement.Type = D3DDECLTYPE_FLOAT1 + element.size - 1;
                    pFormat->stride += sizeof(FLOAT32) * element.size;
                    break;
                case DataType_Byte:
                    d3dElement.Type = D3DDECLTYPE_UBYTE4;
                    pFormat->stride += sizeof(BYTE) * element.size;
                    break;
                case DataType_Short:
                    d3dElement.Type = element.size > 2 ? D3DDECLTYPE_SHORT4 : D3DDECLTYPE_SHORT2;
                    pFormat->stride += sizeof(SHORT) * element.size;
                    break;
            }
            d3dElement.Method = D3DDECLMETHOD_DEFAULT;

            d3dElement.Usage = s_usage[element.usage];
            if ( element.usage == DataUsage_Color1 || element.usage == DataUsage_TexCoord1 )
            {
                d3dElement.UsageIndex = 1;
            }
        }

        D3DVERTEXELEMENT9& endElement = pD3DVtxElements[count];
        D3DVERTEXELEMENT9 declEnd = D3DDECL_END();
        endElement = declEnd;

        if ( FAILED(m_pD3DDevice->CreateVertexDeclaration( pD3DVtxElements, &pFormat->format )) )
        {
            delete [] pD3DVtxElements;
            delete pFormat;
            return NULL;
        }
        delete [] pD3DVtxElements;
        return pFormat;
    }

    VOID DX9GraphicContext::DestroyVertexBufferFormat( VERTEX_BUFFER_FORMAT_HANDLE format )
    {
        VertexBufferFormat* pFormat = (VertexBufferFormat*)format;
        pFormat->format->Release();
        delete pFormat;
    }

    VERTEX_BUFFER_HANDLE DX9GraphicContext::CreateVertexBuffer( UINT32 size )
    {
        LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
        if ( FAILED(m_pD3DDevice->CreateVertexBuffer( size, 0, 0, D3DPOOL_DEFAULT, &pVertexBuffer, NULL )) )
        {
            return NULL;
        }
        return pVertexBuffer;
    }

    VOID* DX9GraphicContext::GetVertexBufferPointer( VERTEX_BUFFER_HANDLE vtxHandle )
    {
        VOID* pBuffer = NULL;
        LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = (LPDIRECT3DVERTEXBUFFER9)vtxHandle;
        if ( FAILED( pVertexBuffer->Lock( 0, 0, &pBuffer, 0 ) ) || NULL == pBuffer )
        {
            return NULL;
        }
        return pBuffer;
    }

    BOOL DX9GraphicContext::CommitVertexBuffer( VOID*, VERTEX_BUFFER_HANDLE vtxHandle )
    {
        LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = (LPDIRECT3DVERTEXBUFFER9)vtxHandle;
        return SUCCEEDED(pVertexBuffer->Unlock());
    }

    VOID DX9GraphicContext::DestroyVertexBuffer( VERTEX_BUFFER_HANDLE vtxHandle )
    {
        LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = (LPDIRECT3DVERTEXBUFFER9)vtxHandle;
        pVertexBuffer->Release();
    }

    INDEX_BUFFER_HANDLE DX9GraphicContext::CreateIndexBuffer( UINT32 size )
    {
        LPDIRECT3DINDEXBUFFER9 pIndexBuffer;
        if ( FAILED(m_pD3DDevice->CreateIndexBuffer( size, D3DUSAGE_POINTS, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pIndexBuffer, NULL )) )
        {
            return NULL;
        }
        return pIndexBuffer;
    }

    UINT16* DX9GraphicContext::GetIndexBufferPointer( INDEX_BUFFER_HANDLE idxHandle )
    {
        UINT16* pBuffer = NULL;
        LPDIRECT3DINDEXBUFFER9 pIndexBuffer = (LPDIRECT3DINDEXBUFFER9)idxHandle;
        if ( FAILED(pIndexBuffer->Lock( 0, 0, (PVOID*)&pBuffer, 0 )) )
        {
            return NULL;
        }
        return pBuffer;
    }

    BOOL DX9GraphicContext::CommitIndexBuffer( UINT16*, INDEX_BUFFER_HANDLE idxHandle )
    {
        LPDIRECT3DINDEXBUFFER9 pIndexBuffer = (LPDIRECT3DINDEXBUFFER9)idxHandle;
        return SUCCEEDED(pIndexBuffer->Unlock());
    }

    VOID DX9GraphicContext::DestroyIndexBuffer( INDEX_BUFFER_HANDLE idxHandle )
    {
        LPDIRECT3DINDEXBUFFER9 pIndexBuffer = (LPDIRECT3DINDEXBUFFER9)idxHandle;
        pIndexBuffer->Release();
    }

    SURFACE_DATA_HANDLE DX9GraphicContext::GetRenderTargetData( UINT32 x, UINT32 y, UINT32 width, UINT32 height )
    {
        SurfaceDataReader* pReader = new SurfaceDataReader;
        if ( NULL == pReader )
        {
            return NULL;
        }
        if ( FAILED( m_pD3DDevice->CreateOffscreenPlainSurface( width, height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pReader->surface, 0 ) ) )
        {
            delete pReader;
            return NULL;
        }
        RenderTarget* pTarget = (RenderTarget*)m_pRenderTarget;
        if ( FAILED(m_pD3DDevice->GetRenderTargetData( (LPDIRECT3DSURFACE9)pTarget->renderSurface, pReader->surface )) )
        {
            pReader->surface->Release();
            delete pReader;
            return NULL;
        }
        RECT rect = { x, y, x + width, y + height };
        if ( FAILED(pReader->surface->LockRect( &pReader->lockedRect, &rect, D3DLOCK_READONLY )) )
        {
            pReader->surface->Release();
            delete pReader;
            return NULL;
        }
        return pReader;
    }

    VOID* DX9GraphicContext::GetSurfaceDataPointer( SURFACE_DATA_HANDLE dataHandle )
    {
        SurfaceDataReader* pReader = (SurfaceDataReader*)dataHandle;
        return pReader->lockedRect.pBits;
    }
    
    VOID DX9GraphicContext::ReleaseSurfaceData( SURFACE_DATA_HANDLE dataHandle )
    {
        SurfaceDataReader* pReader = (SurfaceDataReader*)dataHandle;
        pReader->surface->UnlockRect();
        pReader->surface->Release();
        delete pReader;
    }

    VOID DX9GraphicContext::GetSurfaceSize( SURFACE_HANDLE surface, UINT32 &width, UINT32 &height )
    {
        LPDIRECT3DSURFACE9 pSurface = (LPDIRECT3DSURFACE9)surface;
        D3DSURFACE_DESC desc;
        pSurface->GetDesc( &desc );
        width = desc.Width;
        height = desc.Height;
    }

    TEXTURE_HANDLE DX9GraphicContext::CreateTexture( UINT32 width, UINT32 height )
    {
        LPDIRECT3DTEXTURE9 pTexture = NULL;
        if ( FAILED(m_pD3DDevice->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexture, NULL )) )
        {
            return NULL;
        }
        return pTexture;
    }

    struct ColorMask16
    {
        UINT16 rSrc;
        UINT16 gSrc;
        UINT16 bSrc;
        UINT16 aSrc;
        UINT16 rDst;
        UINT16 gDst;
        UINT16 bDst;
        UINT16 aDst;
    };

    TEXTURE_HANDLE DX9GraphicContext::CreateTexture( const VOID* data, UINT32 width, UINT32 height, PixelType pixelType )
    {
        LPDIRECT3DTEXTURE9 pTexture = NULL;
        static const D3DFORMAT s_textureFormats[] = { D3DFMT_A8R8G8B8, D3DFMT_A4R4G4B4, D3DFMT_R5G6B5, D3DFMT_A1R5G5B5 };
        if ( FAILED(m_pD3DDevice->CreateTexture( width, height, 1, 0, s_textureFormats[pixelType], D3DPOOL_MANAGED, &pTexture, NULL )) )
        {
            return NULL;
        }
        if ( NULL == data )
        {
            return pTexture;
        }
        D3DLOCKED_RECT lockedRect;
        if ( FAILED(pTexture->LockRect( 0, &lockedRect, NULL, 0 )) )
        {
            pTexture->Release();
            return NULL;
        }
        switch ( pixelType )
        {
            case PixelType_32_8888:
            {
                struct SrcColor32_RGBA
                {
                    UINT32 r : 8;
                    UINT32 g : 8;
                    UINT32 b : 8;
                    UINT32 a : 8;
                };
                const SrcColor32_RGBA* src = (const SrcColor32_RGBA*)data;

                struct DstColor32_BGRA
                {
                    UINT32 b : 8;
                    UINT32 g : 8;
                    UINT32 r : 8;
                    UINT32 a : 8;
                } *dst = NULL;
                for ( UINT32 y = 0; y < height; ++y )
                {
                    dst = (DstColor32_BGRA*)((BYTE*)lockedRect.pBits + lockedRect.Pitch * y);
                    for ( UINT32 x = 0; x < width; ++x )
                    {
                        const SrcColor32_RGBA& srcPixel = src[y * width + x];
                        DstColor32_BGRA& dstPixel = dst[x];
                        dstPixel.r = srcPixel.r;
                        dstPixel.g = srcPixel.g;
                        dstPixel.b = srcPixel.b;
                        dstPixel.a = srcPixel.a;
                    }
                }
                break;
            }
            case PixelType_16_4444:
            {
                struct SrcColor16_RGBA
                {
                    UINT16 r : 4;
                    UINT16 g : 4;
                    UINT16 b : 4;
                    UINT16 a : 4;
                };
                const SrcColor16_RGBA *src = (const SrcColor16_RGBA*)data;

                struct DstColor16_BGRA
                {
                    UINT16 b : 4;
                    UINT16 g : 4;
                    UINT16 r : 4;
                    UINT16 a : 4;
                } *dst = NULL;
                for ( UINT32 y = 0; y < height; ++y )
                {
                    dst = (DstColor16_BGRA*)((BYTE*)lockedRect.pBits + lockedRect.Pitch * y);
                    for ( UINT32 x = 0; x < width; ++x )
                    {
                        const SrcColor16_RGBA& srcPixel = src[y * width + x];
                        DstColor16_BGRA& dstPixel = dst[x];
                        dstPixel.r = srcPixel.r;
                        dstPixel.g = srcPixel.g;
                        dstPixel.b = srcPixel.b;
                        dstPixel.a = srcPixel.a;
                    }
                }
                break;
            }
            case PixelType_16_565:
            {
                struct SrcColor16_RGB
                {
                    UINT16 r : 5;
                    UINT16 g : 6;
                    UINT16 b : 5;
                };
                const SrcColor16_RGB *src = (const SrcColor16_RGB*)data;

                struct DstColor16_BGR
                {
                    UINT16 b : 5;
                    UINT16 g : 6;
                    UINT16 r : 5;
                } *dst = NULL;
                for ( UINT32 y = 0; y < height; ++y )
                {
                    dst = (DstColor16_BGR*)((BYTE*)lockedRect.pBits + lockedRect.Pitch * y);
                    for ( UINT32 x = 0; x < width; ++x )
                    {
                        const SrcColor16_RGB& srcPixel = src[y * width + x];
                        DstColor16_BGR& dstPixel = dst[x];
                        dstPixel.r = srcPixel.r;
                        dstPixel.g = srcPixel.g;
                        dstPixel.b = srcPixel.b;
                    }
                }
                break;
            }
            
            case PixelType_16_5551:
            {
                struct SrcColor16_RGBA
                {
                    UINT16 r : 5;
                    UINT16 g : 5;
                    UINT16 b : 5;
                    UINT16 a : 1;
                };
                const SrcColor16_RGBA *src = (const SrcColor16_RGBA*)data;

                struct DstColor16_BGRA
                {
                    UINT16 b : 5;
                    UINT16 g : 5;
                    UINT16 r : 5;
                    UINT16 a : 1;
                } *dst = NULL;
                for ( UINT32 y = 0; y < height; ++y )
                {
                    dst = (DstColor16_BGRA*)((BYTE*)lockedRect.pBits + lockedRect.Pitch * y);
                    for ( UINT32 x = 0; x < width; ++x )
                    {
                        const SrcColor16_RGBA& srcPixel = src[y * width + x];
                        DstColor16_BGRA& dstPixel = dst[x];
                        dstPixel.r = srcPixel.r;
                        dstPixel.g = srcPixel.g;
                        dstPixel.b = srcPixel.b;
                        dstPixel.a = srcPixel.a;
                    }
                }
                break;
            }
            default:
                pTexture->UnlockRect( 0 );
                pTexture->Release();
                return NULL;
        }
        pTexture->UnlockRect( 0 );
        return pTexture;
    }

    VOID DX9GraphicContext::TextureSubImage( TEXTURE_HANDLE texture, const VOID* data, UINT32 width, UINT32 height, UINT32 x, UINT32 y )
    {
        LPDIRECT3DTEXTURE9 pTexture = (LPDIRECT3DTEXTURE9)texture;
        D3DLOCKED_RECT lockedRect;
        RECT rect = { x, y, x + width, y + height };
        if ( FAILED(pTexture->LockRect( 0, &lockedRect, &rect, 0 )) )
        {
            return;
        }
        struct SrcColor32_RGBA
        {
            UINT32 r : 8;
            UINT32 g : 8;
            UINT32 b : 8;
            UINT32 a : 8;
        };
        const SrcColor32_RGBA* src = (const SrcColor32_RGBA*)data;

        struct DstColor32_BGRA
        {
            UINT32 b : 8;
            UINT32 g : 8;
            UINT32 r : 8;
            UINT32 a : 8;
        } *dst = NULL;
        for ( UINT32 y = 0; y < height; ++y )
        {
            dst = (DstColor32_BGRA*)((BYTE*)lockedRect.pBits + lockedRect.Pitch * y);
            for ( UINT32 x = 0; x < width; ++x )
            {
                const SrcColor32_RGBA& srcPixel = src[y * width + x];
                DstColor32_BGRA& dstPixel = dst[x];
                dstPixel.r = srcPixel.r;
                dstPixel.g = srcPixel.g;
                dstPixel.b = srcPixel.b;
                dstPixel.a = srcPixel.a;
            }
        }
        pTexture->UnlockRect( 0 );
    }

    VOID DX9GraphicContext::DestroyTexture( TEXTURE_HANDLE texture )
    {
        LPDIRECT3DTEXTURE9 pTexture = (LPDIRECT3DTEXTURE9)texture;
        pTexture->Release();
    }

    VOID DX9GraphicContext::EnableTexture( UINT32 stage )
    {
    }

    VOID DX9GraphicContext::DisableTexture( UINT32 stage )
    {
        m_pD3DDevice->SetTexture( stage, NULL );
    }

    VOID DX9GraphicContext::SetTexture( UINT32 stage, TEXTURE_HANDLE texture )
    {
        m_pD3DDevice->SetTexture( stage, (LPDIRECT3DTEXTURE9)texture );
    }

    SURFACE_HANDLE DX9GraphicContext::GetSurfaceFromTexture( TEXTURE_HANDLE texture )
    {
        LPDIRECT3DSURFACE9 pSurface = NULL;
        LPDIRECT3DTEXTURE9 pTexture = (LPDIRECT3DTEXTURE9)texture;
        if ( FAILED( pTexture->GetSurfaceLevel( 0, &pSurface ) ) )
        {
            return NULL;
        }
        return pSurface;
    }

    RENDER_TARGET_HANDLE DX9GraphicContext::CreateRenderTarget()
    {
        RenderTarget* pTarget = new RenderTarget;
        memset( pTarget, 0, sizeof(RenderTarget) );
        return pTarget;
    }

    VOID DX9GraphicContext::DestroyRenderTarget( RENDER_TARGET_HANDLE target )
    {
        RenderTarget* pTarget = (RenderTarget*)target;
        delete pTarget;
    }

    VOID DX9GraphicContext::SetRenderTarget( RENDER_TARGET_HANDLE target )
    {
        if ( m_pRenderTarget != target )
        {
            m_pRenderTarget = target;
            RenderTarget* pTarget = (RenderTarget*)target;
            m_pD3DDevice->SetRenderTarget( 0, pTarget->renderSurface );
            m_pD3DDevice->SetDepthStencilSurface( pTarget->depthStencil );
        }
    }

    RENDER_TARGET_HANDLE DX9GraphicContext::GetRenderTarget()
    {
        return m_pRenderTarget;
    }

    SURFACE_HANDLE DX9GraphicContext::CreateDepthStencilSurface( UINT32 width, UINT32 height )
    {
        LPDIRECT3DSURFACE9 pSurface = NULL;
        if ( FAILED(m_pD3DDevice->CreateDepthStencilSurface( width, height, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE, &pSurface, NULL )) )
        {
            return NULL;
        }
        return pSurface;
    }

    VOID DX9GraphicContext::DestroyDepthStencilSurface( SURFACE_HANDLE surface )
    {
        LPDIRECT3DSURFACE9 pSurface = (LPDIRECT3DSURFACE9)surface;
        pSurface->Release();
    }

    VOID DX9GraphicContext::SetDepthStencilSurface( RENDER_TARGET_HANDLE target, SURFACE_HANDLE surface )
    {
        RenderTarget* pTarget = (RenderTarget*)target;
        pTarget->depthStencil = (LPDIRECT3DSURFACE9)surface;
    }

    SURFACE_HANDLE DX9GraphicContext::GetDepthStencilSurface( RENDER_TARGET_HANDLE target )
    {
        RenderTarget* pTarget = (RenderTarget*)target;
        return pTarget->depthStencil;
    }

    VOID DX9GraphicContext::SetColorSurface( RENDER_TARGET_HANDLE target, SURFACE_HANDLE surface )
    {
        RenderTarget* pTarget = (RenderTarget*)target;
        pTarget->renderSurface = (LPDIRECT3DSURFACE9)surface;
    }

    SURFACE_HANDLE DX9GraphicContext::GetColorSurface( RENDER_TARGET_HANDLE target )
    {
        RenderTarget* pTarget = (RenderTarget*)target;
        return pTarget->renderSurface;
    }

    VOID DX9GraphicContext::EnableDepthTest( BOOL bEnable )
    {
        m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, bEnable );
    }

    static const D3DCMPFUNC s_cmpFunc[] = {
            D3DCMP_NEVER
            , D3DCMP_LESS
            , D3DCMP_EQUAL
            , D3DCMP_LESSEQUAL
            , D3DCMP_GREATER
            , D3DCMP_NOTEQUAL
            , D3DCMP_GREATEREQUAL
            , D3DCMP_ALWAYS
        };

    VOID DX9GraphicContext::SetDepthTestFunc( DepthTestFunction func )
    {
        m_pD3DDevice->SetRenderState( D3DRS_ZFUNC, s_cmpFunc[func] );
    }

    VOID DX9GraphicContext::DepthBufferWriteEnable( BOOL bEnable )
    {
        m_pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, bEnable );
    }

    VOID DX9GraphicContext::ColorWriteEnable( BOOL bRedEnable, BOOL bGreenEnable, BOOL bBlueEnable, BOOL bAlphaEnable )
    {
        UINT32 flag = 0;
        if ( bRedEnable )
        {
            flag |= D3DCOLORWRITEENABLE_RED;
        }
        if ( bGreenEnable )
        {
            flag |= D3DCOLORWRITEENABLE_GREEN;
        }
        if ( bBlueEnable )
        {
            flag |= D3DCOLORWRITEENABLE_BLUE;
        }
        if ( bAlphaEnable )
        {
            flag |= D3DCOLORWRITEENABLE_ALPHA;
        }
        m_pD3DDevice->SetRenderState( D3DRS_COLORWRITEENABLE, flag );
    }

    VOID DX9GraphicContext::EnableBlend( BOOL bEnable )
    {
        m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, bEnable );
    }

    VOID DX9GraphicContext::SetBlendFunc( BlendFunction src, BlendFunction dst )
    {
        static const D3DBLEND s_blendFunc[] = {
            D3DBLEND_ZERO
            , D3DBLEND_ONE
            , D3DBLEND_SRCCOLOR
            , D3DBLEND_INVSRCCOLOR
            , D3DBLEND_SRCALPHA
            , D3DBLEND_INVSRCALPHA
            , D3DBLEND_DESTCOLOR
            , D3DBLEND_INVDESTCOLOR
            , D3DBLEND_DESTALPHA
            , D3DBLEND_INVDESTALPHA
        };
        m_pD3DDevice->SetRenderState( D3DRS_SRCBLEND, s_blendFunc[src] );
        m_pD3DDevice->SetRenderState( D3DRS_DESTBLEND, s_blendFunc[dst] );
    }

    VOID DX9GraphicContext::SetCullMode( CullMode mode )
    {
        static const D3DCULL s_cullMode[] = {
            D3DCULL_CW
            , D3DCULL_CCW
        };
        m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, s_cullMode[mode] );
    }

    VOID DX9GraphicContext::EnableCullFace( BOOL bEnable )
    {
        if ( !bEnable )
        {
            m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
        }
    }

    VOID DX9GraphicContext::EnableStencilTest( BOOL bEnable )
    {
        m_pD3DDevice->SetRenderState( D3DRS_STENCILENABLE, bEnable );
    }

    VOID DX9GraphicContext::SetStencilOperation( StencilOperation fail, StencilOperation zFail, StencilOperation pass )
    {
        static const D3DSTENCILOP s_StencilOp[] = {
            D3DSTENCILOP_KEEP
            , D3DSTENCILOP_ZERO
            , D3DSTENCILOP_REPLACE
            , D3DSTENCILOP_INCRSAT
            , D3DSTENCILOP_DECRSAT
            , D3DSTENCILOP_INVERT
        };

        m_pD3DDevice->SetRenderState( D3DRS_STENCILFAIL, s_StencilOp[fail] );
        m_pD3DDevice->SetRenderState( D3DRS_STENCILZFAIL, s_StencilOp[zFail] );
        m_pD3DDevice->SetRenderState( D3DRS_STENCILPASS, s_StencilOp[pass] );
    }

    VOID DX9GraphicContext::SetStencilTestFunc( StencilTestFunction func, UINT32 ref, UINT32 mask )
    {
        m_pD3DDevice->SetRenderState( D3DRS_STENCILFUNC, s_cmpFunc[func] );
        m_pD3DDevice->SetRenderState( D3DRS_STENCILREF, ref );
        m_pD3DDevice->SetRenderState( D3DRS_STENCILMASK, mask );
    }

    VOID DX9GraphicContext::SetStencilWriteMask( UINT32 mask )
    {
        m_pD3DDevice->SetRenderState( D3DRS_STENCILWRITEMASK, mask );
    }

    VOID DX9GraphicContext::SetViewPort( const ViewPort& viewport )
    {
        m_viewport = viewport;
    }

    VOID DX9GraphicContext::GetViewPort( ViewPort& viewport )
    {
        viewport = m_viewport;
    }


    VOID DX9GraphicContext::SetVertexBuffer( VERTEX_BUFFER_HANDLE vtxHandle, VERTEX_BUFFER_FORMAT_HANDLE format )
    {
        VertexBufferFormat* pFormat = (VertexBufferFormat*)format;
        m_pD3DDevice->SetStreamSource( 0, (LPDIRECT3DVERTEXBUFFER9)vtxHandle, 0, pFormat->stride );
        m_pD3DDevice->SetVertexDeclaration( pFormat->format );
    }

    VOID DX9GraphicContext::SetIndexBuffer( INDEX_BUFFER_HANDLE idxHandle )
    {
        m_pD3DDevice->SetIndices( (LPDIRECT3DINDEXBUFFER9)idxHandle );
    }

    static const D3DPRIMITIVETYPE s_primtiveType[] = {
        D3DPT_TRIANGLELIST
        , D3DPT_TRIANGLESTRIP
        , D3DPT_TRIANGLEFAN
    };

    VOID DX9GraphicContext::DrawPrimitive( PrimitiveType type, UINT32 startVtx, UINT32, UINT32 primitiveCount )
    {
        m_pD3DDevice->DrawPrimitive( s_primtiveType[type], startVtx, primitiveCount );
    }

    VOID DX9GraphicContext::DrawIndexedPrimitive( PrimitiveType type, UINT32 startVtx, UINT32 vtxCount, UINT32 startIdx, UINT32, UINT32 primitiveCount )
    {
        m_pD3DDevice->DrawIndexedPrimitive( s_primtiveType[type], startVtx, 0, vtxCount, startIdx, primitiveCount );
    }

    BOOL DX9GraphicContext::BeginScene()
    {
        return SUCCEEDED( m_pD3DDevice->BeginScene() );
    }

    VOID DX9GraphicContext::EndScene()
    {
        m_pD3DDevice->EndScene();
    }

    VOID DX9GraphicContext::Clear( UINT32 flags, ColorF_RGBA& color, FLOAT depth, DWORD stencil )
    {
        m_pD3DDevice->Clear( 0, NULL, flags, D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a), depth, stencil );
    }

    VOID DX9GraphicContext::Present()
    {
        RECT rect;
        rect.left = m_viewport.uMinX;
        rect.top = m_viewport.uMinX;
        rect.right = m_viewport.uMinX + m_viewport.uWidth;
        rect.bottom = m_viewport.uMinY + m_viewport.uHeight;
        m_pD3DDevice->Present( NULL, &rect, NULL, NULL );
    }

    VERTEX_SHADER_HANDLE DX9GraphicContext::CreateVertexShader( const CHAR* input, UINT32 len )
    {
        VertexShader* pVtxShader = new VertexShader;
        if ( NULL == pVtxShader )
        {
            return NULL;
        }
        LPD3DXBUFFER pShaderBuf = NULL;
        HRESULT result = D3DXCompileShader( input, len, NULL, NULL, "main", "vs_2_0", 0, &pShaderBuf, NULL, &pVtxShader->vertexShaderConstant );
		if ( FAILED( result ) )
		{
            delete pVtxShader;
			return NULL;
		}
        result = m_pD3DDevice->CreateVertexShader( (DWORD*)pShaderBuf->GetBufferPointer(), &pVtxShader->vertexShader );
        pShaderBuf->Release();
		if ( FAILED( result ) )
		{
            pVtxShader->vertexShaderConstant->Release();
            delete pVtxShader;
		}
        return pVtxShader;
    }

    VERTEX_SHADER_HANDLE DX9GraphicContext::CreateVertexShader( const VOID* pBuffer, UINT32 size )
    {
        VertexShader* pVtxShader = new VertexShader;
        if ( NULL == pVtxShader )
        {
            return NULL;
        }
        HRESULT result = D3DXGetShaderConstantTable( (DWORD*)pBuffer, &pVtxShader->vertexShaderConstant );
		if ( FAILED( result ) )
		{
            delete pVtxShader;
			return NULL;
		}
        result = m_pD3DDevice->CreateVertexShader( (DWORD*)pBuffer, &pVtxShader->vertexShader );
		if ( FAILED( result ) )
		{
            pVtxShader->vertexShaderConstant->Release();
            delete pVtxShader;
            return NULL;
		}
        return pVtxShader;
    }

    VOID DX9GraphicContext::DestroyVertexShader( VERTEX_SHADER_HANDLE shader )
    {
        VertexShader* pVtxShader = (VertexShader*)shader;
        pVtxShader->vertexShader->Release();
        pVtxShader->vertexShaderConstant->Release();
        delete pVtxShader;
    }

    PIXEL_SHADER_HANDLE DX9GraphicContext::CreatePixelShader( const CHAR* input, UINT32 len )
    {
        PixelShader* pPxlShader = new PixelShader;
        if ( NULL == pPxlShader )
        {
            return NULL;
        }
        LPD3DXBUFFER pShaderBuf = NULL;
        HRESULT result = D3DXCompileShader( input, len, NULL, NULL, "main", "ps_2_0", 0, &pShaderBuf, NULL, &pPxlShader->pixelShaderConstant );
		if ( FAILED( result ) )
		{
            delete pPxlShader;
			return NULL;
		}
        result = m_pD3DDevice->CreatePixelShader( (DWORD*)pShaderBuf->GetBufferPointer(), &pPxlShader->pixelShader );
        pShaderBuf->Release();
		if ( FAILED( result ) )
		{
            pPxlShader->pixelShaderConstant->Release();
            delete pPxlShader;
			return NULL;
		}
        return pPxlShader;
    }

    PIXEL_SHADER_HANDLE DX9GraphicContext::CreatePixelShader( const VOID* pBuffer, UINT32 size )
    {
        PixelShader* pPxlShader = new PixelShader;
        if ( NULL == pPxlShader )
        {
            return NULL;
        }
        HRESULT result = D3DXGetShaderConstantTable( (DWORD*)pBuffer, &pPxlShader->pixelShaderConstant );
		if ( FAILED( result ) )
		{
			delete pPxlShader;
			return NULL;
		}
        result = m_pD3DDevice->CreatePixelShader( (DWORD*)pBuffer, &pPxlShader->pixelShader );
		if ( FAILED( result ) )
		{
            pPxlShader->pixelShaderConstant->Release();
            delete pPxlShader;
			return NULL;
		}
        return pPxlShader;
    }

    VOID DX9GraphicContext::DestroyPixelShader( PIXEL_SHADER_HANDLE shader )
    {
        PixelShader* pPxlShader = (PixelShader*)shader;
        pPxlShader->pixelShader->Release();
        pPxlShader->pixelShaderConstant->Release();
        delete pPxlShader;
    }

    SHADER_PROGRAM_HANDLE DX9GraphicContext::CreateShaderProgram( VERTEX_SHADER_HANDLE vtxShader, PIXEL_SHADER_HANDLE pxlShader )
    {
        ShaderProgram* pShader = new ShaderProgram;
        if ( NULL == pShader )
        {
            return NULL;
        }
        pShader->vertexShader = (VertexShader*)vtxShader;
        pShader->pixelShader = (PixelShader*)pxlShader;
        return pShader;
    }

    VOID DX9GraphicContext::SetShaderProgram( SHADER_PROGRAM_HANDLE program )
    {
        if ( m_currentShaderProgram != program )
        {
            m_currentShaderProgram = program;
            ShaderProgram* pShader = (ShaderProgram*)program;
            m_pD3DDevice->SetVertexShader( pShader->vertexShader->vertexShader );
            m_pD3DDevice->SetPixelShader( pShader->pixelShader->pixelShader );
        }
    }

    VOID DX9GraphicContext::DestroyShaderProgram( SHADER_PROGRAM_HANDLE program )
    {
        ShaderProgram* pShader = (ShaderProgram*)program;
        delete pShader;
    }

    UINT32 DX9GraphicContext::GetVertexShaderConstantLocation( SHADER_PROGRAM_HANDLE program, const CHAR* name )
    {
        ShaderProgram* pShader = (ShaderProgram*)program;
        return (UINT32)pShader->vertexShader->vertexShaderConstant->GetConstantByName( NULL, name );
    }

    VOID DX9GraphicContext::SetVertexShaderConstantFloat( UINT32 location, FLOAT32 value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->vertexShader->vertexShaderConstant->SetFloat( m_pD3DDevice, (D3DXHANDLE)location, value );
    }

    VOID DX9GraphicContext::SetVertexShaderConstantFloatArray( UINT32 location, const FLOAT32* value, UINT32 count )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->vertexShader->vertexShaderConstant->SetFloatArray( m_pD3DDevice, (D3DXHANDLE)location, value, count );
    }

    VOID DX9GraphicContext::SetVertexShaderConstantInt( UINT32 location, INT32 value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->vertexShader->vertexShaderConstant->SetInt( m_pD3DDevice, (D3DXHANDLE)location, value );
    }

    VOID DX9GraphicContext::SetVertexShaderConstantIntArray( UINT32 location, const INT32* value, UINT32 count )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->vertexShader->vertexShaderConstant->SetIntArray( m_pD3DDevice, (D3DXHANDLE)location, value, count );
    }

    VOID DX9GraphicContext::SetVertexShaderConstantMatrix( UINT32 location, const Matrix16* value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->vertexShader->vertexShaderConstant->SetMatrix( m_pD3DDevice, (D3DXHANDLE)location, value );
    }

    VOID DX9GraphicContext::SetVertexShaderConstantMatrix( UINT32 location, const Matrix9* value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->vertexShader->vertexShaderConstant->SetValue( m_pD3DDevice, (D3DXHANDLE)location, value, sizeof(Matrix9) );
    }

    VOID DX9GraphicContext::SetVertexShaderConstantMatrixArray( UINT32 location, const Matrix16* value, UINT32 count )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->vertexShader->vertexShaderConstant->SetMatrixArray( m_pD3DDevice, (D3DXHANDLE)location, value, count );
    }

    VOID DX9GraphicContext::SetVertexShaderConstantVector( UINT32 location, const Vector4* value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->vertexShader->vertexShaderConstant->SetVector( m_pD3DDevice, (D3DXHANDLE)location, value );
    }

    VOID DX9GraphicContext::SetVertexShaderConstantVectorArray( UINT32 location, const Vector4* value, UINT32 count )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->vertexShader->vertexShaderConstant->SetVectorArray( m_pD3DDevice, (D3DXHANDLE)location, value, count );
    }

    VOID DX9GraphicContext::SetVertexShaderConstantVector( UINT32 location, const Vector3* value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->vertexShader->vertexShaderConstant->SetValue( m_pD3DDevice, (D3DXHANDLE)location, value, sizeof(Vector3) );
    }

    VOID DX9GraphicContext::SetVertexShaderConstantVector( UINT32 location, const Vector2* value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->vertexShader->vertexShaderConstant->SetValue( m_pD3DDevice, (D3DXHANDLE)location, value, sizeof(Vector2) );
    }

    UINT32 DX9GraphicContext::GetPixelShaderConstantLocation( SHADER_PROGRAM_HANDLE program, const CHAR* name )
    {
        ShaderProgram* pShader = (ShaderProgram*)program;
        return (UINT32)pShader->pixelShader->pixelShaderConstant->GetConstantByName( NULL, name );
    }

    VOID DX9GraphicContext::SetPixelShaderConstantFloat( UINT32 location, FLOAT32 value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->pixelShader->pixelShaderConstant->SetFloat( m_pD3DDevice, (D3DXHANDLE)location, value );
    }

    VOID DX9GraphicContext::SetPixelShaderConstantFloatArray( UINT32 location, const FLOAT32* value, UINT32 count )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->pixelShader->pixelShaderConstant->SetFloatArray( m_pD3DDevice, (D3DXHANDLE)location, value, count );
    }

    VOID DX9GraphicContext::SetPixelShaderConstantInt( UINT32 location, INT32 value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->pixelShader->pixelShaderConstant->SetInt( m_pD3DDevice, (D3DXHANDLE)location, value );
    }

    VOID DX9GraphicContext::SetPixelShaderConstantIntArray( UINT32 location, const INT32* value, UINT32 count )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->pixelShader->pixelShaderConstant->SetIntArray( m_pD3DDevice, (D3DXHANDLE)location, value, count );
    }

    VOID DX9GraphicContext::SetPixelShaderConstantMatrix( UINT32 location, const Matrix16* value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->pixelShader->pixelShaderConstant->SetMatrix( m_pD3DDevice, (D3DXHANDLE)location, value );
    }

    VOID DX9GraphicContext::SetPixelShaderConstantMatrix( UINT32 location, const Matrix9* value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->pixelShader->pixelShaderConstant->SetValue( m_pD3DDevice, (D3DXHANDLE)location, value, sizeof(Matrix9) );
    }

    VOID DX9GraphicContext::SetPixelShaderConstantMatrixArray( UINT32 location, const Matrix16* value, UINT32 count )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->pixelShader->pixelShaderConstant->SetMatrixArray( m_pD3DDevice, (D3DXHANDLE)location, value, count );
    }

    VOID DX9GraphicContext::SetPixelShaderConstantVector( UINT32 location, const Vector4* value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->pixelShader->pixelShaderConstant->SetVector( m_pD3DDevice, (D3DXHANDLE)location, value );
    }

    VOID DX9GraphicContext::SetPixelShaderConstantVectorArray( UINT32 location, const Vector4* value, UINT32 count )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->pixelShader->pixelShaderConstant->SetVectorArray( m_pD3DDevice, (D3DXHANDLE)location, value, count );
    }

    VOID DX9GraphicContext::SetPixelShaderConstantVector( UINT32 location, const Vector3* value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->pixelShader->pixelShaderConstant->SetValue( m_pD3DDevice, (D3DXHANDLE)location, value, sizeof(Vector3) );
    }

    VOID DX9GraphicContext::SetPixelShaderConstantVector( UINT32 location, const Vector2* value )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        pShader->pixelShader->pixelShaderConstant->SetValue( m_pD3DDevice, (D3DXHANDLE)location, value, sizeof(Vector2) );
    }

    VOID DX9GraphicContext::SetShaderTexture( SHADER_PROGRAM_HANDLE program, const CHAR* name, UINT32 stage, TEXTURE_HANDLE texture )
    {
        ShaderProgram* pShader = (ShaderProgram*)m_currentShaderProgram;
        D3DXHANDLE location = pShader->pixelShader->pixelShaderConstant->GetConstantByName( NULL, name );
        UINT32 index = pShader->pixelShader->pixelShaderConstant->GetSamplerIndex( location );
        m_pD3DDevice->SetTexture( index, (LPDIRECT3DTEXTURE9)texture );
    }

    VOID DX9GraphicContext::Release()
    {
        delete this;
    }
}