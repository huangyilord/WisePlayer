#include "SystemEncodingAPI.h"

#if defined ( WIN32 )
#include <windows.h>
#elif defined ( LINUX ) || defined ( MAC_OS ) || defined( ANDROID )
#include <wctype.h>
#include <wchar.h>
#include <string.h>
#endif

#define UCS2_NULL   0x0000
#define UTF8_NULL   0x00

namespace SystemAPI
{
    UINT32 SystemEncodingAPI::WideToUTF8( CHAR* utf8Str, const WCHAR* wstr, UINT32 size )
    {
#if defined ( WIN32 )
        return WideCharToMultiByte( CP_UTF8, 0, wstr, -1, utf8Str, size, NULL, NULL );
#elif defined ( LINUX ) || defined ( MAC_OS ) || defined( ANDROID )
        INT32 nConvertLen = 0;
        INT32 nSrcLen = (INT32)wcslen(wstr);
	    INT32 nSrcConvert = nSrcLen;
	    while ( wstr != 0 && *wstr != UCS2_NULL )
        {
		    WCHAR szSrc = *wstr;
		    BYTE byDest[3];
		    UINT32 nConv = 1;
		    if (szSrc <= 0x007F)
            {
			    byDest[0] = (BYTE)szSrc;
		    }
		    else if (szSrc <= 0x07FF)
            {
			    byDest[0] = (BYTE)(((szSrc >> 6) & 0x1F) | 0xC0);
			    byDest[1] = (BYTE)((szSrc & 0x3F) | 0x80);
			    nConv = 2;
		    }
		    else if (szSrc <= 0xFFFF)
            {
			    byDest[0] = (BYTE)(((szSrc >> 12) & 0x0F) | 0xE0);
			    byDest[1] = (BYTE)(((szSrc >> 6) & 0x3F) | 0x80);
			    byDest[2] = (BYTE)((szSrc & 0x3F) | 0x80);
			    nConv = 3;
		    }
		    else
            {
			    return 0;
		    }
		    if (nSrcLen >= 0 && nSrcConvert <= 0)
            {
			    break;
		    }
		    if (utf8Str != NULL && (INT32)size <= nConvertLen)
            {
			    break;
		    }
		    if (utf8Str != NULL)
            {
			    for (UINT32 n = 0; n < nConv; n++)
                {
				    *utf8Str++ = (CHAR)byDest[n];
			    }
		    }
		    wstr++;
		    nSrcConvert--;
		    nConvertLen += nConv;
	    }
	    if (nConvertLen < (INT32)size)
        {
		    *utf8Str = UTF8_NULL;
	    }
	    return (UINT32)nConvertLen;
#endif
    }

    UINT32 SystemEncodingAPI::UTF8ToWide( WCHAR* wstr, const CHAR* utf8Str, UINT32 size )
    {
#if defined ( WIN32 )
        return MultiByteToWideChar( CP_UTF8, 0, utf8Str, -1, wstr, size ) - 1;
#elif defined ( LINUX ) || defined ( MAC_OS ) || defined( ANDROID )
        INT32 nConvertLen = 0;
        INT32 nSrcLen = (INT32)strlen(utf8Str);
	    INT32 nSrcConvert = nSrcLen;
        size = size >> 1;
	    while ( utf8Str != NULL && *utf8Str != UTF8_NULL )
        {
		    WCHAR wUnicode = UCS2_NULL;
		    BYTE byCh1 = (BYTE)*utf8Str;
		    UINT32 nNext = 1;
		    if (byCh1 <= 0x7F)
            {
			    wUnicode = byCh1;
		    }
		    else if ((byCh1 & 0xE0) == 0xE0)
            {
			    nNext = 3;
			    BYTE byCh2 = (BYTE)*(utf8Str + 1);
			    BYTE byCh3 = (BYTE)*(utf8Str + 2);
			    wUnicode = (WCHAR)(((WCHAR)(byCh1 ^ 0xE0) << 12) | ((WCHAR)(byCh2 ^ 0x80) <<  6) | ((WCHAR)(byCh3 ^ 0x80) <<  0));
		    }
		    else if ((byCh1 & 0xC0) == 0xC0)
            {
			    nNext = 2;
			    BYTE byCh2 = (BYTE)*(utf8Str + 1);
			    wUnicode = (WCHAR)(((WCHAR)(byCh1 ^ 0xC0) <<  6) | ((WCHAR)(byCh2 ^ 0x80) <<  0));
		    }
		    else
            {
			    wUnicode = byCh1;
		    }
		    if (nSrcLen >= 0 && nSrcConvert <= 0)
            {
			    break;
		    }
		    if (wstr != NULL && (INT32)size <= nConvertLen)
            {
			    break;
		    }
		    if (wstr != NULL)
            {
			    *wstr++ = wUnicode;
		    }
		    utf8Str += nNext;
		    nSrcConvert -= nNext;
		    nConvertLen++;
	    }
	    if (nConvertLen < (INT32)size)
        {
		    *wstr = UCS2_NULL;
	    }
	    return (UINT32)nConvertLen;
#endif
    }
}
