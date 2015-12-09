#if !defined ( _IPHONE_FONT_H_ )
#define _IPHONE_FONT_H_

#include "CharacterBasedFont.h"

namespace Graphic
{
    class IPhoneFont
        : public CharacterBasedFont
    {
    public:
        IPhoneFont();
        virtual ~IPhoneFont();

        BOOL Initialize( CHAR* name = NULL );
        VOID Destroy();
    protected:
        virtual BOOL GetChar( WCHAR character, VOID* &image, UINT16 &width, UINT16 &height );
    private:
        VOID* m_pUIFont;
    };

    class IPhoneFontManager
        : public FontManager
    {
    public:
        IPhoneFontManager();
        virtual ~IPhoneFontManager();

        virtual IFont* GetFont( FontTypes type );

    protected:
        // GraphicResourceController interface
        virtual BOOL OnInitializeResource( IGraphicContext* pGraphicContext );
        virtual BOOL OnResetResourceDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext );
        virtual VOID OnDestroyResource( IGraphicContext* pGraphicContext );
    private:
        IPhoneFont*                 m_pDefaultFont;
        IGraphicContext*            m_pGraphicContext;
    };
}

#endif
