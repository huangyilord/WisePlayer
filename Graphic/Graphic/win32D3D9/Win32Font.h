#if !defined ( _WIN32_FONT_H_ )
#define _WIN32_FONT_H_

#include "../CharacterBasedFont.h"

namespace Graphic
{
    class Win32Font
        : public CharacterBasedFont
    {
    public:
        Win32Font();
        virtual ~Win32Font();

        BOOL Initialize( CHAR* name = NULL );
        VOID Destroy();
    protected:
        virtual BOOL GetChar( WCHAR character, VOID* &image, UINT16 &width, UINT16 &height );
    private:
        HFONT                       m_pFont;
    };

    class Win32FontManager
        : public FontManager
    {
    public:
        Win32FontManager();
        virtual ~Win32FontManager();

        virtual IFont* GetFont( FontTypes type );

    protected:
        // GraphicResourceController interface
        virtual BOOL OnInitializeResource( IGraphicContext* pGraphicContext );
        virtual BOOL OnResetResourceDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext );
        virtual VOID OnDestroyResource( IGraphicContext* pGraphicContext );
    private:
        Win32Font*                  m_pDefaultFont;
        IGraphicContext*            m_pGraphicContext;
    };
}

#endif
