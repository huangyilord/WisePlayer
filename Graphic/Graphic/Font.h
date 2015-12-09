#if !defined ( _SHOOT_CLUB_I_FONT_H_ )
#define _SHOOT_CLUB_I_FONT_H_

#include "GraphicResourceMgr.h"
#include "GraphicTypes.h"

#include <list>

namespace Graphic
{
    typedef struct CharGraphicInfo
    {
        TextureInfo     texInfo;
        UINT16          height;
        UINT16          width;
    } CharGraphicInfo;

    class IFont
    {
    public:
        virtual ~IFont() {}
        virtual BOOL CreateString( const CHAR* text, std::list<CharGraphicInfo*>& info ) = 0;
        virtual BOOL CreateString( const WCHAR* text, UINT32 len, std::list<CharGraphicInfo*>& info ) = 0;
        virtual VOID DestroyString( const CHAR* text ) = 0;
        virtual VOID DestroyString( const WCHAR* text, UINT32 len ) = 0;
    };

    class FontManager
        : public GraphicResourceController
    {
    public:
        enum FontTypes
        {
            FontType_Default,
            
            FontType_Num
        };

        virtual IFont* GetFont( FontTypes type = FontType_Default ) = 0;
    };
}

#endif
