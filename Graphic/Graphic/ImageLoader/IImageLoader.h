#if !defined ( _I_IMAGE_LOADER_H_ )
#define _I_IMAGE_LOADER_H_

#include "Util/IOBuffer.h"

namespace Graphic
{
    class IImageLoader
    {
    public:
        virtual BOOL LoadImage( const char* fullPath, VOID* &data, UINT32 &width, UINT32 &height ) = 0;
        virtual BOOL LoadImage( Util::IOBuffer* buffer, VOID* &data, UINT32 &width, UINT32 &height ) = 0;
    };
}

#endif
