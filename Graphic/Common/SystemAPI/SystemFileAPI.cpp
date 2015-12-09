#include "SystemFileAPI.h"
#include <stdio.h>
namespace SystemAPI
{
#if defined ( LINUX ) || defined ( MAC_OS ) || defined ( ANDROID )
	FILE* SystemFileAPI::FOpen( const CHAR* path, const CHAR* mode )
	{
		return fopen( path, mode );
	}

	INT64 SystemFileAPI::FSeek( FILE* file, INT64 offset, int origin )
	{
		return fseek( file, (INT32)offset, origin );
	}

	UINT32 SystemFileAPI::FRead( FILE* file, VOID* pBuf, UINT32 uLen )
	{
		return (UINT32)fread( pBuf, 1, uLen, file );
	}

	UINT32 SystemFileAPI::FWrite( FILE* file, const VOID* pBuf, UINT32 uLen )
	{
		return (UINT32)fwrite( pBuf, 1, uLen, file );
	}

	INT64 SystemFileAPI::FTell( FILE* file )
	{
		return ftell( file );
	}

	VOID SystemFileAPI::FClose( FILE* file )
	{
		fclose( file );
	}

#elif defined ( WIN32 )
#pragma warning ( disable: 4996 )
	FILE* SystemFileAPI::FOpen( const CHAR* path, const CHAR* mode )
	{
		return fopen( path, mode );
	}

	INT64 SystemFileAPI::FSeek( FILE* file, INT64 offset, int origin )
	{
		return _fseeki64( file, offset, origin );
		
	}
#pragma warning ( default: 4996 )

	UINT32 SystemFileAPI::FRead( FILE* file, VOID* pBuf, UINT uLen )
	{
		return (UINT)fread( pBuf, 1, uLen, file );
	}

	UINT32 SystemFileAPI::FWrite( FILE* file, const VOID* pBuf, UINT uLen )
	{
		return (UINT)fwrite( pBuf, 1, uLen, file );
	}

	INT64 SystemFileAPI::FTell( FILE* file )
	{
		return _ftelli64( file );
	}

	VOID SystemFileAPI::FClose( FILE* file )
	{
		fclose( file );
	}
#endif

#if defined ( LINUX ) || defined ( MAC_OS ) || defined ( ANDROID )
	INT32 SystemFileAPI::RemoveFile( const CHAR* path )
	{
		if ( remove( path ) < 0 )
		{
			return -1;
		}
		return 0;
	}

	BOOL SystemFileAPI::ExistFile( const CHAR* path )
	{
		if ( access( path, R_OK | W_OK ) < 0 )
		{
			return FALSE;
		}
		return TRUE;
	}

	INT32 SystemFileAPI::MakeDir( const CHAR* path )
	{
		if ( mkdir( path, 0777 ) < 0 )
		{
			return -1;
		}
		return 0;
	}

	BOOL SystemFileAPI::IsDir( const CHAR* path )
	{
		struct stat s;
		if ( lstat( path, &s ) < 0 )
		{   
             return FALSE;
        }
		if ( S_ISDIR( s.st_mode ) )
		{
			return TRUE;
		}
		return FALSE;
	}

    VOID SystemFileAPI::AppendPathElement( std::string& path, const CHAR* element )
    {
        path += "/";
        path += element;
    }

	VOID SystemFileAPI::EnumFiles( const CHAR* path, std::list<std::string>& fileList )
	{
		DIR *hpDir;
		struct dirent *currFP = 0;
		hpDir = opendir( path );
		if ( NULL == hpDir )
		{
			printf( "Failed to open dir \'%s\'\n", path );
			return;
		}
		while ( 0 != ( currFP = readdir( hpDir ) ) )
		{
			if ( '.' != currFP->d_name[0] )
			{
				fileList.push_back( currFP->d_name );
			}
		}
	}

	INT64 SystemFileAPI::GetFileLength( const CHAR* path )
	{
		struct stat buf;
		if ( lstat( path, &buf ) < 0 )
		{   
             return 0;
        }
		return buf.st_size;
	}
#elif defined ( WIN32 )
	INT32 SystemFileAPI::RemoveFile( const CHAR* path )
	{
		if ( DeleteFileA( path ) )
		{
			return 0;
		}
		return -1;
	}

	BOOL SystemFileAPI::ExistFile( const CHAR* path )
	{
		return GetFileAttributesA( path ) != -1;
	}

	INT32 SystemFileAPI::MakeDir( const CHAR* path )
	{
		if ( CreateDirectoryA( path, NULL ) )
		{
			return 0;
		}
		return -1;
	}

	BOOL SystemFileAPI::IsDir( const CHAR* path )
	{
		return GetFileAttributesA( path ) & FILE_ATTRIBUTE_DIRECTORY;
	}

    VOID SystemFileAPI::AppendPathElement( std::string& path, const CHAR* element )
    {
        path += "\\";
        path += element;
    }

	VOID SystemFileAPI::EnumFiles( const CHAR* path, std::list<std::string>& fileList )
	{
		WIN32_FIND_DATAA findFileData;
		HANDLE hFind;
		std::string fullPath = path;
		std::string findStr = fullPath + "\\*.*";
		hFind = FindFirstFileA( findStr.c_str(), &findFileData );
		if ( INVALID_HANDLE_VALUE != hFind )
		{
			do
			{
				if ( '.' != findFileData.cFileName[0] )
				{
					fileList.push_back( findFileData.cFileName );
				}
			} while ( FindNextFileA( hFind, &findFileData ) );
		}
	}

	INT64 SystemFileAPI::GetFileLength( const CHAR* path )
	{
		int iresult;
		struct _stat64 buf;
		iresult = _stat64( path, &buf );
		if( iresult == 0 )
		{
			return buf.st_size;
		}
		return 0;
	}
#endif
}

