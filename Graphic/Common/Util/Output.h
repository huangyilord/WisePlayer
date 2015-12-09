#if !defined ( _OUT_PUT_H_ )
#define _OUT_PUT_H_

#include "HashTable.h"
#if defined LINUX || defined MAC_OS
#include <stdarg.h>
#endif
#if defined ( DEBUG ) || defined ( _DEBUG )
#define HYOUTPUT_TRACE( id, content ) Output::Write( id, content );
#else
#define HYOUTPUT_TRACE( id, content )
#endif

namespace Util
{
	class OutputChannel
	{
	protected:
		OutputChannel();
		virtual ~OutputChannel();

	public:
		virtual VOID Write( const CHAR* pszFormat, va_list args ) = 0;
		virtual VOID Flush() = 0;
	};

	class StdOutputChannel
		: public OutputChannel
	{
	public:
		StdOutputChannel();
		virtual ~StdOutputChannel();

		BOOL SetPrefix( const CHAR* pszPrefix );
		inline const CHAR* GetPrefix() { return m_prefix; }
	public:
		virtual VOID Write( const CHAR* pszFormat, va_list args );
		virtual VOID Flush();

	private:
		CHAR*       m_prefix;
	};

	class Output
	{
	private:
		Output();
		~Output();

		static Output s_Instance;
	public:
		enum EStandardOutputChannel
		{
			EChannel_StdOut = 0
			, EChannel_StdErr

			, EChannel_Num
		};

        static BOOL Initialize();
        static VOID Finalize();

		static VOID Write( DWORD dwID, const CHAR* pszFormat, ... );
		static BOOL SetChannel( DWORD dwID, OutputChannel* pChannel );
		static OutputChannel* GetChannel( DWORD dwID );
		static VOID Flush( DWORD dwID );

        static StdOutputChannel s_StdOutChannel;
		static StdOutputChannel s_StdErrChannel;
	private:
        typedef Util::HashTable<DWORD, OutputChannel*, 128, 32>     ChannelTable;
        ChannelTable                m_mapChannel;
	};
}

#endif
