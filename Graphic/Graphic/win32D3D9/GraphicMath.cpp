#include "GraphicTypeDef.h"
#include <d3dx9math.h>

namespace Graphic
{
    void MatrixIdentity( Matrix16* matrix )
    {
        D3DXMatrixIdentity( matrix );
    }

    void MatrixTranslate( Matrix16* matrix, FLOAT32 x, FLOAT32 y, FLOAT32 z )
    {
        D3DXMatrixTranslation( matrix, x, y, z );
    }

    void MatrixScale( Matrix16* matrix, FLOAT32 sx, FLOAT32 sy, FLOAT32 sz )
    {
        D3DXMatrixScaling( matrix, sx, sy, sz );
    }

    void MatrixRotate( Matrix16* matrix, const Vector3* axis, FLOAT32 angle )
    {
        D3DXMatrixRotationAxis( matrix, axis, angle );
    }

    void MatrixRotateX( Matrix16* matrix, FLOAT32 angle)
    {
        D3DXMatrixRotationX( matrix, angle );
    }

    void MatrixRotateY( Matrix16* matrix, FLOAT32 angle)
    {
        D3DXMatrixRotationY( matrix, angle );
    }

    void MatrixRotateZ( Matrix16* matrix, FLOAT32 angle)
    {
        D3DXMatrixRotationZ( matrix, angle );
    }

    void Point3Transform( Vector3* result, const Vector3* vector, const Matrix16* matrix )
    {
        D3DXVec3TransformCoord( result, vector, matrix );
    }

    void Vector3Transform( Vector3* result, const Vector3* vector, const Matrix16* matrix )
    {
        D3DXVec3TransformNormal( result, vector, matrix );
    }

    void MatrixLookAt( Matrix16* result, const Vector3* from, const Vector3* to, const Vector3* up )
    {
        D3DXMatrixLookAtRH( result, from, to, up );
    }

    void MatrixPerspective( Matrix16* result, FLOAT32 fovy, FLOAT32 aspect, FLOAT32 n, FLOAT32 f )
    {
        D3DXMatrixPerspectiveFovRH( result, fovy, aspect, n, f );
    }

    void MatrixOrtho( Matrix16* result, FLOAT32 w, FLOAT32 h, FLOAT32 n, FLOAT32 f )
    {
        D3DXMatrixOrthoRH( result, w, h, n, f );
    }

    void MatrixInverse( Matrix16* result, const Matrix16* matrix )
    {
        D3DXMatrixInverse( result, NULL, matrix );
    }

    void MatrixMultiply( Matrix16* result, const Matrix16* matrix1, const Matrix16* matrix2 )
    {
        D3DXMatrixMultiply( result, matrix1, matrix2 );
    }

    void Vector3Add( Vector3* result, const Vector3* vec1, const Vector3* vec2 )
    {
        D3DXVec3Add( result, vec1, vec2 );
    }

    void Vector3Subtract( Vector3* result, const Vector3* vec1, const Vector3* vec2 )
    {
        D3DXVec3Subtract( result, vec1, vec2 );
    }

    void Vector3Cross( Vector3* result, const Vector3* vec1, const Vector3* vec2 )
    {
        D3DXVec3Cross( result, vec1, vec2 );
    }

    FLOAT Vector3Dot( const Vector3* vec1, const Vector3* vec2 )
    {
        return D3DXVec3Dot( vec1, vec2 );
    }

    FLOAT Vector3Magnitude( const Vector3* vec )
    {
        return D3DXVec3Length( vec );
    }

    void Vector3Normalize( Vector3* vec )
    {
        FLOAT scale = Vector3Magnitude( vec );
        if ( scale > 0.f )
        {
            vec->x /= scale;
            vec->y /= scale;
            vec->z /= scale;
        }
    }

    void MatrixInterpolate( Matrix16* result, const Matrix16* matrix1, const Matrix16* matrix2, FLOAT32 f )
    {
        for ( UINT32 i = 0; i < 4; ++i )
        {
            for ( UINT32 j = 0; j < 4; ++j )
            {
                (*result)(i, j) = (*matrix1)(i, j) * ( 1.f - f ) + (*matrix2)(i, j) * f;
            }
        }
    }

    void VectorInterpolate( Vector3* result, const Vector3* vec1, const Vector3* vec2, FLOAT32 f )
    {
        result->x = vec1->x * ( 1.f - f ) + vec2->x * f;
        result->y = vec1->y * ( 1.f - f ) + vec2->y * f;
        result->z = vec1->z * ( 1.f - f ) + vec2->z * f;
    }

    void VectorInterpolate( Vector4* result, const Vector4* vec1, const Vector4* vec2, FLOAT32 f )
    {
        result->x = vec1->x * ( 1.f - f ) + vec2->x * f;
        result->y = vec1->y * ( 1.f - f ) + vec2->y * f;
        result->z = vec1->z * ( 1.f - f ) + vec2->z * f;
        result->w = vec1->w * ( 1.f - f ) + vec2->w * f;
    }
}
