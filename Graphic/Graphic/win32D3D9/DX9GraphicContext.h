#if !defined ( _DX9_GRAPHIC_CONTEXT_H_ )
#define _DX9_GRAPHIC_CONTEXT_H_

#include "Graphic/IGraphicContext.h"

namespace Graphic
{
    class DX9GraphicContext
        : public IGraphicContext
    {
    public:
        DX9GraphicContext();
        ~DX9GraphicContext();

        BOOL Initialize( HWND hWnd );
        VOID Destroy();

        virtual VERTEX_BUFFER_FORMAT_HANDLE CreateVertexBufferFormat( const VertexElement* elements, UINT32 count );
        virtual VOID DestroyVertexBufferFormat( VERTEX_BUFFER_FORMAT_HANDLE format );

        virtual VERTEX_BUFFER_HANDLE CreateVertexBuffer( UINT32 size );
        virtual VOID* GetVertexBufferPointer( VERTEX_BUFFER_HANDLE vtxHandle );
        virtual BOOL CommitVertexBuffer( VOID* pBuffer, VERTEX_BUFFER_HANDLE vtxHandle );
        virtual VOID DestroyVertexBuffer( VERTEX_BUFFER_HANDLE vtxHandle );

        virtual INDEX_BUFFER_HANDLE CreateIndexBuffer( UINT32 size );
        virtual UINT16* GetIndexBufferPointer( INDEX_BUFFER_HANDLE idxHandle );
        virtual BOOL CommitIndexBuffer( UINT16* pBuffer, INDEX_BUFFER_HANDLE idxHandle );
        virtual VOID DestroyIndexBuffer( INDEX_BUFFER_HANDLE idxHandle );

        virtual SURFACE_DATA_HANDLE GetRenderTargetData( UINT32 x, UINT32 y, UINT32 width, UINT32 height );
        virtual VOID* GetSurfaceDataPointer( SURFACE_DATA_HANDLE dataHandle );
        virtual VOID ReleaseSurfaceData( SURFACE_DATA_HANDLE dataHandle );
        virtual VOID GetSurfaceSize( SURFACE_HANDLE surface, UINT32 &width, UINT32 &height );

        virtual TEXTURE_HANDLE CreateTexture( UINT32 width, UINT32 height );
        virtual TEXTURE_HANDLE CreateTexture( const VOID* data, UINT32 width, UINT32 height, PixelType pixelType );
        virtual VOID TextureSubImage( TEXTURE_HANDLE texture, const VOID* data, UINT32 width, UINT32 height, UINT32 x, UINT32 y );
        virtual VOID DestroyTexture( TEXTURE_HANDLE texture );

        virtual VOID EnableTexture( UINT32 stage );
        virtual VOID DisableTexture( UINT32 stage );
        virtual VOID SetTexture( UINT32 stage, TEXTURE_HANDLE texture );

        virtual SURFACE_HANDLE GetSurfaceFromTexture( TEXTURE_HANDLE texture );

        virtual RENDER_TARGET_HANDLE CreateRenderTarget();
        virtual VOID DestroyRenderTarget( RENDER_TARGET_HANDLE target );
        virtual VOID SetRenderTarget( RENDER_TARGET_HANDLE target );
        virtual RENDER_TARGET_HANDLE GetRenderTarget();

        virtual SURFACE_HANDLE CreateDepthStencilSurface( UINT32 width, UINT32 height );
        virtual VOID DestroyDepthStencilSurface( SURFACE_HANDLE surface );
        virtual VOID SetDepthStencilSurface( RENDER_TARGET_HANDLE target, SURFACE_HANDLE surface );
        virtual SURFACE_HANDLE GetDepthStencilSurface( RENDER_TARGET_HANDLE target );
        virtual VOID SetColorSurface( RENDER_TARGET_HANDLE target, SURFACE_HANDLE surface );
        virtual SURFACE_HANDLE GetColorSurface( RENDER_TARGET_HANDLE target );

        virtual VOID EnableDepthTest( BOOL bEnable );
        virtual VOID SetDepthTestFunc( DepthTestFunction func );
        virtual VOID DepthBufferWriteEnable( BOOL bEnable );
        virtual VOID ColorWriteEnable( BOOL bRedEnable, BOOL bGreenEnable, BOOL bBlueEnable, BOOL bAlphaEnable );

        virtual VOID EnableBlend( BOOL bEnable );
        virtual VOID SetBlendFunc( BlendFunction src, BlendFunction dst );

        virtual VOID SetCullMode( CullMode mode );
        virtual VOID EnableCullFace( BOOL bEnable );

        virtual VOID EnableStencilTest( BOOL bEnable );
        virtual VOID SetStencilOperation( StencilOperation fail, StencilOperation zFail, StencilOperation pass );
        virtual VOID SetStencilTestFunc( StencilTestFunction func, UINT32 ref, UINT32 mask );
        virtual VOID SetStencilWriteMask( UINT32 mask );

        virtual VOID SetViewPort( const ViewPort& viewport );
        virtual VOID GetViewPort( ViewPort& viewport );

        virtual VOID SetVertexBuffer( VERTEX_BUFFER_HANDLE vtxHandle, VERTEX_BUFFER_FORMAT_HANDLE format );
        virtual VOID SetIndexBuffer( INDEX_BUFFER_HANDLE idxHandle );

        virtual VOID DrawPrimitive( PrimitiveType type, UINT32 startVtx, UINT32 vtxCount, UINT32 primitiveCount );
        virtual VOID DrawIndexedPrimitive( PrimitiveType type, UINT32 startVtx, UINT32 vtxCount, UINT32 startIdx, UINT32 idxCount, UINT32 primitiveCount );

        virtual BOOL BeginScene();
        virtual VOID EndScene();

        virtual VOID Clear( UINT32 flags, ColorF_RGBA& color, FLOAT32 depth, DWORD stencil );
        virtual VOID Present();

        virtual VERTEX_SHADER_HANDLE CreateVertexShader( const CHAR* input, UINT32 len );
        virtual VERTEX_SHADER_HANDLE CreateVertexShader( const VOID* pBuffer, UINT32 size );
        virtual VOID DestroyVertexShader( VERTEX_SHADER_HANDLE shader );

        virtual PIXEL_SHADER_HANDLE CreatePixelShader( const CHAR* input, UINT32 len );
        virtual PIXEL_SHADER_HANDLE CreatePixelShader( const VOID* pBuffer, UINT32 size );
        virtual VOID DestroyPixelShader( PIXEL_SHADER_HANDLE shader );

        virtual SHADER_PROGRAM_HANDLE CreateShaderProgram( VERTEX_SHADER_HANDLE vtxShader, PIXEL_SHADER_HANDLE pxlShader );
        virtual VOID SetShaderProgram( SHADER_PROGRAM_HANDLE program );
        virtual VOID DestroyShaderProgram( SHADER_PROGRAM_HANDLE program );

        virtual UINT32 GetVertexShaderConstantLocation( SHADER_PROGRAM_HANDLE program, const CHAR* name );
        virtual VOID SetVertexShaderConstantFloat( UINT32 location, FLOAT32 value );
        virtual VOID SetVertexShaderConstantFloatArray( UINT32 location, const FLOAT32* value, UINT32 count );
        virtual VOID SetVertexShaderConstantInt( UINT32 location, INT32 value );
        virtual VOID SetVertexShaderConstantIntArray( UINT32 location, const INT32* value, UINT32 count );
        virtual VOID SetVertexShaderConstantMatrix( UINT32 location, const Matrix16* value );
        virtual VOID SetVertexShaderConstantMatrix( UINT32 location, const Matrix9* value );
        virtual VOID SetVertexShaderConstantMatrixArray( UINT32 location, const Matrix16* value, UINT32 count );
        virtual VOID SetVertexShaderConstantVector( UINT32 location, const Vector4* value );
        virtual VOID SetVertexShaderConstantVectorArray( UINT32 location, const Vector4* value, UINT32 count );
        virtual VOID SetVertexShaderConstantVector( UINT32 location, const Vector3* value );
        virtual VOID SetVertexShaderConstantVector( UINT32 location, const Vector2* value );

        virtual UINT32 GetPixelShaderConstantLocation( SHADER_PROGRAM_HANDLE program, const CHAR* name );
        virtual VOID SetPixelShaderConstantFloat( UINT32 location, FLOAT32 value );
        virtual VOID SetPixelShaderConstantFloatArray( UINT32 location, const FLOAT32* value, UINT32 count );
        virtual VOID SetPixelShaderConstantInt( UINT32 location, INT32 value );
        virtual VOID SetPixelShaderConstantIntArray( UINT32 location, const INT32* value, UINT32 count );
        virtual VOID SetPixelShaderConstantMatrix( UINT32 location, const Matrix16* value );
        virtual VOID SetPixelShaderConstantMatrix( UINT32 location, const Matrix9* value );
        virtual VOID SetPixelShaderConstantMatrixArray( UINT32 location, const Matrix16* value, UINT32 count );
        virtual VOID SetPixelShaderConstantVector( UINT32 location, const Vector4* value );
        virtual VOID SetPixelShaderConstantVectorArray( UINT32 location, const Vector4* value, UINT32 count );
        virtual VOID SetPixelShaderConstantVector( UINT32 location, const Vector3* value );
        virtual VOID SetPixelShaderConstantVector( UINT32 location, const Vector2* value );

        virtual VOID SetShaderTexture( SHADER_PROGRAM_HANDLE program, const CHAR* name, UINT32 stage, TEXTURE_HANDLE texture );

        virtual VOID Release();
    private:
        HWND                    m_hWnd;
        D3DPRESENT_PARAMETERS   m_D3DParameter;
		LPDIRECT3DDEVICE9       m_pD3DDevice;
        ViewPort                m_viewport;
        RENDER_TARGET_HANDLE    m_pRenderTarget;
        SHADER_PROGRAM_HANDLE   m_currentShaderProgram;

	private:
		class DECLSPEC_NOVTABLE Direct9
		{
		public:
			Direct9();
			~Direct9();

            inline LPDIRECT3D9 GetDirectHandler() { return m_pD3D; }
        private:
            LPDIRECT3D9 m_pD3D;
		};

		static Direct9 s_Direct;
    };
}

#endif
