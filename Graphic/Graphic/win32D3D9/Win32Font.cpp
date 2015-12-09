#include "Win32Font.h"

#define FONT_SIZE           64

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
namespace Graphic
{
    Win32Font::Win32Font()
        : m_pFont( NULL )
    {
    }

    Win32Font::~Win32Font()
    {
        Destroy();
    }

    BOOL Win32Font::Initialize( CHAR* name )
    {
        if ( NULL == name )
        {
            m_pFont = CreateFontA( FONT_SIZE, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "@system" );
        }
        else
        {
            m_pFont = CreateFontA( FONT_SIZE, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, name );
        }
        if ( NULL == m_pFont )
        {
            return FALSE;
        }
        return TRUE;
    }

    VOID Win32Font::Destroy()
    {
        if ( NULL != m_pFont )
        {
            DeleteObject( m_pFont );
            m_pFont = NULL;
        }
    }

    BOOL Win32Font::GetChar( WCHAR character, VOID* &image, UINT16 &width, UINT16 &height )
    {
        HDC dc = CreateCompatibleDC( NULL );
        if ( NULL == dc )
        {
            return FALSE;
        }
        SelectObject( dc, m_pFont );

        WORD characterIndex = 0;
        if ( GDI_ERROR == GetGlyphIndicesW( dc, &character, 1, &characterIndex, GGI_MARK_NONEXISTING_GLYPHS ) )
        {
            DeleteDC( dc );
            return FALSE;
        }
        INT characterWidth = 0;
        if ( !GetCharWidth32( dc, characterIndex, characterIndex, &characterWidth ) )
        {
            DeleteDC( dc );
            return FALSE;
        }
        width = characterWidth;
        height = FONT_SIZE;
        HBITMAP bitmap = CreateCompatibleBitmap( dc, width, height );
        if ( NULL == bitmap )
        {
            DeleteDC( dc );
            return FALSE;
        }
        SelectObject( dc, bitmap );
        SetTextColor( dc, RGB(255, 255, 255) );
        SetBkColor( dc, RGB(0, 0, 0) );
        RECT rect = { 0, 0, width, height };
        if ( 0 == DrawTextW( dc, &character, 1, &rect, DT_VCENTER ) )
        {
            DeleteObject( bitmap );
            DeleteDC( dc );
            return FALSE;
        }

        BITMAPINFO bitmapInfo = { 0 };
        bitmapInfo.bmiHeader.biBitCount = 0;
        bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        if ( 0 == GetDIBits( dc, bitmap, 0, height, NULL, &bitmapInfo, DIB_RGB_COLORS ) )
        {
            DWORD err = GetLastError();
            DeleteObject( bitmap );
            DeleteDC( dc );
            return FALSE;
        }
        bitmapInfo.bmiHeader.biCompression = BI_RGB;
        bitmapInfo.bmiHeader.biPlanes = 1;
        UINT32 lineSize = ( ( width * bitmapInfo.bmiHeader.biBitCount ) + 31 ) / 32 * 4;
        VOID* pData = malloc( lineSize * height );
        if ( NULL == pData )
        {
            DeleteObject( bitmap );
            DeleteDC( dc );
            return FALSE;
        }

        if ( 0 == GetDIBits( dc, bitmap, 0, height, pData, &bitmapInfo, DIB_RGB_COLORS ) )
        {
            DeleteObject( bitmap );
            DeleteDC( dc );
            free( pData );
            return FALSE;
        }
        image = malloc( width * height * 4 );
        memset( image, 0, width * height * 4 );
        if ( NULL == image )
        {
            DeleteObject( bitmap );
            DeleteDC( dc );
            free( pData );
            return FALSE;
        }

        UINT32 mask = 0;
        for ( UINT32 i = 0; i < bitmapInfo.bmiHeader.biBitCount; ++i )
        {
            mask |= 1 << i;
        }
        for ( UINT32 i = 0; i < height; ++i )
        {
            for ( UINT32 j = 0; j < width; ++j )
            {
                UINT32 index = i * width + j;
                UINT32* pPixel = (UINT32*)image + index;
                BYTE* lineData = (BYTE*)pData + (height - 1 - i) * lineSize;
                UINT32 bitOffset = j * bitmapInfo.bmiHeader.biBitCount;
                UINT32 offset = bitOffset / 8;
                UINT32 testBit = bitOffset - offset * 8;
                if ( lineData[offset] & ( 1 << (7 - testBit) ) )
                {
                    *pPixel = 0xFFFFFFFF;
                }
            }
        }

        free( pData );
        DeleteObject( bitmap );
        DeleteDC( dc );
        return TRUE;
    }

    Win32FontManager::Win32FontManager()
        : m_pDefaultFont( NULL )
    {
    }

    Win32FontManager::~Win32FontManager()
    {
        if ( NULL != m_pDefaultFont )
        {
            delete m_pDefaultFont;
        }
    }

    IFont* Win32FontManager::GetFont( FontTypes type )
    {
        switch ( type )
        {
            case FontType_Default:
            default:
                return m_pDefaultFont;
        }
    }

    BOOL Win32FontManager::OnInitializeResource( IGraphicContext* pGraphicContext )
    {
        if ( NULL == m_pDefaultFont )
        {
            m_pDefaultFont = new Win32Font();
            if ( NULL == m_pDefaultFont || !m_pDefaultFont->Initialize("Times New Roman") )
            {
                return FALSE;
            }
        }
        m_pGraphicContext = pGraphicContext;
        m_pDefaultFont->SetGraphicContext( m_pGraphicContext );

        return TRUE;
    }

    BOOL Win32FontManager::OnResetResourceDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext )
    {
        DestroyResource( pOldGraphicContext );
        return InitializeResource( pNewGraphicContext );
    }

    VOID Win32FontManager::OnDestroyResource( IGraphicContext* pGraphicContext )
    {
        m_pDefaultFont->SetGraphicContext( NULL );
    }
}
