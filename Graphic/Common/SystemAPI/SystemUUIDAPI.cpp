#include "SystemUUIDAPI.h"

#include <assert.h>

VOID SystemAPI::SystemUUIDAPI::GenerateUUID( BYTE uuid[16] )
{
#ifdef WIN32
    CoCreateGuid((GUID*)uuid);
#else
    uuid_generate(uuid);
#endif

}