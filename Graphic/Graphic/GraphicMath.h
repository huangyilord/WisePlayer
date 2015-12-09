#if !defined ( _GRAPHIC_MATH_H_ )
#define _GRAPHIC_MATH_H_

#include "GraphicTypeDef.h"
#include "GraphicCommonDefs.h"

#define GRAPHIC_PI      3.141592653f

namespace Graphic
{
    void MatrixIdentity( Matrix16* matrix );

    void MatrixTranslate( Matrix16* matrix, FLOAT32 x, FLOAT32 y, FLOAT32 z);

    void MatrixScale( Matrix16* matrix, FLOAT32 sx, FLOAT32 sy, FLOAT32 sz);

    void MatrixRotate( Matrix16* matrix, const Vector3* axis, FLOAT32 angle );

    void MatrixRotateX( Matrix16* matrix, FLOAT32 angle );
    void MatrixRotateY( Matrix16* matrix, FLOAT32 angle );
    void MatrixRotateZ( Matrix16* matrix, FLOAT32 angle );

    void Point3Transform( Vector3* result, const Vector3* vector, const Matrix16* matrix );
    void Vector3Transform( Vector3* result, const Vector3* vector, const Matrix16* matrix);

    void MatrixLookAt( Matrix16* result, const Vector3* from, const Vector3* to, const Vector3* up );
    void MatrixFrustum( Matrix16* result, FLOAT32 left, FLOAT32 right, FLOAT32 bottom, FLOAT32 top, FLOAT32 near, FLOAT32 far );
    void MatrixPerspective( Matrix16* result, FLOAT32 fovy, FLOAT32 aspect, FLOAT32 near, FLOAT32 far );
    void MatrixOrtho( Matrix16* result, FLOAT32 width, FLOAT32 height, FLOAT32 near, FLOAT32 far );

    void MatrixInverse( Matrix16* result, const Matrix16* matrix );
    void MatrixMultiply( Matrix16* result, const Matrix16* matrix1, const Matrix16* matrix2 );

    void Vector3Add( Vector3* result, const Vector3* vec1, const Vector3* vec2 );
    void Vector3Subtract( Vector3* result, const Vector3* vec1, const Vector3* vec2 );
    void Vector3Cross( Vector3* result, const Vector3* vec1, const Vector3* vec2 );
    FLOAT32 Vector3Dot( const Vector3* vec1, const Vector3* vec2 );
    FLOAT32 Vector3Magnitude( const Vector3* vec );
    void Vector3Normalize( Vector3* vec );

    void MatrixInterpolate( Matrix16* result, const Matrix16* matrix1, const Matrix16* matrix2, FLOAT32 f );
    void VectorInterpolate( Vector3* result, const Vector3* vec1, const Vector3* vec2, FLOAT32 f );
    void VectorInterpolate( Vector4* result, const Vector4* vec1, const Vector4* vec2, FLOAT32 f );

    void MatrixFromQuaternion( Matrix16* result, const Vector4* quaternions );
}

#endif
