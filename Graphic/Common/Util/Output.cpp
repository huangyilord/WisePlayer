#include "Output.h"

#include <stdio.h>

namespace Util
{
	StdOutputChannel Output::s_StdOutChannel;
	StdOutputChannel Output::s_StdErrChannel;
	Output Output::s_Instance;

	Output::Output()
	{
	}

	Output::~Output()
	{
	}

    BOOL Output::Initialize()
    {
        if ( !s_Instance.m_mapChannel.Insert( EChannel_StdOut, &s_StdOutChannel ) )
        {
            Finalize();
            return FALSE;
        }
        if ( !s_Instance.m_mapChannel.Insert( EChannel_StdErr, &s_StdErrChannel ) )
        {
            Finalize();
            return FALSE;
        }
        return TRUE;
    }

    VOID Output::Finalize()
    {
        s_Instance.m_mapChannel.Clear();
    }

	VOID Output::Write( DWORD dwID, const CHAR* pszFormat, ... )
	{
		OutputChannel* pChannel = NULL;
        if ( !s_Instance.m_mapChannel.Get( dwID, pChannel ) )
		{
			return;
		}
		va_list args;
		va_start( args, pszFormat );
		pChannel->Write( pszFormat, args );
		va_end( args );
	}

	BOOL Output::SetChannel( DWORD dwID, OutputChannel* pChannel )
	{
        s_Instance.m_mapChannel.Remove( dwID );
        return s_Instance.m_mapChannel.Insert( dwID, pChannel );
	}

	OutputChannel* Output::GetChannel( DWORD dwID )
	{
        OutputChannel* pChannel = NULL;
        s_Instance.m_mapChannel.Get( dwID, pChannel );
		return pChannel;
	}

	VOID Output::Flush( DWORD dwID )
	{
		OutputChannel* pChannel = NULL;
		if ( !s_Instance.m_mapChannel.Get( dwID, pChannel ) )
		{
			return;
		}

		pChannel->Flush();
	}

	OutputChannel::OutputChannel()
	{
	}

	OutputChannel::~OutputChannel()
	{
	}


	StdOutputChannel::StdOutputChannel()
        : m_prefix( NULL )
	{
	}

	StdOutputChannel::~StdOutputChannel()
	{
	}

    BOOL StdOutputChannel::SetPrefix( const CHAR* pszPrefix )
    {
        if ( NULL != m_prefix )
        {
            free( m_prefix );
        }
        size_t size = strlen( pszPrefix ) + 1;
        m_prefix = (CHAR*)malloc( size );
        if ( NULL == m_prefix )
        {
            return FALSE;
        }
        memcpy( m_prefix, pszPrefix, size );
        return TRUE;
    }

	VOID StdOutputChannel::Write( const CHAR* pszFormat, va_list args )
	{
		if ( NULL != m_prefix )
		{
			printf( "%s", m_prefix );
		}
		vprintf( pszFormat, args );
	}

	VOID StdOutputChannel::Flush()
	{
		fflush(stdout);
	}

}
