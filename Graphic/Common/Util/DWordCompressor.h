#if !defined ( _DWORD_COMPRESSOR_H_ )
#define _DWORD_COMPRESSOR_H_

// just an example

#include "Compressing.h"

namespace Util
{
	class DWordCompressor
		: public Compressor
	{
	private:
		DWordCompressor();
		DWordCompressor( DWordCompressor& );
		~DWordCompressor();

		static DWordCompressor s_Instance;
	public:
		virtual UINT32 Put( const VOID* lpSrcBuffer, UINT32 uSrcSize, UINT32* pSizeCompressed, PUT_DATA_CALLBACK callback, VOID* pParam );
		virtual UINT32 Get( VOID* lpDstBuffer, UINT32 uDstSize, UINT32* pSizeCompressed, GET_DATA_CALLBACK callback, VOID* pParam );

		inline static DWordCompressor& GetInstance() { return s_Instance; }
	private:
		UINT32 PutDWord( BYTE* lpBuffer, DWORD dwValue );
		UINT32 GetDWord( DWORD& dwValue, GET_DATA_CALLBACK callback, VOID* pParam );
	};
}

#endif
