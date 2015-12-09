#include "iphoneFont.h"
#include "SystemAPI/SystemEncodingAPI.h"

#define FONT_SIZE   64.f

namespace Graphic
{
    IPhoneFont::IPhoneFont()
        : m_pUIFont( NULL )
    {
    }

    IPhoneFont::~IPhoneFont()
    {
        Destroy();
    }

    BOOL IPhoneFont::Initialize( CHAR* name )
    {
        if ( NULL == name )
        {
            UIFont* pFont = [[UIFont italicSystemFontOfSize:FONT_SIZE] retain];
            if ( nil == pFont )
            {
                return FALSE;
            }
            m_pUIFont = (VOID*)pFont;
        }
        else
        {
            NSString* fontName = [NSString stringWithUTF8String:name];
            UIFont* pFont = [[UIFont fontWithName:fontName size:FONT_SIZE] retain];
            if ( nil == pFont )
            {
                return FALSE;
            }
            m_pUIFont = (VOID*)pFont;
        }
        return TRUE;
    }

    VOID IPhoneFont::Destroy()
    {
        UIFont* pFont = (UIFont*)m_pUIFont;
        [pFont release];
    }

    BOOL IPhoneFont::GetChar( WCHAR character, VOID* &image, UINT16 &width, UINT16 &height )
    {
        UIFont* pFont = (UIFont*)m_pUIFont;
        static CHAR s_utf8CharBuffer[8] = {0};
        static WCHAR s_wcharBuffer[2] = {0};
        s_wcharBuffer[0] = character;
        SystemAPI::SystemEncodingAPI::WideToUTF8( s_utf8CharBuffer, s_wcharBuffer, 8 );
        NSString* text = [NSString stringWithUTF8String:s_utf8CharBuffer];
        if ( nil == text )
        {
            return FALSE;
        }
        //CGSize size = [text sizeWithFont:pFont];
        CGSize size = [text sizeWithAttributes:@{ NSFontAttributeName : pFont }];
        width = size.width;
        height = size.height;
        image = malloc( width * height * 4 );
        if ( NULL == image )
        {
            return FALSE;
        }
        memset( image, 0, width * height * 4 );

        // draw text
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGContextRef context = CGBitmapContextCreate( image, width, height, 8, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
        CGColorSpaceRelease( colorSpace );
        CGContextTranslateCTM( context, 0.f, height );
        CGContextScaleCTM( context, 1.f, -1.f );
        UIGraphicsPushContext( context );
        UIColor* pColor = [UIColor colorWithWhite:1.f alpha:1.f];
        [pColor setFill];
        //[text drawInRect:CGRectMake(0, 0, width, height) withFont:pFont lineBreakMode:UILineBreakModeTailTruncation alignment:UITextAlignmentLeft];
        [text drawInRect:CGRectMake(0, 0, width, height) withAttributes:@{ NSFontAttributeName : pFont }];
        UIGraphicsPopContext();
        CGContextRelease(context);
        return TRUE;
    }
    
    IPhoneFontManager::IPhoneFontManager()
        : m_pDefaultFont( NULL )
    {
    }

    IPhoneFontManager::~IPhoneFontManager()
    {
        if ( NULL != m_pDefaultFont )
        {
            delete m_pDefaultFont;
        }
    }

    IFont* IPhoneFontManager::GetFont( FontTypes type )
    {
        switch ( type )
        {
            case FontType_Default:
            default:
                return m_pDefaultFont;
        }
    }

    BOOL IPhoneFontManager::OnInitializeResource( IGraphicContext* pGraphicContext )
    {
        if ( NULL == m_pDefaultFont )
        {
            m_pDefaultFont = new IPhoneFont();
            if ( NULL == m_pDefaultFont || !m_pDefaultFont->Initialize() )
            {
                return FALSE;
            }
        }
        m_pGraphicContext = pGraphicContext;
        m_pDefaultFont->SetGraphicContext( m_pGraphicContext );

        return TRUE;
    }

    BOOL IPhoneFontManager::OnResetResourceDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext )
    {
        DestroyResource( pOldGraphicContext );
        return InitializeResource( pNewGraphicContext );
    }

    VOID IPhoneFontManager::OnDestroyResource( IGraphicContext* pGraphicContext )
    {
        m_pDefaultFont->SetGraphicContext( NULL );
    }
}
