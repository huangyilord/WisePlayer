#if !defined ( _GRAPHIC_UTIL_H_ )
#define _GRAPHIC_UTIL_H_

#include "GraphicTypeDef.h"

namespace Graphic
{
    typedef struct _Box3D
    {
        Vector3 origin;
        Vector3 size;
    } Box3D;

    typedef struct _Box2D
    {
        Vector2 origin;
        Vector2 size;

        BOOL InRect( const Vector2& pt );
        BOOL Intersect( const _Box2D& box2, _Box2D& intersect );

        static BOOL Intersect( const _Box2D& box1, const _Box2D& box, _Box2D& intersect );
    } Box2D;

    typedef struct Frustum
    {
        enum POSITION
        {
            FRONT = 0
            , RIGHT
            , LEFT
            , TOP
            , BOTTOM
            , BACK
        };
        Vector3 vertex[8];
        Vector3 normal[6];

        static BOOL Exclude( const Frustum& frustum1, const Frustum& frustum2 );
    } Frustum;
}

#endif
