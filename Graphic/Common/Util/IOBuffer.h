#if !defined ( _IO_BUFFER_H_ )
#define _IO_BUFFER_H_

#include "Compressing.h"

namespace Util
{
	class IOBuffer
	{
	public:
		typedef UINT32 ( *LOAD_BUFFER_FUNC )( VOID* lpBuffer, UINT32 uSize, VOID* pParam );
		typedef UINT32 ( *PUT_BUFFER_FUNC )( const VOID* lpBuffer, UINT32 uSize, VOID* pParam );

		IOBuffer( VOID* lpBuffer, UINT32 uSize, UINT32 uDataSize = 0 );
		virtual ~IOBuffer();

		VOID SetLoadProc( LOAD_BUFFER_FUNC lpProc, VOID* pParam ) { m_pLoadCallBack = lpProc; m_pLoadParam = pParam; }
		VOID SetPutProc( PUT_BUFFER_FUNC lpProc, VOID* pParam ) { m_pPutCallBack = lpProc; m_pPutParam = pParam; }

		virtual UINT32 Get( VOID* lpBuffer, UINT32 uSize );
		virtual UINT32 Put( const VOID* lpBuffer, UINT32 uSize );
		virtual UINT32 SkipGet( UINT32 uSize );
		virtual UINT32 SkipPut( UINT32 uSize );
		virtual VOID Flush();
		virtual VOID Clear();

        inline VOID* GetPointer() { return m_lpBuffer; }
		inline UINT64 GetCheckSum() { return m_ulCheckSum; }
		inline VOID ResetCheckSum() { m_ulCheckSum = 0; }

		inline BOOL UseCheckSum() const				{ return GetFlag( EFlag_UseCheckSum ); }

		inline BOOL GetFlag( DWORD dwFlag ) const	{ return m_dwFlags & dwFlag; }
		inline VOID SetFlag( DWORD dwFlag )			{ m_dwFlags |= dwFlag; }
		inline VOID ClearFlag( DWORD dwFlag )		{ m_dwFlags &= ~dwFlag; }

		enum EFlags
		{
			EFlag_UseCheckSum = 0x1
		};
		
	protected:
		LOAD_BUFFER_FUNC m_pLoadCallBack;
		VOID* m_pLoadParam;
		PUT_BUFFER_FUNC m_pPutCallBack;
		VOID* m_pPutParam;
		BYTE* m_lpBuffer;
		UINT32 m_uSize;
		UINT32 m_uCurrentDataOffset;
		UINT32 m_uCurrentDataSize;

		virtual UINT32 CallLoadFunc( VOID* lpBuffer, UINT32 uSize, VOID* pParam );
		virtual UINT32 CallPutFunc( const VOID* lpBuffer, UINT32 uSize, VOID* pParam );
		virtual VOID CheckSum( const BYTE* lpBuffer, UINT32 uSize );

	private:
		DWORD m_dwFlags;
		UINT64 m_ulCheckSum;
	};

	class CompressedBuffer
		: public IOBuffer
	{
	public:
		CompressedBuffer( VOID* lpBuffer, UINT32 uSize );
		virtual ~CompressedBuffer();

	public:
		inline VOID SetCompressor( Compressor* pCompressor ) { m_pCompressor = pCompressor; }
		inline Compressor* GetCompressor( VOID ) const { return m_pCompressor; }

		virtual UINT32 Get( VOID* lpBuffer, UINT32 uSize );
		virtual UINT32 Put( const VOID* lpBuffer, UINT32 uSize );
		UINT32 Get( VOID* lpBuffer, UINT32 uSize, UINT32* pSizeCompressed );
		UINT32 Put( const VOID* lpBuffer, UINT32 uSize, UINT32* pSizeCompressed );

		UINT32 RawGet( VOID* lpBuffer, UINT32 uSize ) { return IOBuffer::Get( lpBuffer, uSize ); }
		UINT32 RawPut( const VOID* lpBuffer, UINT32 uSize ) { return IOBuffer::Put( lpBuffer, uSize ); }

	protected:
		Compressor* m_pCompressor;

	private:
		static UINT32 PutDataCallBack( const VOID* lpBuffer, UINT32 uSize, VOID* pParam );
		static UINT32 GetDataCallBack( VOID* lpBuffer, UINT32 uSize, VOID* pParam );
	};
}

#endif

