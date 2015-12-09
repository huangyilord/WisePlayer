//
//  common_debug.h
//  common
//
//  Created by Yi Huang on 10/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _COMMON_DEBUG_H_ )
#define _COMMON_DEBUG_H_

#if defined ( DEBUG ) || defined ( _DEBUG )
#include "assert.h"

#if !defined (ASSERT)
#define ASSERT(x) assert(x)
#endif

#if !defined (VERIFY)
#define VERIFY(x) assert(x)
#endif

#if !defined (TRACE)
#define TRACE(...) printf(__VA_ARGS__)
#endif

#else // defined ( DEBUG ) || defined ( _DEBUG )

#if !defined (ASSERT)
#define ASSERT(x)
#endif

#if !defined (VERIFY)
#define VERIFY(x) (void)(x)
#endif

#if !defined (TRACE)
#define TRACE(...)
#endif

#endif // defined ( DEBUG ) || defined ( _DEBUG )

#endif
