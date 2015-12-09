#if !defined ( _PNG_IMAGE_LOADER_H_ )
#define _PNG_IMAGE_LOADER_H_

#include "IImageLoader.h"

namespace Graphic
{
    class PNGImageLoader
        : public IImageLoader
    {
    public:
        virtual BOOL LoadImage( const char* fullPath, VOID* &data, UINT32 &width, UINT32 &height );
        virtual BOOL LoadImage( Util::IOBuffer* buffer, VOID* &data, UINT32 &width, UINT32 &height );
    };
}

#endif
