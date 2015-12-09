#include "DWordCompressor.h"

namespace Util
{
	DWordCompressor DWordCompressor::s_Instance;

	DWordCompressor::DWordCompressor()
	{
	}

	DWordCompressor::~DWordCompressor()
	{
	}

	UINT32 DWordCompressor::PutDWord( BYTE* lpBuffer, DWORD dwValue )
	{
		if ( dwValue <= 0x7F )
		{
			lpBuffer[0] = ( BYTE )dwValue;
			return 1;
		}
		else if ( dwValue <= 0x3FFF )
		{
			lpBuffer[0] = ( BYTE )( 0x80 | ( dwValue >> 8 ) );
			lpBuffer[1] = ( BYTE )( 0xFF & dwValue );
			return 2;
		}
		else if ( dwValue <= 0x1FFFFF )	
		{
			lpBuffer[0] = ( BYTE )( 0xC0 | ( dwValue >> 16 ) );
			lpBuffer[1] = ( BYTE )( ( 0xFF00 & dwValue ) >> 8 );
			lpBuffer[2] = ( BYTE )( 0xFF & dwValue );
			return 3;
		}
		else if ( dwValue <= 0xFFFFFFF )
		{
			lpBuffer[0] = ( BYTE )( 0xE0 | ( dwValue >> 24 ) );
			lpBuffer[1] = ( BYTE )( ( 0xFF0000 & dwValue ) >> 16 );
			lpBuffer[2] = ( BYTE )( ( 0xFF00 & dwValue ) >> 8 );
			lpBuffer[3] = ( BYTE )( 0xFF & dwValue );
			return 4;
		}
		else
		{
			lpBuffer[0] = 0xF0;
			lpBuffer[1] = ( BYTE )( ( 0xFF000000 & dwValue ) >> 24 );
			lpBuffer[2] = ( BYTE )( ( 0xFF0000 & dwValue ) >> 16 );
			lpBuffer[3] = ( BYTE )( ( 0xFF00 & dwValue ) >> 8 );
			lpBuffer[4] = ( BYTE )( 0xFF & dwValue );
			return 5;
		}
	}

	UINT32 DWordCompressor::GetDWord( DWORD& dwValue, GET_DATA_CALLBACK callback, VOID* pParam )
	{
		BYTE mask = 0x80;
		UINT32 i = 0;
		BYTE byHead = 0;
		if ( 0 == callback( &byHead, 1, pParam ) )
		{
			return 0;
		}
		while ( ( mask & byHead ) != 0 )
		{
			i++;
			mask = mask >> 1;
		}
		BYTE byDecompressBuffer[4];
		switch ( i )
		{
		case 0:
			dwValue = byHead;
			return 1;
		case 1:
			{
				if ( 0 == callback( byDecompressBuffer, 1, pParam ) )
				{
					return 0;
				}
				dwValue = byHead & 0x3F;
				dwValue = ( dwValue << 8 ) | byDecompressBuffer[0];
			}
			return 2;
		case 2:
			{
				if ( 2 != callback( byDecompressBuffer, 2, pParam ) )
				{
					return 0;
				}
				dwValue = byHead & 0x1F;
				dwValue = ( dwValue << 8 ) | byDecompressBuffer[0];
				dwValue = ( dwValue << 8 ) | byDecompressBuffer[1];
			}
			return 3;
		case 3:
			{
				if ( 3 != callback( byDecompressBuffer, 3, pParam ) )
				{
					return 0;
				}
				dwValue = byHead & 0xF;
				dwValue = ( dwValue << 8 ) | byDecompressBuffer[0];
				dwValue = ( dwValue << 8 ) | byDecompressBuffer[1];
				dwValue = ( dwValue << 8 ) | byDecompressBuffer[2];
			}
			return 4;
		default:
			{
				if ( 4 != callback( byDecompressBuffer, 4, pParam ) )
				{
					return 0;
				}
				dwValue = byDecompressBuffer[0];
				dwValue = ( dwValue << 8 ) | byDecompressBuffer[1];
				dwValue = ( dwValue << 8 ) | byDecompressBuffer[2];
				dwValue = ( dwValue << 8 ) | byDecompressBuffer[3];
			}
			return 5;
		}
	}

	UINT32 DWordCompressor::Put( const VOID* lpSrcBuffer, UINT32 uSrcSize, UINT32* pSizeCompressed, PUT_DATA_CALLBACK callback, VOID* pParam )
	{
		BYTE byCompressBuffer[5];
		DWORD* pBuffer = ( DWORD* )lpSrcBuffer;
		UINT32 uSizePut = 0;
		UINT32 uTotalCompSize = 0;
		UINT32 uCompressedSize = 0;
		while ( uSrcSize - uSizePut > 3 )
		{
			uCompressedSize = PutDWord( byCompressBuffer, *( pBuffer++ ) );
			if ( uCompressedSize != callback( byCompressBuffer, uCompressedSize, pParam ) )
			{
				if ( NULL != pSizeCompressed )
				{
					*pSizeCompressed = uTotalCompSize;
				}
				return uSizePut;
			}
			uTotalCompSize += uCompressedSize;
			uSizePut += 4;
		}
		if ( uSizePut == uSrcSize )
		{
			if ( NULL != pSizeCompressed )
			{
				*pSizeCompressed = uTotalCompSize;
			}
			return uSizePut;
		}
		BYTE* pby = ( BYTE* )lpSrcBuffer + uSizePut;
		BYTE dword[4] = { 0 };
		UINT32 uRest = uSrcSize - uSizePut;
		for ( UINT32 i = 0; i < uRest; ++i )
		{
			dword[i] = pby[i];
		}
		uCompressedSize = PutDWord( byCompressBuffer, *( DWORD* )dword );
		if ( uCompressedSize != callback( byCompressBuffer, uCompressedSize, pParam ) )
		{
			if ( NULL != pSizeCompressed )
			{
				*pSizeCompressed = uTotalCompSize;
			}
			return uSizePut;
		}
		uTotalCompSize += uCompressedSize;
		if ( NULL != pSizeCompressed )
		{
			*pSizeCompressed = uTotalCompSize;
		}
		return uSrcSize;
	}

	UINT32 DWordCompressor::Get( VOID* lpDstBuffer, UINT32 uDstSize, UINT32* pSizeCompressed, GET_DATA_CALLBACK callback, VOID* pParam )
	{
		DWORD* pBuffer = ( DWORD* )lpDstBuffer;
		UINT32 uSizeGot = 0;
		DWORD dwValue;
		UINT32 uTotalCompSize = 0;
		UINT32 uCompressedSize = 0;
		while ( uDstSize - uSizeGot > 3 )
		{
			uCompressedSize = GetDWord( dwValue, callback, pParam );
			if ( 0 == uCompressedSize )
			{
				if ( NULL != pSizeCompressed )
				{
					*pSizeCompressed = uTotalCompSize;
				}
				return uSizeGot;
			}
			uTotalCompSize += uCompressedSize;
			*( pBuffer++ ) = dwValue;
			uSizeGot += 4;
		}
		if ( uSizeGot == uDstSize )
		{
			if ( NULL != pSizeCompressed )
			{
				*pSizeCompressed = uTotalCompSize;
			}
			return uSizeGot;
		}
		uCompressedSize = GetDWord( dwValue, callback, pParam );
		if ( 0 == uCompressedSize )
		{
			if ( NULL != pSizeCompressed )
			{
				*pSizeCompressed = uTotalCompSize;
			}
			return uSizeGot;
		}
		uTotalCompSize += uCompressedSize;
		BYTE* pby = ( BYTE* )lpDstBuffer + uSizeGot;
		BYTE* dword = ( BYTE* )&dwValue;
		UINT32 uRest = uDstSize - uSizeGot;
		for ( UINT32 i = 0; i < uRest; ++i )
		{
			pby[i] = dword[i];
		}
		if ( NULL != pSizeCompressed )
		{
			*pSizeCompressed = uTotalCompSize;
		}
		return uDstSize;
	}
}