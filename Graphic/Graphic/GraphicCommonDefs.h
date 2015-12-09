#if !defined ( _GRAPHIC_COMMON_DEFS_H_ )
#define _GRAPHIC_COMMON_DEFS_H_

#include <math.h>

#if !defined ASSERT
#define ASSERT(x)
#endif

#define PRECISION 0.0001f
#define LARGER_THAN0( a ) ( a > PRECISION )											// a > 0
#define LESS_THAN0( a ) ( a < -PRECISION )											// a < 0
#define EQUALS0( a ) ( !LARGER_THAN0(a) && !LESS_THAN0(a) )							// a == 0
#define LARGER_THAN( a, b ) LARGER_THAN0( a - b )									// a > b
#define LESS_THAN( a, b ) LESS_THAN0( a - b )										// a < b
#define EQUALS( a, b ) EQUALS0( a - b )												// a == b
#define BETWEEN( x, a, b ) ( LESS_THAN( x, b ) && LARGER_THAN( x, a ) )				// x in ( a, b )
#define BETWEENE( x, a, b ) ( !LESS_THAN( x, a ) && !LARGER_THAN( x, b ) )          // x in [ a, b ]

#endif
