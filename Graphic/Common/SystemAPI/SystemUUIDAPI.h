#if !defined ( _SYSTEM_UUID_API_H_ )
#define _SYSTEM_UUID_API_H_

#ifdef WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif

namespace SystemAPI
{
    class SystemUUIDAPI
    {
    public:
        static VOID GenerateUUID( BYTE uuid[16] );
    };
}

#endif
