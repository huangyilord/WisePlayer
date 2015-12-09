#if !defined ( _COMPRESSING_H )
#define _COMPRESSING_H

#include <map>

namespace Util
{
	typedef UINT32 ( *PUT_DATA_CALLBACK )( const VOID* lpBuffer, UINT32 uSize, VOID* pParam );
	typedef UINT32 ( *GET_DATA_CALLBACK )( VOID* lpBuffer, UINT32 uSize, VOID* pParam );

	class Compressor
	{
	protected:
		Compressor();
		Compressor( Compressor& );
		virtual ~Compressor();

	public:
		virtual UINT32 Put( const VOID* lpSrcBuffer, UINT32 uSrcSize, UINT32* pSizeCompressed, PUT_DATA_CALLBACK, VOID* pParam ) = 0;
		virtual UINT32 Get( VOID* lpDstBuffer, UINT32 uDstSize, UINT32* pSizeCompressed, GET_DATA_CALLBACK, VOID* pParam ) = 0;
	};

	class CompressorManager
	{
	private:
		CompressorManager();
		~CompressorManager();

		static CompressorManager s_Instance;
		
		Compressor* _GetCompressor( WORD wTypeID );

		std::map<WORD, Compressor*> m_mapCompressors;
	public:
		enum CompressorType
		{
			Compressor_None = 0
			, Compressor_DWord
		};

		static Compressor* GetCompressor( WORD wTypeID ) { return s_Instance._GetCompressor( wTypeID ); }
	};
}

#endif