#if !defined ( _I_GRAPHIC_CONTEXT_H_ )
#define _I_GRAPHIC_CONTEXT_H_

#include "GraphicTypes.h"

namespace Graphic
{
    class DECLSPEC_NOVTABLE IGraphicContext
    {
    public:
        enum DataType
        {
            DataType_Float             = 0
            , DataType_Byte
            , DataType_Short

            , DataType_Num
        };

        enum DataUsage
        {
            DataUsage_Position          = 0
            , DataUsage_BlendWeight
            , DataUsage_BlendIndices
            , DataUsage_Normal
            , DataUsage_Color0
            , DataUsage_Color1
            , DataUsage_TexCoord0
            , DataUsage_TexCoord1
            , DataUsage_Num
        };

        enum PixelType
        {
            PixelType_32_8888           = 0
            , PixelType_16_4444
            , PixelType_16_565
            , PixelType_16_5551

            , PixelType_Num
        };

        typedef struct VertexElement
        {
            DataUsage   usage;
            DataType    dataType;
            UINT16      size;
            UINT16      offset;
        } VertexElement;

        enum PrimitiveType
        {
            Primitive_Triagnles         = 0
            , Primitive_TriagnleStrip
            , Primitive_TriagnleFan

            , PrimitiveType_Num
        };

        enum BlendFunction
        {
            Blend_Zero                  = 0
            , Blend_One
            , Blend_Src_Color
            , Blend_Inverse_Src_Color
            , Blend_Src_Alpha
            , Blend_Inverse_Src_Alpha
            , Blend_Dst_Color
            , Blend_Inverse_Dst_Color
            , Blend_Dst_Alpha
            , Blend_Inverse_Dst_Alpha

            , BlendFunction_Num
        };

        enum ClearFlag
        {
            Clear_Color                 = 0x1
            , Clear_Depth               = 0x2
            , Clear_Stencil             = 0x4
        };

        enum StencilOperation
        {
            StencilOp_Keep              = 0
            , StencilOp_Zero
            , StencilOp_Replace
            , StencilOp_Increase
            , StencilOp_Decrease
            , StencilOp_Invert

            , StencilOperation_Num
        };

        enum StencilTestFunction
        {
            StencilTest_Never           = 0
            , StencilTest_Less
            , StencilTest_Equal
            , StencilTest_LessEqual
            , StencilTest_Greater
            , StencilTest_NotEqual
            , StencilTest_GreaterEqual
            , StencilTest_Always

            , StencilTestFunction_Num
        };

        enum DepthTestFunction
        {
            DepthTest_Never             = 0
            , DepthTest_Less
            , DepthTest_Equal
            , DepthTest_LessEqual
            , DepthTest_Greater
            , DepthTest_NotEqual
            , DepthTest_GreaterEqual
            , DepthTest_Always

            , DepthTestFunction_Num
        };

        enum CullMode
        {
            CullMode_ClockWise          = 0
            , CullMode_CounterClockWise
        };

        virtual VERTEX_BUFFER_FORMAT_HANDLE CreateVertexBufferFormat( const VertexElement* elements, UINT32 count ) = 0;
        virtual VOID DestroyVertexBufferFormat( VERTEX_BUFFER_FORMAT_HANDLE format ) = 0;

        virtual VERTEX_BUFFER_HANDLE CreateVertexBuffer( UINT32 size ) = 0;
        virtual VOID* GetVertexBufferPointer( VERTEX_BUFFER_HANDLE vtxHandle ) = 0;
        virtual BOOL CommitVertexBuffer( VOID* pBuffer, VERTEX_BUFFER_HANDLE vtxHandle ) = 0;
        virtual VOID DestroyVertexBuffer( VERTEX_BUFFER_HANDLE vtxHandle ) = 0;

        virtual INDEX_BUFFER_HANDLE CreateIndexBuffer( UINT32 size ) = 0;
        virtual UINT16* GetIndexBufferPointer( INDEX_BUFFER_HANDLE idxHandle ) = 0;
        virtual BOOL CommitIndexBuffer( UINT16* pBuffer, INDEX_BUFFER_HANDLE idxHandle ) = 0;
        virtual VOID DestroyIndexBuffer( INDEX_BUFFER_HANDLE idxHandle ) = 0;

        virtual SURFACE_DATA_HANDLE GetRenderTargetData( UINT32 x, UINT32 y, UINT32 width, UINT32 height ) = 0;
        virtual VOID* GetSurfaceDataPointer( SURFACE_DATA_HANDLE dataHandle ) = 0;
        virtual VOID ReleaseSurfaceData( SURFACE_DATA_HANDLE dataHandle ) = 0;
        virtual VOID GetSurfaceSize( SURFACE_HANDLE surface, UINT32 &width, UINT32 &height ) = 0;

        virtual TEXTURE_HANDLE CreateTexture( UINT32 width, UINT32 height ) = 0;
        virtual TEXTURE_HANDLE CreateTexture( const VOID* data, UINT32 width, UINT32 height, PixelType pixelType ) = 0;
        virtual VOID TextureSubImage( TEXTURE_HANDLE texture, const VOID* data, UINT32 width, UINT32 height, UINT32 x, UINT32 y ) = 0;
        virtual VOID DestroyTexture( TEXTURE_HANDLE texture ) = 0;

        virtual VOID EnableTexture( UINT32 stage ) = 0;
        virtual VOID DisableTexture( UINT32 stage ) = 0;
        virtual VOID SetTexture( UINT32 stage, TEXTURE_HANDLE texture ) = 0;

        virtual SURFACE_HANDLE GetSurfaceFromTexture( TEXTURE_HANDLE texture ) = 0;

        virtual RENDER_TARGET_HANDLE CreateRenderTarget() = 0;
        virtual VOID DestroyRenderTarget( RENDER_TARGET_HANDLE target ) = 0;
        virtual VOID SetRenderTarget( RENDER_TARGET_HANDLE target ) = 0;
        virtual RENDER_TARGET_HANDLE GetRenderTarget() = 0;

        virtual SURFACE_HANDLE CreateDepthStencilSurface( UINT32 width, UINT32 height ) = 0;
        virtual VOID DestroyDepthStencilSurface( SURFACE_HANDLE surface ) = 0;
        virtual VOID SetDepthStencilSurface( RENDER_TARGET_HANDLE target, SURFACE_HANDLE surface ) = 0;
        virtual SURFACE_HANDLE GetDepthStencilSurface( RENDER_TARGET_HANDLE target ) = 0;
        virtual VOID SetColorSurface( RENDER_TARGET_HANDLE target, SURFACE_HANDLE surface ) = 0;
        virtual SURFACE_HANDLE GetColorSurface( RENDER_TARGET_HANDLE target ) = 0;

        virtual VOID EnableDepthTest( BOOL bEnable ) = 0;
        virtual VOID SetDepthTestFunc( DepthTestFunction func ) = 0;
        virtual VOID DepthBufferWriteEnable( BOOL bEnable ) = 0;
        virtual VOID ColorWriteEnable( BOOL bRedEnable, BOOL bGreenEnable, BOOL bBlueEnable, BOOL bAlphaEnable ) = 0;

        virtual VOID EnableBlend( BOOL bEnable ) = 0;
        virtual VOID SetBlendFunc( BlendFunction src, BlendFunction dst ) = 0;

        virtual VOID SetCullMode( CullMode mode ) = 0;
        virtual VOID EnableCullFace( BOOL bEnable ) = 0;

        virtual VOID EnableStencilTest( BOOL bEnable ) = 0;
        virtual VOID SetStencilOperation( StencilOperation fail, StencilOperation zFail, StencilOperation pass ) = 0;
        virtual VOID SetStencilTestFunc( StencilTestFunction func, UINT32 ref, UINT32 mask ) = 0;
        virtual VOID SetStencilWriteMask( UINT32 mask ) = 0;

        virtual VOID SetViewPort( const ViewPort& viewport ) = 0;
        virtual VOID GetViewPort( ViewPort& viewport ) = 0;

        virtual VOID SetVertexBuffer( VERTEX_BUFFER_HANDLE vtxHandle, VERTEX_BUFFER_FORMAT_HANDLE format ) = 0;
        virtual VOID SetIndexBuffer( INDEX_BUFFER_HANDLE idxHandle ) = 0;

        virtual VOID DrawPrimitive( PrimitiveType type, UINT32 startVtx, UINT32 vtxCount, UINT32 primitiveCount ) = 0;
        virtual VOID DrawIndexedPrimitive( PrimitiveType type, UINT32 startVtx, UINT32 vtxCount, UINT32 startIdx, UINT32 idxCount, UINT32 primitiveCount ) = 0;

        virtual BOOL BeginScene() = 0;
        virtual VOID EndScene() = 0;

        virtual VOID Clear( UINT32 flags, ColorF_RGBA& color, FLOAT32 depth, DWORD stencil ) = 0;
        virtual VOID Present() = 0;

        virtual VERTEX_SHADER_HANDLE CreateVertexShader( const CHAR* input, UINT32 len ) = 0;
        virtual VERTEX_SHADER_HANDLE CreateVertexShader( const VOID* pBuffer, UINT32 size ) = 0;
        virtual VOID DestroyVertexShader( VERTEX_SHADER_HANDLE shader ) = 0;

        virtual PIXEL_SHADER_HANDLE CreatePixelShader( const CHAR* input, UINT32 len ) = 0;
        virtual PIXEL_SHADER_HANDLE CreatePixelShader( const VOID* pBuffer, UINT32 size ) = 0;
        virtual VOID DestroyPixelShader( PIXEL_SHADER_HANDLE shader ) = 0;

        virtual SHADER_PROGRAM_HANDLE CreateShaderProgram( VERTEX_SHADER_HANDLE vtxShader, PIXEL_SHADER_HANDLE pxlShader ) = 0;
        virtual VOID SetShaderProgram( SHADER_PROGRAM_HANDLE program ) = 0;
        virtual SHADER_PROGRAM_HANDLE GetShaderProgram() = 0;
        virtual VOID ResetShaderProgram() = 0;
        virtual VOID DestroyShaderProgram( SHADER_PROGRAM_HANDLE program ) = 0;

        virtual UINT32 GetVertexShaderConstantLocation( SHADER_PROGRAM_HANDLE program, const CHAR* name ) = 0;
        virtual VOID SetVertexShaderConstantFloat( UINT32 location, FLOAT32 value ) = 0;
        virtual VOID SetVertexShaderConstantFloatArray( UINT32 location, const FLOAT32* value, UINT32 count ) = 0;
        virtual VOID SetVertexShaderConstantInt( UINT32 location, INT32 value ) = 0;
        virtual VOID SetVertexShaderConstantIntArray( UINT32 location, const INT32* value, UINT32 count ) = 0;
        virtual VOID SetVertexShaderConstantMatrix( UINT32 location, const Matrix16* value ) = 0;
        virtual VOID SetVertexShaderConstantMatrix( UINT32 location, const Matrix9* value ) = 0;
        virtual VOID SetVertexShaderConstantMatrixArray( UINT32 location, const Matrix16* value, UINT32 count ) = 0;
        virtual VOID SetVertexShaderConstantVector( UINT32 location, const Vector4* value ) = 0;
        virtual VOID SetVertexShaderConstantVectorArray( UINT32 location, const Vector4* value, UINT32 count ) = 0;
        virtual VOID SetVertexShaderConstantVector( UINT32 location, const Vector3* value ) = 0;
        virtual VOID SetVertexShaderConstantVector( UINT32 location, const Vector2* value ) = 0;

        virtual UINT32 GetPixelShaderConstantLocation( SHADER_PROGRAM_HANDLE program, const CHAR* name ) = 0;
        virtual VOID SetPixelShaderConstantFloat( UINT32 location, FLOAT32 value ) = 0;
        virtual VOID SetPixelShaderConstantFloatArray( UINT32 location, const FLOAT32* value, UINT32 count ) = 0;
        virtual VOID SetPixelShaderConstantInt( UINT32 location, INT32 value ) = 0;
        virtual VOID SetPixelShaderConstantIntArray( UINT32 location, const INT32* value, UINT32 count ) = 0;
        virtual VOID SetPixelShaderConstantMatrix( UINT32 location, const Matrix16* value ) = 0;
        virtual VOID SetPixelShaderConstantMatrix( UINT32 location, const Matrix9* value ) = 0;
        virtual VOID SetPixelShaderConstantMatrixArray( UINT32 location, const Matrix16* value, UINT32 count ) = 0;
        virtual VOID SetPixelShaderConstantVector( UINT32 location, const Vector4* value ) = 0;
        virtual VOID SetPixelShaderConstantVectorArray( UINT32 location, const Vector4* value, UINT32 count ) = 0;
        virtual VOID SetPixelShaderConstantVector( UINT32 location, const Vector3* value ) = 0;
        virtual VOID SetPixelShaderConstantVector( UINT32 location, const Vector2* value ) = 0;

        virtual VOID SetShaderTexture( SHADER_PROGRAM_HANDLE program, const CHAR* name, UINT32 stage, TEXTURE_HANDLE texture ) = 0;

        virtual VOID Release() = 0;
        virtual VOID EnsureThreadContext() = 0;
    };
}

#endif
