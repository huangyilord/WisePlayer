#include "GraphicUtil.h"

#include "GraphicMath.h"

namespace Graphic
{
    BOOL Box2D::Intersect( const _Box2D& box1, const _Box2D& box2, _Box2D& intersect )
    {
        FLOAT32 min1, max1, min2, max2;
        FLOAT32 maxLeft, minTop, minRight, maxBottom;
        if ( box1.size.x > 0.f )
        {
            min1 = box1.origin.x;
            max1 = box1.origin.x + box1.size.x;
        }
        else
        {
            min1 = box1.origin.x + box1.size.x;
            max1 = box1.origin.x;
        }

        if ( box2.size.x > 0.f )
        {
            min2 = box2.origin.x;
            max2 = box2.origin.x + box2.size.x;
        }
        else
        {
            min2 = box2.origin.x + box2.size.x;
            max2 = box2.origin.x;
        }

        maxLeft = min1 > min2 ? min1 : min2;
        minRight = max1 > max2 ? max2 : max1;

        if ( box1.size.y > 0.f )
        {
            min1 = box1.origin.y;
            max1 = box1.origin.y + box1.size.y;
        }
        else
        {
            min1 = box1.origin.y + box1.size.y;
            max1 = box1.origin.y;
        }

        if ( box2.size.y > 0.f )
        {
            min2 = box2.origin.y;
            max2 = box2.origin.y + box2.size.y;
        }
        else
        {
            min2 = box2.origin.y + box2.size.y;
            max2 = box2.origin.y;
        }

        maxBottom = min1 > min2 ? min1 : min2;
        minTop = max1 > max2 ? max2 : max1;

        intersect.origin.x = maxLeft;
        intersect.origin.y = maxBottom;
        intersect.size.x = minRight - maxLeft;
        intersect.size.y = minTop - maxBottom;

        return ( intersect.size.x > 0.f ) && ( intersect.size.y > 0.f );
    }

    BOOL Box2D::InRect( const Vector2& pt )
    {
        return ( ( origin.x - pt.x ) * ( origin.x + size.x - pt.x ) < 0.f ) && ( ( origin.y - pt.y ) * ( origin.y + size.y - pt.y ) < 0.f );
    }

    BOOL Box2D::Intersect( const _Box2D& box, _Box2D& intersect )
    {
        return Intersect( *this, box, intersect );
    }

    BOOL Frustum::Exclude( const Frustum& frustum1, const Frustum& frustum2 )
    {
        /*
            6--------------7
           /|             /|
          / |            / |
         2--------------3  |
         |  |           |  |
         |  |           |  |
         |  |           |  |
         |  4-----------|--5
         | /            | /
         |/             |/
         0--------------1
        */
        for ( UINT32 iface = 0; iface < 6; ++iface )
        {
            UINT32 iFrustumPoint = (iface % 2 ? 7 : 0);
            BOOL bExcluded = TRUE;
            for ( UINT32 i = 0; i < 8; ++i )
            {
                Graphic::Vector3 vec;
                Graphic::Vector3Subtract( &vec, &frustum1.vertex[iFrustumPoint], &frustum2.vertex[i] );
                if ( Graphic::Vector3Dot( &frustum1.normal[iface], &vec ) > 0.f )
                {
                    bExcluded = FALSE;
                    break;
                }
            }
            if ( bExcluded )
            {
                return TRUE;
            }
        }
        return FALSE;
    }
}
