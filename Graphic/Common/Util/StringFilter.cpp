#include "StringFilter.h"

namespace Util
{
	StringFilter::StringFilter()
		: m_setSpecificPatterns()
	{
	}

	StringFilter::~StringFilter()
	{
		ClearPattern();
	}

	void StringFilter::AddPattern( const CHAR* pattern )
	{
		if ( NULL == pattern )
		{
			return;
		}
		if ( IsSpecific( pattern ) )
		{
			m_setSpecificPatterns.insert( pattern );
		}
		else
		{


		}
	}

	BOOL StringFilter::Match( const CHAR* str )
	{
		if ( m_setSpecificPatterns.end() != m_setSpecificPatterns.find( str ) )
		{
			return !IsExclude();
		}
		return PatternMatch( str ) ^ IsExclude();
	}

	void StringFilter::ClearPattern()
	{
		m_setSpecificPatterns.clear();
	}

	BOOL StringFilter::PatternMatch( const CHAR* )
	{
		return FALSE;
	}

	BOOL StringFilter::IsSpecific( const CHAR* pattern )
	{
		const char* p = pattern;
		char c;
		while ( 0 != ( c = *( p++ ) ) )
		{
			if ( c == '*' || c == '?' )
			{
				return FALSE;
			}
		}
		return TRUE;
	}
}


