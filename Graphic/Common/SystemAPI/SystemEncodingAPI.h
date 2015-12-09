#if !defined ( _SYSTEM_ENCODING_API_H_ )
#define _SYSTEM_ENCODING_API_H_

#define MAX_STRING_LENTH 511

namespace SystemAPI
{
    class SystemEncodingAPI
    {
    public:
        /**
         *  Convert string from UCS-2 to UTF-8
         *
         *  @param utf8Str: Output the string encoded by UTF-8.
         *  @param wstr: The string encoded by UCS-2.
         *  @param size: The buffer size of utf8Str in bytes.
         *  @return: Returns the character count converted.
         */
        static UINT32 WideToUTF8( CHAR* utf8Str, const WCHAR* wstr, UINT32 size );

        /**
         *  Convert string from UTF-8 to UCS-2
         *
         *  @param wstr: Output the result string encoded by UCS-2.
         *  @param utf8Str: The string encoded by UTF-8.
         *  @param size: The buffer size of wstr in bytes.
         *  @return: Returns the character count converted.
         */
        static UINT32 UTF8ToWide( WCHAR* wstr, const CHAR* utf8Str, UINT32 size );
    };
}

#endif
