#include "IOBuffer.h"
#include <string.h>
namespace Util
{
	IOBuffer::IOBuffer( VOID* lpBuffer, UINT32 uSize, UINT32 uDataSize )
		: m_pLoadCallBack( NULL )
		, m_pLoadParam( NULL )
		, m_pPutCallBack( NULL )
		, m_pPutParam( NULL )
		, m_lpBuffer( (BYTE*)lpBuffer )
		, m_uSize( uSize )
		, m_uCurrentDataOffset( 0 )
		, m_uCurrentDataSize( uDataSize )
		, m_dwFlags( 0 )
		, m_ulCheckSum( 0 )
	{
	}

	IOBuffer::~IOBuffer() 
	{
	}

	UINT32 IOBuffer::Get( VOID* lpBuffer, UINT32 uSize )
	{
		if ( 0 == uSize )
		{
			return 0;
		}
		BYTE* lpOutBuffer = ( BYTE* )lpBuffer;
		UINT32 uBytesGot = 0;
		if ( m_uCurrentDataSize < uSize )
		{
			if ( m_uCurrentDataSize > ( m_uSize - m_uCurrentDataOffset ) )
			{
				::memcpy( lpOutBuffer, m_lpBuffer + m_uCurrentDataOffset, m_uSize - m_uCurrentDataOffset );
				uBytesGot = m_uSize - m_uCurrentDataOffset;
				::memcpy( lpOutBuffer + uBytesGot, m_lpBuffer, m_uCurrentDataSize - uBytesGot );
			}
			else
			{
				::memcpy( lpOutBuffer, m_lpBuffer + m_uCurrentDataOffset, m_uCurrentDataSize );

			}
			uBytesGot = m_uCurrentDataSize;
			m_uCurrentDataOffset = 0;
			m_uCurrentDataSize = CallLoadFunc( m_lpBuffer, m_uSize, m_pLoadParam );
			if ( 0 == m_uCurrentDataSize )
			{
				return uBytesGot;
			}
			while ( m_uCurrentDataSize < ( uSize - uBytesGot ) )
			{
				::memcpy( lpOutBuffer + uBytesGot, m_lpBuffer, m_uCurrentDataSize );
				uBytesGot += m_uCurrentDataSize;
				m_uCurrentDataSize = CallLoadFunc( m_lpBuffer, m_uSize, m_pLoadParam );
				if ( 0 == m_uCurrentDataSize )
				{
					return uBytesGot;
				}
			}
			::memcpy( lpOutBuffer + uBytesGot, m_lpBuffer, uSize - uBytesGot );
			m_uCurrentDataOffset += uSize - uBytesGot;
			m_uCurrentDataSize -= uSize - uBytesGot;
			if ( 0 == m_uCurrentDataSize )
			{
				m_uCurrentDataOffset = 0;
			}
			return uSize;
		}
		else
		{
			if ( m_uCurrentDataSize > ( m_uSize - m_uCurrentDataOffset ) )
			{
				if ( ( m_uSize - m_uCurrentDataOffset ) < uSize )
				{
					::memcpy( lpOutBuffer, m_lpBuffer + m_uCurrentDataOffset, m_uSize - m_uCurrentDataOffset );
					uBytesGot = m_uSize - m_uCurrentDataOffset;
					::memcpy( lpOutBuffer + uBytesGot, m_lpBuffer, uSize - uBytesGot );
					m_uCurrentDataOffset = uSize - uBytesGot;
					m_uCurrentDataSize -= uSize;
				}
				else
				{
					::memcpy( lpOutBuffer, m_lpBuffer + m_uCurrentDataOffset, uSize );
					m_uCurrentDataOffset = ( m_uCurrentDataOffset + uSize ) % m_uSize;
					m_uCurrentDataSize -= uSize;
				}
			}
			else
			{
				::memcpy( lpOutBuffer, m_lpBuffer + m_uCurrentDataOffset, uSize );
				m_uCurrentDataOffset = ( m_uCurrentDataOffset + uSize ) % m_uSize;
				m_uCurrentDataSize -= uSize;
			}
			if ( 0 == m_uCurrentDataSize )
			{
				m_uCurrentDataOffset = 0;
			}
			return uSize;
		}
	}

	UINT32 IOBuffer::Put( const VOID* lpBuffer, UINT32 uSize )
	{
		if ( 0 == uSize )
		{
			return 0;
		}
		BYTE* lpInBuffer = ( BYTE* )lpBuffer;
		UINT32 uBytesPut = 0;
		UINT32 uCurrentSpaceSize = m_uSize - m_uCurrentDataSize;
		UINT32 uCurrentSpaceOffset = ( m_uCurrentDataOffset + m_uCurrentDataSize ) % m_uSize;
		if ( uCurrentSpaceSize < uSize )
		{
			if ( uCurrentSpaceSize > ( m_uSize - uCurrentSpaceOffset ) )
			{
				::memcpy( m_lpBuffer + uCurrentSpaceOffset, lpInBuffer, m_uSize - uCurrentSpaceOffset );
				uBytesPut = m_uSize - uCurrentSpaceOffset;
				::memcpy( m_lpBuffer, lpInBuffer + uBytesPut, uCurrentSpaceSize - uBytesPut );
			}
			else
			{
				::memcpy( m_lpBuffer + uCurrentSpaceOffset, lpInBuffer, uCurrentSpaceSize );

			}
			uBytesPut = uCurrentSpaceSize;
			uCurrentSpaceOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
			if ( 0 == uCurrentSpaceOffset )
			{
				uCurrentSpaceSize = CallPutFunc( m_lpBuffer, m_uSize , m_pPutParam );
			}
			else
			{
				uCurrentSpaceSize = CallPutFunc( m_lpBuffer + uCurrentSpaceOffset, m_uSize - uCurrentSpaceOffset , m_pPutParam );
				if ( uCurrentSpaceSize != m_uSize - uCurrentSpaceOffset )
				{
					m_uCurrentDataSize = m_uSize - uCurrentSpaceSize;
					m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
					return uBytesPut;
				}
				uCurrentSpaceSize += CallPutFunc( m_lpBuffer, uCurrentSpaceOffset , m_pPutParam );
			}
			if ( uCurrentSpaceSize != m_uSize )
			{
				m_uCurrentDataSize = m_uSize - uCurrentSpaceSize;
				m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
				return uBytesPut;
			}
			uCurrentSpaceOffset = 0;
			while ( uCurrentSpaceSize < ( uSize - uBytesPut ) )
			{
				::memcpy( m_lpBuffer, lpInBuffer + uBytesPut, uCurrentSpaceSize );
				uBytesPut += uCurrentSpaceSize;
				uCurrentSpaceSize = CallPutFunc( m_lpBuffer, m_uSize, m_pPutParam );
				if ( m_uSize != uCurrentSpaceSize )
				{
					m_uCurrentDataSize = m_uSize;
					m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
					return uBytesPut;
				}
			}
			::memcpy( m_lpBuffer, lpInBuffer + uBytesPut, uSize - uBytesPut );
			uCurrentSpaceOffset = ( uSize - uBytesPut ) % m_uSize;
			uCurrentSpaceSize -= uSize - uBytesPut;
			if ( 0 == uCurrentSpaceSize )
			{
				uCurrentSpaceSize = CallPutFunc( m_lpBuffer, m_uSize, m_pPutParam );
				uCurrentSpaceSize = m_uSize;
			}
			m_uCurrentDataSize = m_uSize - uCurrentSpaceSize;
			m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
			return uSize;
		}
		else
		{
			if ( uCurrentSpaceSize > ( m_uSize - uCurrentSpaceOffset ) )
			{
				if ( ( m_uSize - uCurrentSpaceOffset ) < uSize )
				{
					::memcpy( m_lpBuffer + uCurrentSpaceOffset, lpInBuffer, m_uSize - uCurrentSpaceOffset );
					uBytesPut = m_uSize - uCurrentSpaceOffset;
					::memcpy( m_lpBuffer, lpInBuffer + uBytesPut, m_uSize - uBytesPut );
					uCurrentSpaceOffset = m_uSize - uBytesPut;
					uCurrentSpaceSize -= uSize;
				}
				else
				{
					::memcpy( m_lpBuffer + uCurrentSpaceOffset, lpInBuffer, uSize );
					uCurrentSpaceOffset = ( uCurrentSpaceOffset + uSize ) % m_uSize;
					uCurrentSpaceSize -= uSize;
				}
			}
			else
			{
				::memcpy( m_lpBuffer + uCurrentSpaceOffset, lpInBuffer, uSize );
				uCurrentSpaceOffset += uSize;
				uCurrentSpaceSize -= uSize;
			}
			if ( 0 == uCurrentSpaceSize )
			{
				if ( 0 == uCurrentSpaceOffset )
				{
					uCurrentSpaceSize = CallPutFunc( m_lpBuffer, m_uSize , m_pPutParam );
				}
				else
				{
					uCurrentSpaceSize = CallPutFunc( m_lpBuffer + uCurrentSpaceOffset, m_uSize - uCurrentSpaceOffset , m_pPutParam );
					if ( uCurrentSpaceSize != m_uSize - uCurrentSpaceOffset )
					{
						m_uCurrentDataSize = m_uSize - uCurrentSpaceSize;
						m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
						return uBytesPut;
					}
					uCurrentSpaceSize += CallPutFunc( m_lpBuffer, uCurrentSpaceOffset , m_pPutParam );
				}
				if ( uCurrentSpaceSize != m_uSize )
				{
					m_uCurrentDataSize = m_uSize - uCurrentSpaceSize;
					m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
					return uBytesPut;
				}
				uCurrentSpaceOffset = 0;
			}
			m_uCurrentDataSize = m_uSize - uCurrentSpaceSize;
			m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
			return uSize;
		}
	}

	VOID IOBuffer::Flush()
	{
		if ( NULL != m_pPutCallBack )
		{
			if ( m_uCurrentDataSize > ( m_uSize - m_uCurrentDataOffset ) )
			{
				UINT32 uBytesPut = CallPutFunc( m_lpBuffer + m_uCurrentDataOffset, m_uSize - m_uCurrentDataOffset, m_pPutParam );
				m_uCurrentDataSize -= uBytesPut;
				m_uCurrentDataOffset = ( m_uCurrentDataOffset + uBytesPut ) % m_uSize;
				if ( 0 != m_uCurrentDataOffset )
				{
					return;
				}
				uBytesPut = CallPutFunc( m_lpBuffer, m_uCurrentDataSize, m_pPutParam );
				m_uCurrentDataSize -= uBytesPut;
				m_uCurrentDataOffset += uBytesPut;
			}
			else
			{
				UINT32 uBytesPut = CallPutFunc( m_lpBuffer + m_uCurrentDataOffset, m_uCurrentDataSize, m_pPutParam );
				m_uCurrentDataSize -= uBytesPut;
				m_uCurrentDataOffset += uBytesPut;
			}
			if ( 0 == m_uCurrentDataSize )
			{
				m_uCurrentDataOffset = 0;
			}
		}
	}

	UINT32 IOBuffer::SkipGet( UINT32 uSize )
	{
		if ( 0 == uSize )
		{
			return 0;
		}
		UINT32 uBytesGot = 0;
		if ( m_uCurrentDataSize < uSize )
		{
			if ( m_uCurrentDataSize > ( m_uSize - m_uCurrentDataOffset ) )
			{
				uBytesGot = m_uSize - m_uCurrentDataOffset;
			}
			uBytesGot = m_uCurrentDataSize;
			m_uCurrentDataOffset = 0;
			if ( NULL != m_pLoadCallBack )
			{
				m_uCurrentDataSize = CallLoadFunc( m_lpBuffer, m_uSize, m_pLoadParam );
				if ( 0 == m_uCurrentDataSize )
				{
					return uBytesGot;
				}
			}
			else
			{
				m_uCurrentDataSize = 0;
				return uBytesGot;
			}
			while ( m_uCurrentDataSize < ( uSize - uBytesGot ) )
			{
				uBytesGot += m_uCurrentDataSize;
				m_uCurrentDataSize = CallLoadFunc( m_lpBuffer, m_uSize, m_pLoadParam );
				if ( 0 == m_uCurrentDataSize )
				{
					return uBytesGot;
				}
			}
			m_uCurrentDataOffset += uSize - uBytesGot;
			m_uCurrentDataSize -= uSize - uBytesGot;
			if ( 0 == m_uCurrentDataSize )
			{
				m_uCurrentDataOffset = 0;
			}
			return uSize;
		}
		else
		{
			if ( m_uCurrentDataSize > ( m_uSize - m_uCurrentDataOffset ) )
			{
				if ( ( m_uSize - m_uCurrentDataOffset ) < uSize )
				{
					uBytesGot = m_uSize - m_uCurrentDataOffset;
					m_uCurrentDataOffset = uSize - uBytesGot;
					m_uCurrentDataSize -= uSize;
				}
				else
				{
					m_uCurrentDataOffset = ( m_uCurrentDataOffset + uSize ) % m_uSize;
					m_uCurrentDataSize -= uSize;
				}
			}
			else
			{
				m_uCurrentDataOffset = ( m_uCurrentDataOffset + uSize ) % m_uSize;
				m_uCurrentDataSize -= uSize;
			}
			if ( 0 == m_uCurrentDataSize )
			{
				m_uCurrentDataOffset = 0;
			}
			return uSize;
		}
	}

	UINT32 IOBuffer::SkipPut( UINT32 uSize )
	{
		if ( 0 == uSize )
		{
			return 0;
		}
		UINT32 uBytesPut = 0;
		UINT32 uCurrentSpaceSize = m_uSize - m_uCurrentDataSize;
		UINT32 uCurrentSpaceOffset = ( m_uCurrentDataOffset + m_uCurrentDataSize ) % m_uSize;
		if ( uCurrentSpaceSize < uSize )
		{
			if ( uCurrentSpaceSize > ( m_uSize - uCurrentSpaceOffset ) )
			{
				uBytesPut = m_uSize - uCurrentSpaceOffset;
			}
			uBytesPut = uCurrentSpaceSize;
			uCurrentSpaceOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
			uCurrentSpaceSize = 0;
			if ( NULL != m_pPutCallBack )
			{
				if ( 0 == uCurrentSpaceOffset )
				{
					uCurrentSpaceSize = CallPutFunc( m_lpBuffer, m_uSize , m_pPutParam );
				}
				else
				{
					uCurrentSpaceSize = CallPutFunc( m_lpBuffer + uCurrentSpaceOffset, m_uSize - uCurrentSpaceOffset , m_pPutParam );
					if ( uCurrentSpaceSize != m_uSize - uCurrentSpaceOffset )
					{
						m_uCurrentDataSize = m_uSize - uCurrentSpaceSize;
						m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
						return uBytesPut;
					}
					uCurrentSpaceSize += CallPutFunc( m_lpBuffer, uCurrentSpaceOffset , m_pPutParam );
				}
				if ( uCurrentSpaceSize != m_uSize )
				{
					m_uCurrentDataSize = m_uSize - uCurrentSpaceSize;
					m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
					return uBytesPut;
				}
				uCurrentSpaceOffset = 0;
			}
			else
			{
				m_uCurrentDataSize = m_uSize;
				m_uCurrentDataOffset = uCurrentSpaceOffset;
				return uBytesPut;
			}
			while ( uCurrentSpaceSize < ( uSize - uBytesPut ) )
			{
				uBytesPut += uCurrentSpaceSize;
				uCurrentSpaceSize = CallPutFunc( m_lpBuffer, m_uSize, m_pPutParam );
				if ( m_uSize != uCurrentSpaceSize )
				{
					m_uCurrentDataSize = m_uSize;
					m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
					return uBytesPut;
				}
			}
			uCurrentSpaceOffset = ( uSize - uBytesPut ) % m_uSize;
			uCurrentSpaceSize -= uSize - uBytesPut;
			if ( 0 == uCurrentSpaceSize )
			{
				uCurrentSpaceSize = CallPutFunc( m_lpBuffer, m_uSize, m_pPutParam );
				uCurrentSpaceSize = m_uSize;
			}
			m_uCurrentDataSize = m_uSize - uCurrentSpaceSize;
			m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
			return uSize;
		}
		else
		{
			if ( uCurrentSpaceSize > ( m_uSize - uCurrentSpaceOffset ) )
			{
				if ( ( m_uSize - uCurrentSpaceOffset ) < uSize )
				{
					uBytesPut = m_uSize - uCurrentSpaceOffset;
					uCurrentSpaceOffset = m_uSize - uBytesPut;
					uCurrentSpaceSize -= uSize;
				}
				else
				{
					uCurrentSpaceOffset = ( uCurrentSpaceOffset + uSize ) % m_uSize;
					uCurrentSpaceSize -= uSize;
				}
			}
			else
			{
				uCurrentSpaceOffset += uSize;
				uCurrentSpaceSize -= uSize;
			}
			if ( 0 == uCurrentSpaceSize )
			{
				if ( NULL != m_pPutCallBack )
				{
					if ( 0 == uCurrentSpaceOffset )
					{
						uCurrentSpaceSize = CallPutFunc( m_lpBuffer, m_uSize , m_pPutParam );
					}
					else
					{
						uCurrentSpaceSize = CallPutFunc( m_lpBuffer + uCurrentSpaceOffset, m_uSize - uCurrentSpaceOffset , m_pPutParam );
						if ( uCurrentSpaceSize != m_uSize - uCurrentSpaceOffset )
						{
							m_uCurrentDataSize = m_uSize - uCurrentSpaceSize;
							m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
							return uBytesPut;
						}
						uCurrentSpaceSize += CallPutFunc( m_lpBuffer, uCurrentSpaceOffset , m_pPutParam );
					}
					if ( uCurrentSpaceSize != m_uSize )
					{
						m_uCurrentDataSize = m_uSize - uCurrentSpaceSize;
						m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
						return uBytesPut;
					}
					uCurrentSpaceOffset = 0;
				}
			}
			m_uCurrentDataSize = m_uSize - uCurrentSpaceSize;
			m_uCurrentDataOffset = ( uCurrentSpaceOffset + uCurrentSpaceSize ) % m_uSize;
			return uSize;
		}
	}


	VOID IOBuffer::Clear()
	{
		m_uCurrentDataSize = 0;
		m_uCurrentDataOffset = 0;
	}

	UINT32 IOBuffer::CallLoadFunc( VOID* lpBuffer, UINT32 uSize, VOID* pParam )
	{
		if ( NULL == m_pLoadCallBack )
		{
			return 0;
		}
		UINT32 uSizeGot = m_pLoadCallBack( lpBuffer, uSize, pParam );
		if ( UseCheckSum() )
		{
			CheckSum( ( const BYTE* )lpBuffer, uSizeGot );
		}
		return uSizeGot;
	}

	UINT32 IOBuffer::CallPutFunc( const VOID* lpBuffer, UINT32 uSize, VOID* pParam )
	{
		if ( NULL == m_pPutCallBack )
		{
			return 0;
		}
		if ( UseCheckSum() )
		{
			CheckSum( ( const BYTE* )lpBuffer, uSize );
		}
		return m_pPutCallBack( lpBuffer, uSize, pParam );
	}

	VOID IOBuffer::CheckSum( const BYTE* lpBuffer, UINT32 uSize )
	{
		while ( uSize > 0 )
		{
			m_ulCheckSum += *lpBuffer;
			--uSize;
			++lpBuffer;
		}
	}

	CompressedBuffer::CompressedBuffer( VOID* lpBuffer, UINT32 uSize )
		: IOBuffer( lpBuffer, uSize )
		, m_pCompressor( NULL )
	{
	}

	CompressedBuffer::~CompressedBuffer()
	{
	}

	UINT32 CompressedBuffer::Get( VOID* lpBuffer, UINT32 uSize )
	{
		if ( NULL != m_pCompressor )
		{
			if ( 0 == uSize )
			{
				return 0;
			}
			UINT32 uTotalGot = 0;
			UINT32 uGot = 0;
			while ( uTotalGot < uSize )
			{
				uGot = m_pCompressor->Get( lpBuffer, uSize - uTotalGot, NULL, GetDataCallBack, this );
				if ( 0 == uGot )
				{
					return uTotalGot;
				}
				uTotalGot += uGot;
			}
			return uTotalGot;
		}
		else
		{
			return IOBuffer::Get( lpBuffer, uSize );
		}
	}

	UINT32 CompressedBuffer::Put( const VOID* lpBuffer, UINT32 uSize )
	{
		if ( NULL != m_pCompressor )
		{
			if ( 0 == uSize )
			{
				return 0;
			}
			UINT32 uTotalPut = 0;
			UINT32 uPut = 0;
			while ( uTotalPut < uSize )
			{
				uPut = m_pCompressor->Put( lpBuffer, uSize - uTotalPut, NULL, PutDataCallBack, this );
				if ( 0 == uPut )
				{
					return uTotalPut;
				}
				uTotalPut += uPut;
			}
			return uTotalPut;
		}
		else
		{
			return IOBuffer::Put( lpBuffer, uSize );
		}
	}

	UINT32 CompressedBuffer::Get( VOID* lpBuffer, UINT32 uSize, UINT32* pSizeCompressed )
	{
		if ( NULL != m_pCompressor )
		{
			if ( 0 == uSize )
			{
				return 0;
			}
			UINT32 uTotalGot = 0;
			UINT32 uGot = 0;
			UINT32 uCompressed = 0;
			UINT32 uTotalCompressed = 0;
			while ( uTotalGot < uSize )
			{
				uGot = m_pCompressor->Get( lpBuffer, uSize - uTotalGot, &uCompressed, GetDataCallBack, this );
				uTotalCompressed += uCompressed;
				if ( 0 == uGot )
				{
					if ( NULL != pSizeCompressed )
					{
						*pSizeCompressed = uTotalCompressed;
					}
					return uTotalGot;
				}
				uTotalGot += uGot;
			}
			if ( NULL != pSizeCompressed )
			{
				*pSizeCompressed = uTotalCompressed;
			}
			return uTotalGot;
		}
		else
		{
			UINT32 uTotalCompressed = IOBuffer::Get( lpBuffer, uSize );
			if ( NULL != pSizeCompressed )
			{
				*pSizeCompressed = uTotalCompressed;
			}
			return uTotalCompressed;
		}
	}

	UINT32 CompressedBuffer::Put( const VOID* lpBuffer, UINT32 uSize, UINT32* pSizeCompressed )
	{
		if ( NULL != m_pCompressor )
		{
			if ( 0 == uSize )
			{
				return 0;
			}
			UINT32 uTotalPut = 0;
			UINT32 uPut = 0;
			UINT32 uCompressed = 0;
			UINT32 uTotalCompressed = 0;
			while ( uTotalPut < uSize )
			{
				uPut = m_pCompressor->Put( lpBuffer, uSize - uTotalPut, &uCompressed, PutDataCallBack, this );
				uTotalCompressed += uCompressed;
				if ( 0 == uPut )
				{
					if ( NULL != pSizeCompressed )
					{
						*pSizeCompressed = uTotalCompressed;
					}
					return uTotalPut;
				}
				uTotalPut += uPut;
			}
			if ( NULL != pSizeCompressed )
			{
				*pSizeCompressed = uTotalCompressed;
			}
			return uTotalPut;
		}
		else
		{
			UINT32 uTotalCompressed = IOBuffer::Put( lpBuffer, uSize );
			if ( NULL != pSizeCompressed )
			{
				*pSizeCompressed = uTotalCompressed;
			}
			return uTotalCompressed;
		}
	}

	UINT32 CompressedBuffer::PutDataCallBack( const VOID* lpBuffer, UINT32 uSize, VOID* lpParam )
	{
		return ( ( CompressedBuffer* )lpParam )->RawPut( lpBuffer, uSize );
	}

	UINT32 CompressedBuffer::GetDataCallBack( VOID* lpBuffer, UINT32 uSize, VOID* lpParam )
	{
		return ( ( CompressedBuffer* )lpParam )->RawGet( lpBuffer, uSize );
	}
}

