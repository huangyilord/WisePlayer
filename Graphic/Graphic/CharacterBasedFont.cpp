#include "CharacterBasedFont.h"
#include "SystemAPI/SystemEncodingAPI.h"
#include "IGraphicContext.h"

#include <stdlib.h>

#define ENCODE_BUFFER_SIZE  1024

namespace Graphic
{
    CharacterBasedFont::CharacterBasedFont()
        : m_slots( NULL )
        , m_res( NULL )
        , m_pGraphicContext( NULL )
    {
    }

    CharacterBasedFont::~CharacterBasedFont()
    {
        ClearCache();
    }

    BOOL CharacterBasedFont::CreateString( const CHAR* text, std::list<CharGraphicInfo*>& info )
    {
        BYTE buffer[ENCODE_BUFFER_SIZE];
        memset(buffer, 0, ENCODE_BUFFER_SIZE);
        UINT32 textLen = SystemAPI::SystemEncodingAPI::UTF8ToWide( (WCHAR*)buffer, text, ENCODE_BUFFER_SIZE );
        return CreateString( (WCHAR*)buffer, textLen, info );
    }
    BOOL CharacterBasedFont::CreateString( const WCHAR* text, UINT32 len, std::list<CharGraphicInfo*>& info )
    {
        for ( UINT32 i = 0; i < len; ++i )
        {
            std::map<WCHAR, CharInfo*>::iterator iter = m_charMap.find( text[i] );
            if ( iter != m_charMap.end() )
            {
                CharInfo* charInfo = iter->second;
                if ( 0 == charInfo->refCount )
                {
                    RemoveCharInfo( charInfo );
                }
                
                ++ charInfo->refCount;
                info.push_back( &charInfo->graphicInfo );
            }
            else
            {
                if ( NULL == m_slots )
                {
                    TEXTURE_HANDLE tex = m_pGraphicContext->CreateTexture( NULL, TEXTURE_SIZE, TEXTURE_SIZE, Graphic::IGraphicContext::PixelType_32_8888 );
                   
                    if ( NULL == tex )
                    {
                        info.clear();
                        DestroyString( text, i );
                        return FALSE;
                    }
                    SlotRes* pRes = (SlotRes*)malloc( sizeof(SlotRes) );
                    if ( NULL == pRes )
                    {
                        m_pGraphicContext->DestroyTexture( tex );
                        info.clear();
                        DestroyString( text, i );
                        return FALSE;
                    }
                    memset( pRes, 0, sizeof(SlotRes) );
                    pRes->texID = tex;
                    AppendRes( pRes );
                    for ( UINT32 i = 0; i < TEXTURE_SIZE / CHARACTER_SLOT_SIZE; ++i )
                    {
                        for ( UINT32 j = 0; j < TEXTURE_SIZE / CHARACTER_SLOT_SIZE; ++j )
                        {
                            CharInfo* pCharInfo = &pRes->charInfos[i * TEXTURE_SIZE / CHARACTER_SLOT_SIZE + j];
                            pCharInfo->graphicInfo.texInfo.texture = tex;
                            pCharInfo->graphicInfo.texInfo.texCoord.origin.x = j * (FLOAT32)CHARACTER_SLOT_SIZE / (FLOAT32)TEXTURE_SIZE;
                            pCharInfo->graphicInfo.texInfo.texCoord.origin.y = i * (FLOAT32)CHARACTER_SLOT_SIZE / (FLOAT32)TEXTURE_SIZE;
                            pCharInfo->graphicInfo.texInfo.texCoord.size.x = (FLOAT32)CHARACTER_SLOT_SIZE / (FLOAT32)TEXTURE_SIZE;
                            pCharInfo->graphicInfo.texInfo.texCoord.size.y = (FLOAT32)CHARACTER_SLOT_SIZE / (FLOAT32)TEXTURE_SIZE;
                            pCharInfo->x = j;
                            pCharInfo->y = i;
                            AppendCharInfoFront( pCharInfo );
                        }
                    }
                }
                ASSERT( NULL != m_slots );

                CharInfo* pCharInfo = m_slots;
                RemoveCharInfo( pCharInfo );
                if ( 0 != pCharInfo->character )
                {
                    // means the graphic still being cached
                    m_charMap.erase( pCharInfo->character );
                    pCharInfo->character = 0;
                }

                VOID* image = NULL;
                if ( !GetChar( text[i], image, pCharInfo->graphicInfo.width, pCharInfo->graphicInfo.height ) )
                {
                    AppendCharInfoFront( pCharInfo );
                    info.clear();
                    DestroyString( text, i );
                    return FALSE;
                }
                if ( !DrawTexture( image, pCharInfo ) )
                {
                    free( image );
                    AppendCharInfoFront( pCharInfo );
                    info.clear();
                    DestroyString( text, i );
                    return FALSE;
                }
                free( image );
                pCharInfo->character = text[i];
                m_charMap.insert( std::pair<WCHAR, CharInfo*>( pCharInfo->character, pCharInfo ) );
                info.push_back( &pCharInfo->graphicInfo );
            }
        }
        return TRUE;
    }

    VOID CharacterBasedFont::DestroyString( const CHAR* text )
    {
        BYTE buffer[ENCODE_BUFFER_SIZE];
        UINT32 textLen = SystemAPI::SystemEncodingAPI::UTF8ToWide( (WCHAR*)buffer, text, ENCODE_BUFFER_SIZE );
        DestroyString( (WCHAR*)buffer, textLen );
    }
    VOID CharacterBasedFont::DestroyString( const WCHAR* text, UINT32 len )
    {
        for ( UINT32 i = 0; i < len; ++i )
        {
            std::map<WCHAR, CharInfo*>::iterator iter = m_charMap.find( text[i] );
            if ( iter != m_charMap.end() )
            {
                CharInfo* charInfo = iter->second;
                if ( 0 < charInfo->refCount )
                {
                    -- charInfo->refCount;
                    if ( 0 == charInfo->refCount )
                    {
                        AppendCharInfoBack( charInfo );
                    }
                }
            }
        }
    }
    
    VOID CharacterBasedFont::SetGraphicContext( IGraphicContext* pGraphicContext )
    {
        ClearCache();
        m_pGraphicContext = pGraphicContext;
    }

    VOID CharacterBasedFont::ClearCache()
    {
        SlotRes* pRes = m_res;
        if ( NULL != pRes )
        {
            do
            {
                SlotRes* pNext = pRes->pNext;
                m_pGraphicContext->DestroyTexture( pRes->texID );
                free( pRes );
                pRes = pNext;
            } while ( m_res != pRes );
        }
        m_res = NULL;
        m_slots = NULL;
        m_charMap.clear();
    }

    VOID CharacterBasedFont::AppendRes( SlotRes* pRes )
    {
        if ( NULL == m_res )
        {
            m_res = pRes->pNext = pRes->pPrev = pRes;
        }
        else
        {
            pRes->pNext = m_res;
            pRes->pPrev = m_res->pPrev;
            pRes->pPrev->pNext = m_res->pPrev = pRes;
        }
    }

    VOID CharacterBasedFont::RemoveRes( SlotRes* pRes )
    {
        pRes->pNext->pPrev = pRes->pPrev;
        pRes->pPrev->pNext = pRes->pNext;
        if ( m_res == pRes )
        {
            if ( pRes == pRes->pNext )
            {
                m_res = NULL;
            }
            else
            {
                m_res = pRes->pNext;
            }
        }
    }
    
    VOID CharacterBasedFont::AppendCharInfoBack( CharInfo* pCharInfo )
    {
        if ( NULL == m_slots )
        {
            m_slots = pCharInfo->pNext = pCharInfo->pPrev = pCharInfo;
        }
        else
        {
            pCharInfo->pNext = m_slots;
            pCharInfo->pPrev = m_slots->pPrev;
            pCharInfo->pPrev->pNext = m_slots->pPrev = pCharInfo;
        }
    }

    VOID CharacterBasedFont::AppendCharInfoFront( CharInfo* pCharInfo )
    {
        if ( NULL == m_slots )
        {
            m_slots = pCharInfo->pNext = pCharInfo->pPrev = pCharInfo;
        }
        else
        {
            pCharInfo->pNext = m_slots;
            pCharInfo->pPrev = m_slots->pPrev;
            pCharInfo->pPrev->pNext = m_slots->pPrev = pCharInfo;
            m_slots = pCharInfo;
        }
    }

    VOID CharacterBasedFont::RemoveCharInfo( CharInfo* pCharInfo )
    {
        pCharInfo->pNext->pPrev = pCharInfo->pPrev;
        pCharInfo->pPrev->pNext = pCharInfo->pNext;
        if ( m_slots == pCharInfo )
        {
            if ( pCharInfo == pCharInfo->pNext )
            {
                m_slots = NULL;
            }
            else
            {
                m_slots = pCharInfo->pNext;
            }
        }
    }

    struct COLOR
    {
        UINT8 r;
        UINT8 g;
        UINT8 b;
        UINT8 a;
    };

    static COLOR Sampler2D( VOID* image, UINT32 width, UINT32 height, FLOAT32 fromX, FLOAT32 toX, FLOAT32 fromY, FLOAT32 toY )
    {
        COLOR result = {0};
        FLOAT32 fMinX = width * fromX;
        FLOAT32 fMaxX = width * toX;
        FLOAT32 fMinY = height * fromY;
        FLOAT32 fMaxY = height * toY;
        FLOAT32 totalSize = (fMaxX - fMinX) * (fMaxY - fMinY);

        FLOAT32 red = 0.f;
        FLOAT32 green = 0.f;
        FLOAT32 blue = 0.f;
        FLOAT32 alpha = 0.f;

        COLOR* pixels = (COLOR*)image;
        FLOAT32 x = fMinX;
        while ( x < fMaxX )
        {
            FLOAT32 nextX = (FLOAT32)((INT32)(x + 1.f));
            FLOAT32 dX = nextX - x;
            if ( fMaxX < nextX )
            {
                dX = fMaxX - x;
            }
            FLOAT32 y = fMinY;
            while ( y < fMaxY )
            {
                FLOAT32 nextY = (FLOAT32)((INT32)(y + 1.f));
                FLOAT32 dY = nextY - y;
                if ( fMaxY < nextY )
                {
                    dY = fMaxY - y;
                }
                FLOAT32 f = dX * dY / totalSize;
                COLOR pixel = pixels[(INT32)y * width + (INT32)x];
                red += pixel.r * f;
                green += pixel.g * f;
                blue += pixel.b * f;
                alpha += pixel.a * f;

                y = nextY;
            }
            x = nextX;
        }
        result.r = (UINT8)red;
        result.g = (UINT8)green;
        result.b = (UINT8)blue;
        result.a = (UINT8)alpha;
        return result;
    }

    BOOL CharacterBasedFont::DrawTexture( VOID* image, CharInfo* pCharInfo )
    {
        COLOR* stretchedImage = (COLOR*)malloc( CHARACTER_SLOT_SIZE * CHARACTER_SLOT_SIZE * 4 );
        if ( NULL == stretchedImage )
        {
            return FALSE;
        }
        for ( UINT32 i = 0; i < CHARACTER_SLOT_SIZE; ++i )
        {
            for ( UINT32 j = 0; j < CHARACTER_SLOT_SIZE; ++j )
            {
                COLOR pixel = Sampler2D( image, pCharInfo->graphicInfo.width, pCharInfo->graphicInfo.height
                                        , (FLOAT32)j / (FLOAT32)CHARACTER_SLOT_SIZE
                                        , (FLOAT32)(j + 1) / (FLOAT32)CHARACTER_SLOT_SIZE
                                        , (FLOAT32)i / (FLOAT32)CHARACTER_SLOT_SIZE
                                        , (FLOAT32)(i + 1) / (FLOAT32)CHARACTER_SLOT_SIZE);
                COLOR &output = stretchedImage[i * CHARACTER_SLOT_SIZE + j];
                output.r = output.g = output.b = output.a = pixel.a;
            }
        }
        m_pGraphicContext->TextureSubImage( pCharInfo->graphicInfo.texInfo.texture
                                           , stretchedImage
                                           , CHARACTER_SLOT_SIZE
                                           , CHARACTER_SLOT_SIZE
                                           , pCharInfo->x * CHARACTER_SLOT_SIZE
                                           , pCharInfo->y * CHARACTER_SLOT_SIZE);
        free( stretchedImage );
        return TRUE;
    }
}

