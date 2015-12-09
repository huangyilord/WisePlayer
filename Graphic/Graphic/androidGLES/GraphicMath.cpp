#include "Graphic/GraphicMath.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

namespace Graphic
{
    void MatrixIdentity( Matrix16* matrix )
    {
        memset( matrix, 0, sizeof(Matrix16) );
        matrix->m[0][0] = 1.f;
        matrix->m[1][1] = 1.f;
        matrix->m[2][2] = 1.f;
        matrix->m[3][3] = 1.f;
    }

    void MatrixTranslate( Matrix16* matrix, FLOAT32 x, FLOAT32 y, FLOAT32 z)
    {
        MatrixIdentity( matrix );
        matrix->m[3][0] = x;
        matrix->m[3][1] = y;
        matrix->m[3][2] = z;
    }

    void MatrixScale( Matrix16* matrix, FLOAT32 sx, FLOAT32 sy, FLOAT32 sz)
    {
        memset( matrix, 0, sizeof(Matrix16) );
        matrix->m[0][0] = sx;
        matrix->m[1][1] = sy;
        matrix->m[2][2] = sz;
        matrix->m[3][3] = 1.f;
    }

    void MatrixRotate( Matrix16* matrix, const Vector3* axis, FLOAT32 angle )
    {
        FLOAT32 xx, yy, zz, xy, yz, zx, xs, ys, zs;
        FLOAT32 oneMinusCos;
        FLOAT32 sinAngle, cosAngle;
        FLOAT32 x = axis->x;
        FLOAT32 y = axis->y;
        FLOAT32 z = axis->z;
        FLOAT32 mag = (FLOAT32)sqrtf( x * x + y * y + z * z );

        sinAngle = (FLOAT32)sinf(angle);
        cosAngle = (FLOAT32)cosf(angle);
        if ( mag > 0.0f )
        {
            x /= mag;
            y /= mag;
            z /= mag;
            
            xx = x * x;
            yy = y * y;
            zz = z * z;
            xy = x * y;
            yz = y * z;
            zx = z * x;
            xs = x * sinAngle;
            ys = y * sinAngle;
            zs = z * sinAngle;
            oneMinusCos = 1.f - cosAngle;

            matrix->m[0][0] = (oneMinusCos * xx) + cosAngle;
            matrix->m[0][1] = (oneMinusCos * xy) + zs;
            matrix->m[0][2] = (oneMinusCos * zx) - ys;
            matrix->m[0][3] = 0.f;

            matrix->m[1][0] = (oneMinusCos * xy) - zs;
            matrix->m[1][1] = (oneMinusCos * yy) + cosAngle;
            matrix->m[1][2] = (oneMinusCos * yz) + xs;
            matrix->m[1][3] = 0.f;

            matrix->m[2][0] = (oneMinusCos * zx) + ys;
            matrix->m[2][1] = (oneMinusCos * yz) - xs;
            matrix->m[2][2] = (oneMinusCos * zz) + cosAngle;
            matrix->m[2][3] = 0.f;

            matrix->m[3][0] = 0.f;
            matrix->m[3][1] = 0.f;
            matrix->m[3][2] = 0.f;
            matrix->m[3][3] = 1.f;
        }
        else
        {
            MatrixIdentity( matrix );
        }
    }

    void MatrixRotateX( Matrix16* matrix, FLOAT32 angle)
    {
        FLOAT32 sinAngle, cosAngle;

        sinAngle = (FLOAT32)sinf(angle);
        cosAngle = (FLOAT32)cosf(angle);

        matrix->m[0][0] = 1.f;
        matrix->m[0][1] = 0.f;
        matrix->m[0][2] = 0.f;
        matrix->m[0][3] = 0.f;

        matrix->m[1][0] = 0.f;
        matrix->m[1][1] = cosAngle;
        matrix->m[1][2] = sinAngle;
        matrix->m[1][3] = 0.f;

        matrix->m[2][0] = 0.f;
        matrix->m[2][1] = -sinAngle;
        matrix->m[2][2] = cosAngle;
        matrix->m[2][3] = 0.f;

        matrix->m[3][0] = 0.f;
        matrix->m[3][1] = 0.f;
        matrix->m[3][2] = 0.f;
        matrix->m[3][3] = 1.f;
    }

    void MatrixRotateY( Matrix16* matrix, FLOAT32 angle)
    {
        FLOAT32 sinAngle, cosAngle;
        sinAngle = sinf(angle);
        cosAngle = cosf(angle);

        matrix->m[0][0] = cosAngle;
        matrix->m[0][1] = 0.f;
        matrix->m[0][2] = -sinAngle;
        matrix->m[0][3] = 0.f;

        matrix->m[1][0] = 0.f;
        matrix->m[1][1] = 1.f;
        matrix->m[1][2] = 0.f;
        matrix->m[1][3] = 0.f;

        matrix->m[2][0] = sinAngle;
        matrix->m[2][1] = 0.f;
        matrix->m[2][2] = cosAngle;
        matrix->m[2][3] = 0.f;

        matrix->m[3][0] = 0.f;
        matrix->m[3][1] = 0.f;
        matrix->m[3][2] = 0.f;
        matrix->m[3][3] = 1.f;
    }

    void MatrixRotateZ( Matrix16* matrix, FLOAT32 angle)
    {
        FLOAT32 sinAngle, cosAngle;
        sinAngle = sinf(angle);
        cosAngle = cosf(angle);

        matrix->m[0][0] = cosAngle;
        matrix->m[0][1] = sinAngle;
        matrix->m[0][2] = 0.f;
        matrix->m[0][3] = 0.f;

        matrix->m[1][0] = -sinAngle;
        matrix->m[1][1] = cosAngle;
        matrix->m[1][2] = 0.f;
        matrix->m[1][3] = 0.f;

        matrix->m[2][0] = 0.f;
        matrix->m[2][1] = 0.f;
        matrix->m[2][2] = 1.f;
        matrix->m[2][3] = 0.f;

        matrix->m[3][0] = 0.f;
        matrix->m[3][1] = 0.f;
        matrix->m[3][2] = 0.f;
        matrix->m[3][3] = 1.f;
    }

    void Point3Transform( Vector3* result, const Vector3* vector, const Matrix16* matrix )
    {
        Vector3 tempVector;
        FLOAT32 w;
        tempVector.x = matrix->m[0][0] * vector->x + matrix->m[1][0] * vector->y + matrix->m[2][0] * vector->z + matrix->m[3][0];
        tempVector.y = matrix->m[0][1] * vector->x + matrix->m[1][1] * vector->y + matrix->m[2][1] * vector->z + matrix->m[3][1];
        tempVector.z = matrix->m[0][2] * vector->x + matrix->m[1][2] * vector->y + matrix->m[2][2] * vector->z + matrix->m[3][2];
        w = matrix->m[0][3] * vector->x + matrix->m[1][3] * vector->y + matrix->m[2][3] * vector->z + matrix->m[3][3];
        tempVector.x /= w;
        tempVector.y /= w;
        tempVector.z /= w;
        *result = tempVector;
    }

    void Vector3Transform( Vector3* result, const Vector3* vector, const Matrix16* matrix)
    {
        Vector3 tempVector;
        tempVector.x = matrix->m[0][0] * vector->x + matrix->m[1][0] * vector->y + matrix->m[2][0] * vector->z;
        tempVector.y = matrix->m[0][1] * vector->x + matrix->m[1][1] * vector->y + matrix->m[2][1] * vector->z;
        tempVector.z = matrix->m[0][2] * vector->x + matrix->m[1][2] * vector->y + matrix->m[2][2] * vector->z;
        *result = tempVector;
    }

    void MatrixLookAt( Matrix16* result, const Vector3* from, const Vector3* to, const Vector3* up )
    {
        Matrix16 rotate;
        Vector3	i, j, k;

        Vector3Subtract( &k, from, to );
        Vector3Normalize( &k );

        Vector3Cross( &i, up, &k );
        Vector3Normalize( &i );

        Vector3Cross( &j, &k, &i ); 
        Vector3Normalize( &j );

        rotate.m[0][0] = i.x;
        rotate.m[1][0] = i.y;
        rotate.m[2][0] = i.z;
        rotate.m[3][0] = 0.f;

        rotate.m[0][1] = j.x;
        rotate.m[1][1] = j.y;
        rotate.m[2][1] = j.z;
        rotate.m[3][1] = 0.f;

        rotate.m[0][2] = k.x;
        rotate.m[1][2] = k.y;
        rotate.m[2][2] = k.z;
        rotate.m[3][2] = 0.f;

        rotate.m[0][3] = 0.f;
        rotate.m[1][3] = 0.f;
        rotate.m[2][3] = 0.f;
        rotate.m[3][3] = 1.f;

        MatrixTranslate( result, - from->x, - from->y, -from->z );
        MatrixMultiply( result, result, &rotate );
    }

    void MatrixFrustum( Matrix16* result, FLOAT32 left, FLOAT32 right, FLOAT32 bottom, FLOAT32 top, FLOAT32 near, FLOAT32 far )
    {
        FLOAT32 deltaX = right - left;
        FLOAT32 deltaY = top - bottom;
        FLOAT32 deltaZ = far - near;

        if ((near <= 0.0f) || (far <= 0.0f) || (deltaX <= 0.0f)
            || (deltaY <= 0.0f) || (deltaZ <= 0.0f))
        {
            MatrixIdentity( result );
            return;
        }
        result->m[0][0] = 1.0f / deltaX;
        result->m[1][0] = result->m[2][0] = result->m[3][0] = 0.0f;

        result->m[1][1] = 1.0f / deltaY;
        result->m[0][1] = result->m[2][1] = result->m[3][1] = 0.0f;

        result->m[0][2] = - (right + left) / deltaX;
        result->m[1][2] = - (top + bottom) / deltaY;
        result->m[2][2] = - (near + far) / deltaZ;
        result->m[3][2] = - 2.0f * near * far / deltaZ;

        result->m[2][3] = - 1.0f;
        result->m[0][3] = result->m[1][3] = result->m[3][3] = 0.0f;
    }

    void MatrixPerspective( Matrix16* result, FLOAT32 fovy, FLOAT32 aspect, FLOAT32 near, FLOAT32 far )
    {
        FLOAT32 frustumH = tanf(fovy / 2.f) * near;
        FLOAT32 frustumW = frustumH * aspect;

        MatrixFrustum( result, -frustumW, frustumW, -frustumH, frustumH, near, far );
    }
    
    void MatrixOrtho( Matrix16* result, FLOAT32 width, FLOAT32 height, FLOAT32 near, FLOAT32 far )
    {
        FLOAT32 deltaZ = far - near;
        if ( width <= 0.f || height <= 0.f || deltaZ <= 0.f )
        {
            MatrixIdentity( result );
            return;
        }

        result->m[0][0] = 2.f / width;
        result->m[0][3] = -1.f;
        result->m[0][1] = result->m[0][2] = 0.f;

        result->m[1][1] = 2.f / height;
        result->m[1][3] = -1.f;
        result->m[1][0] = result->m[1][2] = 0.f;

        result->m[2][2] = 2.f / deltaZ;
        result->m[2][3] = -( near + far ) / deltaZ;
        result->m[2][0] = result->m[2][1] = 0.f;

        result->m[3][3] = 1.f;
        result->m[3][0] = result->m[3][1] = result->m[3][2] = 0.f;
    }

    static FLOAT32 det(FLOAT32* a, int n); 
    static FLOAT32 rem(FLOAT32* a, int i, int j, int n); 
    static void inv(FLOAT32* a, FLOAT32* b, int n);

    FLOAT32 rem( FLOAT32* a,int i,int j,int n) 
    { 
        int k, m; 
        FLOAT32 *pTemp = (FLOAT32*)malloc( sizeof(FLOAT32) * ( n - 1 ) * ( n - 1 ) );   

        for ( k = 0; k < i; k++ ) 
        { 
            for ( m = 0; m < j; m++ )
                pTemp[k * ( n - 1 ) + m] = a[k * n + m]; 
            for ( m = j; m < n - 1; m++ )
                pTemp[k * ( n - 1 ) + m] = a[k * n + m + 1]; 
        } 
        for( k = i; k < n - 1; k++ ) 
        { 
            for( m = 0; m < j; m++ )
                pTemp[k * ( n - 1 ) + m] = a[( k + 1 ) * n + m]; 
            for( m = j; m < n - 1; m++ )
                pTemp[k * ( n - 1 ) + m] = a[( k + 1 ) * n + m + 1]; 
        } 
        FLOAT32 dResult = ( ( ( i + j ) % 2 == 1 ) ? -1 : 1 ) * det( pTemp, n - 1 ); 
        free( pTemp ); 
        return dResult;
    }

    FLOAT32 det( FLOAT32* a, int n ) 
    { 
        if ( n == 1 )
            return a[0]; 
        FLOAT32 sum = .0f; 
        for ( int j = 0; j < n; j++ )
            sum += a[0 * n + j] * rem( a, 0, j, n ); 
        return sum; 
    } 
    
    void inv( FLOAT32* a, FLOAT32* b, int n ) 
    { 
        FLOAT32 deta = det( a, n ); 
        for ( int i = 0; i < n; i++ ) 
            for( int j = 0; j < n; j++ ) 
                b[i * n + j] = rem( a, j, i, n ) / deta; 
    }
    void MatrixInverse( Matrix16* result, const Matrix16* matrix)
    {
        Matrix16 tempMatrix;
        inv((FLOAT32*)matrix, (FLOAT32*)&tempMatrix, 4);
        *result = tempMatrix;
    }

    void MatrixMultiply( Matrix16* result, const Matrix16* matrix1, const Matrix16* matrix2 )
    {
        Matrix16 tempMatrix;
        int i;

        for ( i = 0; i < 4; i++ )
        {
            tempMatrix.m[i][0] = (matrix1->m[i][0] * matrix2->m[0][0])
            + (matrix1->m[i][1] * matrix2->m[1][0])
            + (matrix1->m[i][2] * matrix2->m[2][0])
            + (matrix1->m[i][3] * matrix2->m[3][0]);

            tempMatrix.m[i][1] = (matrix1->m[i][0] * matrix2->m[0][1])
            + (matrix1->m[i][1] * matrix2->m[1][1])
            + (matrix1->m[i][2] * matrix2->m[2][1])
            + (matrix1->m[i][3] * matrix2->m[3][1]);

            tempMatrix.m[i][2] = (matrix1->m[i][0] * matrix2->m[0][2])
            + (matrix1->m[i][1] * matrix2->m[1][2])
            + (matrix1->m[i][2] * matrix2->m[2][2])
            + (matrix1->m[i][3] * matrix2->m[3][2]);

            tempMatrix.m[i][3] = (matrix1->m[i][0] * matrix2->m[0][3])
            + (matrix1->m[i][1] * matrix2->m[1][3])
            + (matrix1->m[i][2] * matrix2->m[2][3])
            + (matrix1->m[i][3] * matrix2->m[3][3]);
        }

        *result = tempMatrix;
    }

    void Vector3Add( Vector3* result, const Vector3* vec1, const Vector3* vec2 )
    {
        result->x = vec1->x + vec2->x;
        result->y = vec1->y + vec2->y;
        result->z = vec1->z + vec2->z;
    }

    void Vector3Subtract( Vector3* result, const Vector3* vec1, const Vector3* vec2 )
    {
        result->x = vec1->x - vec2->x;
        result->y = vec1->y - vec2->y;
        result->z = vec1->z - vec2->z;
    }

    void Vector3Cross( Vector3* result, const Vector3* vec1, const Vector3* vec2 )
    {
        Vector3 temp;
        temp.x = vec1->y * vec2->z - vec1->z * vec2->y;
        temp.y = vec1->z * vec2->x - vec1->x * vec2->z;
        temp.z = vec1->x * vec2->y - vec1->y * vec2->x;
        *result = temp;
    }

    FLOAT32 Vector3Dot( const Vector3* vec1, const Vector3* vec2 )
    {
        return vec1->x * vec2->x + vec1->y * vec2->y + vec1->z * vec2->z;
    }

    FLOAT32 Vector3Magnitude( const Vector3* vec )
    {
        return sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
    }

    void Vector3Normalize( Vector3* vec )
    {
        FLOAT32 scale = Vector3Magnitude( vec );
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
