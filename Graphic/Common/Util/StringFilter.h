#if !defined ( _STRING_FILTER_H_ )
#define _STRING_FILTER_H_

#include <set>
#include <string>
#include <list>

namespace Util
{
	class StringFilter
	{
	public:
		StringFilter();
		~StringFilter();

		void AddPattern( const CHAR* pattern );
		BOOL Match( const CHAR* str );
		void ClearPattern();

		inline BOOL IsExclude() const				{ return GetFlag( FLAG_EXCLUDE ); }
		inline VOID SetExclude( BOOL bExclude )		{ bExclude ? SetFlag( FLAG_EXCLUDE ) : ClearFlag( FLAG_EXCLUDE ); }
		inline BOOL GetFlag( DWORD dwFlag ) const	{ return m_dwFlags & dwFlag; }
		inline VOID SetFlag( DWORD dwFlag )			{ m_dwFlags |= dwFlag; }
		inline VOID ClearFlag( DWORD dwFlag )		{ m_dwFlags &= ~dwFlag; }

		enum Flags
		{
			FLAG_EXCLUDE = 0x1
		};

	private:
		std::set<std::string> m_setSpecificPatterns;

		BOOL PatternMatch( const CHAR* str );
		BOOL IsSpecific( const CHAR* pattern );

		DWORD m_dwFlags;
	};
}

#endif

