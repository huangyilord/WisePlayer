#if !defined ( _ANDROID_FONT_H_ )
#define _ANDROID_FONT_H_

#include "../CharacterBasedFont.h"
#include <string>

namespace Graphic
{
    class AndroidFont
        : public CharacterBasedFont
    {
    public:
        AndroidFont();
        virtual ~AndroidFont();

        BOOL Initialize( CHAR* name = NULL );
        VOID Destroy();
    protected:
        virtual BOOL GetChar( WCHAR character, VOID* &image, UINT16 &width, UINT16 &height );
    private:
        std::string                     m_fontString;
    };

    class AndroidFontManager
        : public FontManager
    {
    public:
        AndroidFontManager();
        virtual ~AndroidFontManager();

        virtual IFont* GetFont( FontTypes type );

    protected:
        // GraphicResourceController interface
        virtual BOOL OnInitializeResource( IGraphicContext* pGraphicContext );
        virtual BOOL OnResetResourceDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext );
        virtual VOID OnDestroyResource( IGraphicContext* pGraphicContext );
    private:
        AndroidFont*                  m_pDefaultFont;
        IGraphicContext*              m_pGraphicContext;
    };
}

#endif
