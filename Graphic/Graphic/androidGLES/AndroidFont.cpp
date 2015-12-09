#include "AndroidFont.h"

#define FONT_SIZE           64

void generateImage(WCHAR character, VOID* &image, UINT16 &width, UINT16 &height );

namespace Graphic
{
    AndroidFont::AndroidFont()
        : m_fontString( "" )
    {
    }

    AndroidFont::~AndroidFont()
    {
        Destroy();
    }

    BOOL AndroidFont::Initialize( CHAR* name )
    {
    	if (name != NULL)
    	{
    		m_fontString = name;
    	}
        return TRUE;
    }

    VOID AndroidFont::Destroy()
    {
        m_fontString = "";
    }

    BOOL AndroidFont::GetChar( WCHAR character, VOID* &image, UINT16 &width, UINT16 &height )
    {        
		generateImage(character, image, width, height);
    }

    AndroidFontManager::AndroidFontManager()
        : m_pDefaultFont( NULL )
    {
    }

    AndroidFontManager::~AndroidFontManager()
    {
        if ( NULL != m_pDefaultFont )
        {
            delete m_pDefaultFont;
        }
    }

    IFont* AndroidFontManager::GetFont( FontTypes type )
    {
        switch ( type )
        {
            case FontType_Default:
            default:
                return m_pDefaultFont;
        }
    }

    BOOL AndroidFontManager::OnInitializeResource( IGraphicContext* pGraphicContext )
    {
        if ( NULL == m_pDefaultFont )
        {
            m_pDefaultFont = new AndroidFont();
            if ( NULL == m_pDefaultFont || !m_pDefaultFont->Initialize() )
            {
                return FALSE;
            }
        }
        m_pGraphicContext = pGraphicContext;
        m_pDefaultFont->SetGraphicContext( m_pGraphicContext );

        return TRUE;
    }

    BOOL AndroidFontManager::OnResetResourceDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext )
    {
        DestroyResource( pOldGraphicContext );
        return InitializeResource( pNewGraphicContext );
    }

    VOID AndroidFontManager::OnDestroyResource( IGraphicContext* pGraphicContext )
    {
        m_pDefaultFont->SetGraphicContext( NULL );
    }
}
