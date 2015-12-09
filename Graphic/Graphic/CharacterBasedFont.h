#if !defined ( _CHARACTER_BASED_FONT_H_ )
#define _CHARACTER_BASED_FONT_H_

#define TEXTURE_SIZE        512
#define CHARACTER_SLOT_SIZE 64
#define SLOT_BLOCK_SIZE     (TEXTURE_SIZE / CHARACTER_SLOT_SIZE) * (TEXTURE_SIZE / CHARACTER_SLOT_SIZE)

#include "Font.h"
#include <map>
#include <set>

namespace  Graphic
{
    class CharacterBasedFont
        : public IFont
    {
    public:
        virtual ~CharacterBasedFont();

        virtual BOOL CreateString( const CHAR* text, std::list<CharGraphicInfo*>& info );
        virtual BOOL CreateString( const WCHAR* text, UINT32 len, std::list<CharGraphicInfo*>& info );
        virtual VOID DestroyString( const CHAR* text );
        virtual VOID DestroyString( const WCHAR* text, UINT32 len );
        
        VOID SetGraphicContext( IGraphicContext* pGraphicContext );
        VOID ClearCache();
    protected:
        CharacterBasedFont();
        
        virtual BOOL GetChar( WCHAR character, VOID* &image, UINT16 &width, UINT16 &height ) = 0;
        
    private:
        typedef struct CharInfo
        {
            CharGraphicInfo graphicInfo;
            CharInfo*       pNext;
            CharInfo*       pPrev;
            UINT16          x;
            UINT16          y;
            UINT16          refCount;
            WCHAR           character;
        } CharInfo;

        typedef struct SlotRes
        {
            TEXTURE_HANDLE  texID;
            CharInfo        charInfos[SLOT_BLOCK_SIZE];
            SlotRes*        pNext;
            SlotRes*        pPrev;
        } SlotRes;

        inline VOID AppendRes( SlotRes* pRes );
        inline VOID RemoveRes( SlotRes* pRes );
        
        inline VOID AppendCharInfoBack( CharInfo* pCharInfo );
        inline VOID AppendCharInfoFront( CharInfo* pCharInfo );
        inline VOID RemoveCharInfo( CharInfo* pCharInfo );

        BOOL DrawTexture( VOID* image, CharInfo* pCharInfo );

        std::map<WCHAR, CharInfo*>              m_charMap;
        CharInfo*                               m_slots;
        SlotRes*                                m_res;
        IGraphicContext*                        m_pGraphicContext;
    };
}

#endif
