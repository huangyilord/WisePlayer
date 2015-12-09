#include "FormatConvert.h"
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#pragma warning(disable: 4127)
#pragma warning(disable: 4996)
namespace Util
{
	bool FormatConvert::ASCIIFloatArrayToBinFloatArray(char* asciiArray, unsigned int asciiSize, std::vector<float>& binArray)
	{
		char buffer[100];
		int bufferCursor = 0;

		if ( *(asciiArray+asciiSize-1) !='\0' )
		{
			asciiSize++;
		}

		for ( unsigned int i=0; i < asciiSize; i++ )
		{
			if ( asciiArray[i] == ' ' || asciiArray[i] == '\t' )
				continue;
			
			if ( asciiArray[i] == ',' || asciiArray[i] == '\0' )
			{
				if ( bufferCursor >= 100 )
					return false;

				buffer[bufferCursor] = '\0';
				
				float bin = (float)atof(buffer);
				binArray.push_back(bin);

				bufferCursor = 0;
			}
			else
			{
				buffer[bufferCursor] = asciiArray[i];
				bufferCursor++;
			}   
		}

		return true;
	}

	BOOL FormatConvert::IPv4Str2IPByteArray( const char* ipStr, BYTE* pByteArray, UINT32 byteArraySize )
    {
        if (byteArraySize!=4)
        {
            return FALSE;
        }

        // *.*.*.*   
        // ***.***.***.***
        if (strlen(ipStr) < 7 || strlen(ipStr) > 15)
        {
            return FALSE;
        }

        // Remove space and \t from head
        const char* pCursor = ipStr;
        while(true)
        {
            if (*pCursor=='\0')
            {
                return FALSE;
            }
            else if (*pCursor==' '||*pCursor=='\t')
            {
                pCursor++;
            }
            else
            {
                break;
            }
        }

        // Remove space and \t from tail
        const char* pStrEnd = ipStr + strlen(ipStr);
        while(true)
        {        
            if (pCursor==pStrEnd)
            {
                return FALSE;
            }
            else if (*pStrEnd==' '||*pStrEnd=='\t'||*pStrEnd=='\0')
            {
                pStrEnd--;
            }
            else
            {
                break;
            }
        }
        pStrEnd++;

        int byteArrayCursor = 0;
        char buffer[4];
        int bufferCursor = 0;
        memset(buffer, 0, 4);
        while(true)
        {
            if (*pCursor=='.'||pCursor==pStrEnd)
            {
                UINT32 uint32Value = 0;
                if (bufferCursor==0)
                {
                    return FALSE;
                }
                sscanf(buffer, "%u", &uint32Value);

                if (uint32Value > 255)
                {
                    return FALSE;
                }
                else
                {
                    if (byteArrayCursor >= 4)
                    {
                        return FALSE;
                    }

                    pByteArray[byteArrayCursor] = (BYTE)uint32Value;
                }

                byteArrayCursor++;
                bufferCursor = 0;
                memset(buffer, 0, 4); 

                if (pCursor == pStrEnd)
                {
                    break;
                }
            }
            else
            {
                if (!( '0' <= *pCursor && *pCursor <= '9'))
                {
                    return FALSE;
                }

                if (bufferCursor >= 3)
                {
                    return FALSE;
                }

                buffer[bufferCursor] = *pCursor; 
                bufferCursor++;
            }

            pCursor++;
        }

        if (byteArrayCursor!=4)
        {
            return FALSE;
        }

        return TRUE;
    }
}
