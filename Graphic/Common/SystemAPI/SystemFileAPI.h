#if !defined ( _SYSTEM_FILE_API_H_ )
#define _SYSTEM_FILE_API_H_

#if defined ( LINUX ) || defined ( MAC_OS )|| defined ( ANDROID )
#include <unistd.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/stat.h>
#elif defined ( WIN32 )
#include <windows.h>
#include <sys/stat.h>
#endif

#include <list>
#include <string>

namespace SystemAPI
{
	class SystemFileAPI
	{
	public:
		static FILE* FOpen( const CHAR* path, const CHAR* mode );
		static INT64 FSeek( FILE* file, INT64 offset, int origin );
		static UINT32 FRead( FILE* file, VOID* pBuf, UINT32 uLen );
		static UINT32 FWrite( FILE* file, const VOID* pBuf, UINT32 uLen );
		static INT64 FTell( FILE* file );
		static VOID FClose( FILE* file );

		static INT32 RemoveFile( const CHAR* path );
		static BOOL ExistFile( const CHAR* path );
		static INT32 MakeDir( const CHAR* path );
		static BOOL IsDir( const CHAR* path );

        static VOID AppendPathElement( std::string& path, const CHAR* element );
		static VOID EnumFiles( const CHAR* path, std::list<std::string>& fileList );

		static INT64 GetFileLength( const CHAR* path );
	};
}

#endif
