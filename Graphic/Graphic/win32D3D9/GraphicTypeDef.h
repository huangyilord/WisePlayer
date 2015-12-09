#if !defined ( _GRAPHIC_TYPE_DEF_H_ )
#define _GRAPHIC_TYPE_DEF_H_

#pragma warning ( disable : 4819 )
#include<d3d9.h>
#include<d3dx9.h>
#pragma warning ( default : 4819 )

namespace Graphic
{
#define SCREEN_COORD_X_MAX 1.f
#define SCREEN_COORD_Y_MAX 1.f
#define SCREEN_COORD_Z_MAX 1.f
#define SCREEN_COORD_X_MIN -1.f
#define SCREEN_COORD_Y_MIN -1.f
#define SCREEN_COORD_Z_MIN 0.f

    typedef VOID*                   VERTEX_BUFFER_FORMAT_HANDLE;
    typedef VOID*                   VERTEX_SHADER_HANDLE;
    typedef VOID*                   PIXEL_SHADER_HANDLE;

    typedef VOID*                   VERTEX_BUFFER_HANDLE;
    typedef VOID*                   INDEX_BUFFER_HANDLE;

    typedef VOID*                   TEXTURE_HANDLE;
    typedef VOID*                   SURFACE_HANDLE;
    typedef VOID*                   RENDER_TARGET_HANDLE;

    typedef VOID*                   PIXEL_SHADER_HANDLE;
    typedef VOID*                   VERTEX_SHADER_HANDLE;
    typedef VOID*                   SHADER_PROGRAM_HANDLE;
    typedef VOID*                   SHADER_CONSTANT_TABLE_HANDLE;

    typedef VOID*                   SURFACE_DATA_HANDLE;

    typedef D3DXMATRIXA16           Matrix16;
    typedef D3DXVECTOR2             Vector2;
    typedef D3DXVECTOR3             Vector3;
    typedef D3DXVECTOR4             Vector4;
    typedef D3DCOLORVALUE           ColorF_RGBA;

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
}

#endif
