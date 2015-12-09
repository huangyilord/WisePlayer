#if !defined ( _GRAPHIC_TYPE_DEF_H_ )
#define _GRAPHIC_TYPE_DEF_H_

namespace Graphic
{
#if !defined DECLSPEC_NOVTABLE
#define DECLSPEC_NOVTABLE
#endif
    
#define SCREEN_COORD_X_MAX 1.f
#define SCREEN_COORD_Y_MAX 1.f
#define SCREEN_COORD_Z_MAX 1.f
#define SCREEN_COORD_X_MIN -1.f
#define SCREEN_COORD_Y_MIN -1.f
#define SCREEN_COORD_Z_MIN 0.f

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

    typedef VOID*                   VERTEX_BUFFER_HANDLE;
    typedef VOID*                   INDEX_BUFFER_HANDLE;

    typedef VOID*                   TEXTURE_HANDLE;
    typedef VOID*                   SURFACE_HANDLE;
    typedef VOID*                   RENDER_TARGET_HANDLE;

    typedef UINT32                  PIXEL_SHADER_HANDLE;
    typedef UINT32                  VERTEX_SHADER_HANDLE;
    typedef UINT32                  SHADER_PROGRAM_HANDLE;

    typedef VOID*                   VERTEX_BUFFER_FORMAT_HANDLE;
    typedef VOID*                   SURFACE_DATA_HANDLE;

    typedef struct _matrix16
    {
        FLOAT32 m[4][4];
        _matrix16() {}
        _matrix16( FLOAT32 m00, FLOAT32 m01, FLOAT32 m02, FLOAT32 m03
                  , FLOAT32 m10, FLOAT32 m11, FLOAT32 m12, FLOAT32 m13
                  , FLOAT32 m20, FLOAT32 m21, FLOAT32 m22, FLOAT32 m23
                  , FLOAT32 m30, FLOAT32 m31, FLOAT32 m32, FLOAT32 m33)
        {
            m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
            m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
            m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
            m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
        }

        FLOAT32& operator() ( UINT32 i, UINT32 j ) { return m[i][j]; }
        FLOAT32 operator() ( UINT32 i, UINT32 j ) const { return m[i][j]; }
    } Matrix16;

    typedef struct _matrix9
    {
        FLOAT32 m[3][3];
        _matrix9() {}
        _matrix9( FLOAT32 m00, FLOAT32 m01, FLOAT32 m02
                 , FLOAT32 m10, FLOAT32 m11, FLOAT32 m12
                 , FLOAT32 m20, FLOAT32 m21, FLOAT32 m22)
        {
            m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
            m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
            m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
        }

        FLOAT32 operator() ( UINT32 i, UINT32 j ) { return m[i][j]; }
    } Matrix9;

    typedef struct _vector2
    {
        FLOAT32 x;
        FLOAT32 y;
        _vector2( FLOAT32 _x, FLOAT32 _y )
        : x( _x ), y( _y ) {}
        _vector2()
        : x( 0.f ), y( 0.f ) {}
    } Vector2;
    
    typedef struct _vector3
    {
        FLOAT32 x;
        FLOAT32 y;
        FLOAT32 z;
        _vector3( FLOAT32 _x, FLOAT32 _y, FLOAT32 _z )
            : x( _x ), y( _y ), z( _z ) {}
        _vector3()
        : x( 0.f ), y( 0.f ), z( 0.f ) {}
    } Vector3;
    
    typedef struct _vector4
    {
        FLOAT32 x;
        FLOAT32 y;
        FLOAT32 z;
        FLOAT32 w;

        _vector4( FLOAT32 _x, FLOAT32 _y, FLOAT32 _z, FLOAT32 _w )
        : x( _x ), y( _y ), z( _z ), w( _w ) {}
        _vector4()
        : x( 0.f ), y( 0.f ), z( 0.f ), w( 0.f ) {}
    } Vector4;

    typedef struct _colorf_rgba
    {
        FLOAT32 r;
        FLOAT32 g;
        FLOAT32 b;
        FLOAT32 a;
    } ColorF_RGBA;
}

#endif
