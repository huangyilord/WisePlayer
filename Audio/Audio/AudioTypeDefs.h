//
//  AudioTypeDefs.h
//  Audio
//
//  Created by Yi Huang on 2/7/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _AUDIO_TYPE_DEFS_H_ )
#define _AUDIO_TYPE_DEFS_H_

namespace Audio
{

#if !defined (VOID)
#define VOID        void
#endif

#if !defined (INT32)
#define INT32       int
#endif

#if !defined (UINT32)
#define UINT32      unsigned
#endif

#if !defined (UINT16)
#define UINT16      unsigned short
#endif

#if !defined (CHAR)
#define CHAR        char
#endif

#if !defined (WCHAR)
#define WCHAR       UINT16
#endif

#if !defined (DWORD)
#define DWORD       UINT32
#endif

#if !defined (WORD)
#define WORD        UINT16
#endif

#if !defined (BYTE)
#define BYTE        unsigned char
#endif

#if !defined (FLOAT32)
#define FLOAT32     float
#endif

#if !defined (BOOL)
#define BOOL        BYTE
#endif

#if !defined (TRUE)
#define TRUE        1
#endif

#if !defined (FALSE)
#define FALSE       0
#endif
    
#if !defined (NULL)
#define NULL        0
#endif

    typedef void* AUDIO_BUFFER_HANDLER;
    typedef void* AUDIO_SOURCE_HANDLER;
}

#endif
