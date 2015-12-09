#include "PNGImageLoader.h"

#include "SystemAPI/SystemFileAPI.h"

#include "png.h"
#include "pngpriv.h"
#include "pngconf.h"

#define IO_BUFFER_SIZE  1024

namespace Graphic
{
    static png_bytep s_ioBuffer[IO_BUFFER_SIZE];

    static void PNGAPI png_read_data_callback( png_structp png_ptr, png_bytep data, png_size_t length )
    {
        Util::IOBuffer* buffer = (Util::IOBuffer*)png_ptr->io_ptr;
        buffer->Get( data, length );
    }

    static UINT32 ReadFileCallback( VOID* lpBuffer, UINT32 uSize, VOID* pParam )
    {
        return SystemAPI::SystemFileAPI::FRead( (FILE*)pParam, lpBuffer, uSize );
    }

    BOOL PNGImageLoader::LoadImage( const char* fullPath, VOID* &data, UINT32 &width, UINT32 &height )
    {
        FILE* pFile = SystemAPI::SystemFileAPI::FOpen( fullPath, "rb" );
        Util::IOBuffer buffer( s_ioBuffer, IO_BUFFER_SIZE );
        buffer.SetLoadProc( ReadFileCallback, pFile );
        BOOL ret = LoadImage( &buffer, data, width, height );
        SystemAPI::SystemFileAPI::FClose( pFile );
        return ret;
    }

    BOOL PNGImageLoader::LoadImage( Util::IOBuffer* buffer, VOID* &data, UINT32 &width, UINT32 &height )
    {
        // Check for the 8-byte signature
        unsigned char sig[8] = {0};
        if ( buffer->Get( sig, 8 ) != 8 || 0 != png_sig_cmp( sig, 0, 8 ) )
        {
            return FALSE;
        }

        png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
        if ( NULL == png_ptr )
        {
            return FALSE;
        }

        png_infop info_ptr = png_create_info_struct( png_ptr );
        if ( NULL == info_ptr )
        {
            png_destroy_read_struct( &png_ptr, (png_infopp)NULL, (png_infopp)NULL );
            return FALSE;
        }

        if ( setjmp( png_jmpbuf( png_ptr ) ) )
        {
            png_destroy_read_struct( &png_ptr, &info_ptr, NULL );
            return FALSE;
        }
        png_set_read_fn( png_ptr, buffer, png_read_data_callback );
        png_set_sig_bytes( png_ptr, 8 );
        png_read_info( png_ptr, info_ptr );

        width = 0;
        height = 0;
        int bit_depth = 0;
        int color_type = 0;
        png_get_IHDR( png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL );
        // Set up some transforms.
        if ( color_type & PNG_COLOR_MASK_ALPHA )
        {
            //png_set_strip_alpha(png_ptr);
        }
        if ( bit_depth > 8 )
        {
            png_set_strip_16( png_ptr );
        }
        if ( color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
        {
            png_set_gray_to_rgb( png_ptr );
        }
        if ( color_type == PNG_COLOR_TYPE_PALETTE )
        {
            png_set_palette_to_rgb( png_ptr );
        }

        // Update the png info struct.
        png_read_update_info( png_ptr, info_ptr );
        // Rowsize in bytes.
        png_uint_32 rowbytes = (png_uint_32)png_get_rowbytes( png_ptr, info_ptr );
        bit_depth = (int)( rowbytes / width );
        if ( bit_depth > 4 || bit_depth < 3 )
        {
            // unsupported
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return FALSE;
        }

        // Allocate the image_data buffer.
        png_bytep image_data = (png_bytep)malloc( width * height * 4 );
        if ( NULL == image_data )
        {
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return FALSE;
        }

        // read data
        png_bytep row_data = (png_bytep)malloc( rowbytes );
        for ( png_uint_32 i = 0; i < height; ++i )
        {
            png_bytep current_row = image_data + i * width * 4;
            png_read_row( png_ptr, row_data, NULL );
            for ( png_uint_32 j = 0; j < width; ++j )
            {
                int current_byte = 0;
                for ( current_byte = 0; current_byte < bit_depth; ++current_byte )
                {
                    *(current_row + j * 4 + current_byte) = *(row_data + j * bit_depth + current_byte);
                }
                while ( current_byte < 4 )
                {
                    *(current_row + j * 4 + current_byte) = 0xFF;
                    ++ current_byte;
                }
            }
        }
        free( row_data );
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

        data = image_data;
        return TRUE;
    }
}
