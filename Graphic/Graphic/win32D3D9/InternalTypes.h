#if !defined ( _INTERNAL_TYPES_H_ )
#define _INTERNAL_TYPES_H_

#include "GraphicTypeDef.h"

namespace Graphic
{
    struct RenderTarget
    {
        LPDIRECT3DSURFACE9  renderSurface;
        LPDIRECT3DSURFACE9  depthStencil;
    };

    struct VertexShader
    {
        LPDIRECT3DVERTEXSHADER9     vertexShader;
        LPD3DXCONSTANTTABLE         vertexShaderConstant;
    };

    struct PixelShader
    {
        LPDIRECT3DPIXELSHADER9      pixelShader;
        LPD3DXCONSTANTTABLE         pixelShaderConstant;
    };

    struct ShaderProgram
    {
        VertexShader*               vertexShader;
        PixelShader*                pixelShader;
    };

    struct SurfaceDataReader
    {
        LPDIRECT3DSURFACE9          surface;
        D3DLOCKED_RECT              lockedRect;
    };

    struct VertexBufferFormat
    {
        LPDIRECT3DVERTEXDECLARATION9    format;
        UINT32                          stride;
    };
}

#endif
