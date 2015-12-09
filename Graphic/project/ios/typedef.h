//
// Prefix header for all source files of the 'ShootClub' target in the 'ShootClub' project
//

#import <Availability.h>

#ifndef __IPHONE_5_0
#warning "This project uses features only available in iOS SDK 5.0 and later."
#endif

#ifdef __OBJC__
    #import <UIKit/UIKit.h>
    #import <Foundation/Foundation.h>
#endif

#include "assert.h"

#if !defined (VOID)
#define VOID        void
#endif

#if !defined (UINT64)
#define UINT64      unsigned long long
#endif

#if !defined (UINT32)
#define UINT32      unsigned
#endif

#if !defined (UINT16)
#define UINT16      unsigned short
#endif

#if !defined (UINT8)
#define UINT8       unsigned char
#endif

#if !defined (INT64)
#define INT64       long long
#endif

#if !defined (INT32)
#define INT32       int
#endif

#if !defined (INT16)
#define INT16       short
#endif

#if !defined (CHAR)
#define CHAR        char
#endif

#if !defined (WCHAR)
#define WCHAR       wchar_t
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

#if !defined (ASSERT)
#if defined DEBUG || _DEBUG
#define ASSERT(x)   assert(x)
#else
#define ASSERT(x)
#endif
#endif

