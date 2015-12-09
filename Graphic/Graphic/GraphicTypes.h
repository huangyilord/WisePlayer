#if !defined ( _GRAPHIC_TYPES_H_ )
#define _GRAPHIC_TYPES_H_

#include "GraphicTypeDef.h"
#include "GraphicUtil.h"
#include <list>
#include <set>
#include <map>

namespace Graphic
{
    class IGraphicContext;
    class UtilGraphicResourceController;

    typedef struct _ViewPort
    {
        UINT32  uMinX;
        UINT32  uMinY;
        UINT32  uWidth;
        UINT32  uHeight;
    } ViewPort;

    typedef struct _RenderContext
    {
        UINT32  timeStamp;
        UINT32  timeInterval;
        FLOAT32 width;
        FLOAT32 height;
    } RenderContext;

    typedef struct _TextureInfo
    {
        TEXTURE_HANDLE  texture;
        Box2D           texCoord;
    } TextureInfo;

    class GraphicFilter
    {
    public:
        GraphicFilter();
        virtual ~GraphicFilter() {}

        virtual VOID Render( IGraphicContext* pGraphicContext, const RenderContext* context, TEXTURE_HANDLE pContent ) = 0;
    private:
    };

    class DECLSPEC_NOVTABLE GraphicFilterContainer
    {
    public:
        GraphicFilterContainer();
        ~GraphicFilterContainer();

        inline VOID SetResController( UtilGraphicResourceController* resController ) { m_resController = resController; }
        inline UtilGraphicResourceController* GetResController() const { return m_resController; }

        BOOL FilterStart( IGraphicContext* pGraphicContext, const RenderContext* context );
        VOID FilterEnd( IGraphicContext* pGraphicContext, const RenderContext* context );

        VOID AddFilter( GraphicFilter* filter );
        VOID RemoveFilter( GraphicFilter* filter );
        VOID ClearFilter();

        BOOL EngageCurrentTarget();
        VOID FreeCurrentTarget();
    private:
        UtilGraphicResourceController*      m_resController;
        RENDER_TARGET_HANDLE                m_swapRenderTarget[2];
        UINT32                              m_uTexIndex[2];
        UINT32                              m_uCurrentBuffer;
        std::list<GraphicFilter*>           m_filterList;
        RENDER_TARGET_HANDLE                m_originTarget;
        ViewPort                            m_originViewport;
    };

    class LayerCollection;
    class GraphicLayer
    {
    public:
        GraphicLayer();
        ~GraphicLayer();

        VOID Render( IGraphicContext* pGraphicContext, const RenderContext* context );

        inline VOID AddFilter( GraphicFilter* filter ) { m_FilterContainer.AddFilter( filter ); }
        inline VOID RemoveFilter( GraphicFilter* filter ) { m_FilterContainer.RemoveFilter( filter ); }
        inline VOID ClearFilter() { m_FilterContainer.ClearFilter(); }

        virtual VOID SetResController( UtilGraphicResourceController* resController )
            { m_FilterContainer.SetResController( resController ); }
        inline UtilGraphicResourceController* GetResController() const { return m_FilterContainer.GetResController(); }
    protected:
        virtual VOID RenderContent( IGraphicContext* pGraphicContext, const RenderContext* context ) = 0;

    private:
        GraphicFilterContainer    m_FilterContainer;
        LayerCollection*          m_pParent;
        friend class LayerCollection; 
    };

    class DECLSPEC_NOVTABLE Object3D
    {
    public:
        Object3D();

        VOID ResetTransform();
        VOID Translate( const Vector3& offset );
        VOID Rotate( const Vector3& axis, FLOAT32 angle );
        VOID RotateX( FLOAT32 angle );
        VOID RotateY( FLOAT32 angle );
        VOID RotateZ( FLOAT32 angle );

        VOID Scale( FLOAT32 sx, FLOAT32 sy, FLOAT32 sz );

        VOID GetAbsoluteTransform( Matrix16& mtrx ) const;
        VOID GetAbsolutePosition( Vector3& pos ) const;
        VOID GetAbsolutePosition( Vector2& pos ) const;
        VOID GetPosition( Vector3& pos ) const;

        BOOL IsChildOf( Object3D* pObj ) const;

        inline VOID GetTransform( Matrix16& mtrx ) const { mtrx = m_transform; }
        inline VOID LoadTransform( const Matrix16& transform ) { m_transform = transform; }

        inline VOID SetParent( Object3D* obj ) { m_parent = obj; }
        inline Object3D* GetParent() const { return m_parent; }
    protected:
        Object3D*       m_parent;
        Matrix16        m_transform;
    };

    class DECLSPEC_NOVTABLE Camera3D
        : public Object3D
    {
    public:
        /**
         *  Set perspective matrix
         *
         *  @param fov: The vertical sight angle. If set to 0, use ortho matrix.
         *  @param width: Width of the screen.
         *  @param height: Height of the screen.
         *  @param near: The near plan. Faces closer than near will be cut off.
         *  @param far: The far plan. Faces behind far will be cut off.
         */
        VOID SetPerspective( FLOAT32 fov, FLOAT32 width, FLOAT32 height, FLOAT32 near, FLOAT32 far );
        inline FLOAT32 GetFOV() const { return m_fFOV; }
        inline FLOAT32 GetNear() const { return m_fNear; }
        inline FLOAT32 GetFar() const { return m_fFar; }
        inline FLOAT32 GetWidth() const { return m_fWidth; }
        inline FLOAT32 GetHeight() const { return m_fHeight; }

        VOID GetDirection( Vector3& dir ) const;
        VOID GetAbsoluteDirection( Vector3& dir ) const;

        VOID GetUpDirection( Vector3& up ) const;
        VOID GetAbsoluteUpDirection( Vector3& up ) const;

        VOID GetViewMatrix( Matrix16& mtrx ) const;
        VOID GetProjectionMatrix( Matrix16& mtrx ) const;

        VOID GetViewMatrixR( Matrix16& mtrx );
        VOID GetProjectionMatrixR( Matrix16& mtrx ) const;
    private:
        Matrix16        m_mtrxView;
        Matrix16        m_mtrxProj;
        Matrix16        m_mtrxViewR;
        Matrix16        m_mtrxProjR;
        FLOAT32         m_fFOV;
        FLOAT32         m_fWidth;
        FLOAT32         m_fHeight;
		FLOAT32         m_fNear;
        FLOAT32         m_fFar;
    };

    class DECLSPEC_NOVTABLE Light3D
        : public Object3D
    {
    public:
        Light3D();

        VOID GetDirection( Vector3& dir ) const;
        VOID GetAbsoluteDirection( Vector3& dir ) const;

        inline VOID SetDiffuse( ColorF_RGBA color ) { m_Diffuse = color; }
        inline ColorF_RGBA GetDiffuse() const { return m_Diffuse; }
        inline VOID SetSpecular( ColorF_RGBA color ) { m_Specular = color; }
        inline ColorF_RGBA GetSpecular() const { return m_Specular; }
        inline VOID SetAmbient( ColorF_RGBA color ) { m_Ambient = color; }
        inline ColorF_RGBA GetAmbient() const { return m_Ambient; }
        inline VOID SetRange( FLOAT32 range ) { m_Range = range; }
        inline FLOAT32 GetRange() const { return m_Range; }
    private:
        ColorF_RGBA     m_Diffuse;
        ColorF_RGBA     m_Specular;
        ColorF_RGBA     m_Ambient;
        FLOAT32         m_Range;
    };

    struct DECLSPEC_NOVTABLE Material
    {
        ColorF_RGBA diffuse;
        ColorF_RGBA ambient;
        ColorF_RGBA specular;
        ColorF_RGBA emissive;

        Material()
        {
            diffuse.r = diffuse.g = diffuse.b = diffuse.a = 1.f;
            ambient.r = ambient.g = ambient.b = ambient.a = 1.f;
            specular.r = specular.g = specular.b = specular.a = 0.f;
            emissive.r = emissive.g = emissive.b = emissive.a = 0.f;
        }
    };

    class IShader
    {
    public:
        virtual VOID SetTextureTransform( UINT32 stage, const Matrix16& mtrx ) = 0;
        virtual VOID SetWorldTransform( const Matrix16& mtrx ) = 0;
        virtual VOID SetViewTransform( const Matrix16& mtrx ) = 0;
        virtual VOID SetProjectionTransform( const Matrix16& mtrx ) = 0;
        virtual VOID CommitChanges( IGraphicContext* pGraphicContext ) = 0;
        virtual SHADER_PROGRAM_HANDLE GetProgram() = 0;
    };

    class Renderable3D
    {
        enum StatusFlag
        {
            VISIBLE = 0
            , SHAPE_VISIBLE
        };
    public:
        Renderable3D();
        virtual ~Renderable3D();

        virtual VOID Update( const RenderContext* context );
        virtual BOOL Cull( const Frustum& frustum );
        virtual VOID Render( IGraphicContext* pGraphicContext, const RenderContext* context, const Matrix16 &viewMtrx, const Matrix16 &projMtrx ) = 0;
        virtual VOID DrawShape( IGraphicContext* pGraphicContext, IShader* pShader );

        inline VOID SetVisible( BOOL bVisible ) { m_dwStatusFlag &= ~(0x1 << VISIBLE); m_dwStatusFlag |= ((bVisible != FALSE) << VISIBLE); };
        inline BOOL IsVisible() const { return ((0x1 << VISIBLE) & m_dwStatusFlag) != FALSE; }

        inline VOID SetShapeVisible( BOOL bVisible ) { m_dwStatusFlag &= ~(0x1 << SHAPE_VISIBLE); m_dwStatusFlag |= ((bVisible != FALSE) << SHAPE_VISIBLE); };
        inline BOOL IsShapeVisible() const { return ((0x1 << SHAPE_VISIBLE) & m_dwStatusFlag) != FALSE; }
    private:
        DWORD m_dwStatusFlag;
    };

    class Geometry3D
        : public Renderable3D
        , public Object3D
    {
    };

    class GeometryGroup3D
        : public Renderable3D
    {
    public:
        virtual VOID Update( const RenderContext* context );
        virtual BOOL Cull( const Frustum& frustum );
        virtual VOID Render( IGraphicContext* pGraphicContext, const RenderContext* context, const Matrix16 &viewMtrx, const Matrix16 &projMtrx );
        virtual VOID DrawShape( IGraphicContext* pGraphicContext, IShader* pShader );

        inline VOID AddGeometryFront( Renderable3D* pGeometry ) { m_objects.push_front( pGeometry ); }
        inline VOID AddGeometryBack( Renderable3D* pGeometry ) { m_objects.push_back( pGeometry ); }
        inline VOID RemoveGeometry( Renderable3D* pGeometry ) { m_objects.remove( pGeometry ); }
        inline UINT32 Count() const { return (UINT32)m_objects.size(); }
        
        inline VOID Clear() { m_objects.clear(); }
    protected:
        virtual VOID RenderStart( IGraphicContext* pGraphicContext, const RenderContext* context );
        virtual VOID RenderEnd( IGraphicContext* pGraphicContext, const RenderContext* context );
    protected:
        std::list<Renderable3D*>    m_objects;
        std::list<Renderable3D*>    m_drawList;
    };

    class IShapeRender
    {
    public:
        virtual VOID PrepareGraphicContextForShape( IGraphicContext* pGraphicContext, Renderable3D* pRenderable ) = 0;
        virtual IShader* GetShader() = 0;
    };

    class Scene3D
        : public GraphicLayer
    {
    public:
        Scene3D();
        ~Scene3D();

        inline VOID SetCamera( Camera3D* pCamera ) { m_pCamera = pCamera; }
        inline Camera3D* GetCamera() const { return m_pCamera; }

        inline VOID AddGeometryFront( Renderable3D* pGeometry ) { m_objects.push_front( pGeometry ); }
        inline VOID AddGeometryBack( Renderable3D* pGeometry ) { m_objects.push_back( pGeometry ); }
        inline VOID RemoveGeometry( Renderable3D* pGeometry ) { m_objects.remove( pGeometry ); }

        inline VOID Clear() { m_objects.clear(); }

        VOID DrawShapes( IGraphicContext* pGraphicContext, IShapeRender* pRender );
    protected:
        virtual VOID RenderContent( IGraphicContext* pGraphicContext, const RenderContext* context );
    private:
        VOID FrustumCull();

        std::list<Renderable3D*>        m_objects;
        std::list<Renderable3D*>        m_drawList;
        Camera3D*                       m_pCamera;
    };

    class Render2D
    {
    public:
        Render2D();

        VOID DrawTexture( const Box2D& boxTarget, const TextureInfo &texInfo );
        VOID DrawTexture( const Matrix16& transform, const TextureInfo &texInfo );

        VOID AddMask( const Box2D& boxMask );
        VOID ClearMask( const Box2D& boxMask );
        VOID ClearMask();

        BOOL RenderStart( IGraphicContext* pGraphicContext );
        VOID RenderEnd();

        VOID SetClipRect( const Box2D& clipRect );
        inline VOID GetClipRect( Box2D& clipRect ) const { clipRect = m_clipRect; }
        VOID SetShader( IShader* pShader );

        inline IShader* GetShader() const { return m_pShader; }
        inline IGraphicContext* GetGraphicContext() const { return m_pGraphicContext; }
        inline VOID SetResController( UtilGraphicResourceController* resController ) { m_resController = resController; }

        VOID PercentCoord2PixelCoord( const Vector2 pt, Vector2 &out );
        VOID PixelCoord2PercentCoord( const Vector2 pt, Vector2 &out );

        static const Box2D DEFAULT_CLIP_RECT;
    private:
        UtilGraphicResourceController*      m_resController;
        IGraphicContext*                    m_pGraphicContext;
        IShader*                            m_pShader;
		Box2D								m_clipRect;
    };

    class Geometry2D
    {
    public:
        enum StatusFlag
        {
            VISIBLE = 0
            , PICKABLE
        };

        Geometry2D();

        virtual VOID Render( Render2D* render, const RenderContext* context ) = 0;
        
        inline VOID SetVisible( BOOL bVisible ) { m_dwStatusFlag &= ~(0x1 << VISIBLE); m_dwStatusFlag |= ((bVisible != FALSE) << VISIBLE); };
        inline BOOL IsVisible() const { return ((0x1 << VISIBLE) & m_dwStatusFlag) != FALSE; }

        inline VOID SetPickable( BOOL bPickable ) { m_dwStatusFlag &= ~(0x1 << PICKABLE); m_dwStatusFlag |= ((bPickable != FALSE) << PICKABLE); };
        inline BOOL IsPickable() const { return ((0x1 << PICKABLE) & m_dwStatusFlag) != FALSE; }
    private:
        DWORD m_dwStatusFlag;
    };

    class GeometryGroup2D
        : public Geometry2D
    {
    public:
        GeometryGroup2D();

        virtual VOID Render( Render2D* render, const RenderContext* context );

        inline VOID AddGeometry( Geometry2D* pGeometry ) { m_geometries.push_back( pGeometry ); }
        inline VOID RemoveGeometry( Geometry2D* pGeometry ) { m_geometries.remove( pGeometry ); }
    private:
        std::list<Geometry2D*>          m_geometries;
    };

    class Scene2D
        : public GraphicLayer
    {
    public:
        Scene2D();
        ~Scene2D();

        inline VOID AddTop( Geometry2D* obj ) { m_2DObjs.remove( obj ); m_2DObjs.push_front( obj ); }
        inline VOID AddBack( Geometry2D* obj ) { m_2DObjs.remove( obj ); m_2DObjs.push_back( obj ); }
        inline VOID RemoveChild( Geometry2D* obj ) { m_2DObjs.remove( obj ); }
        inline VOID Clear() { m_2DObjs.clear(); }

    protected:
        virtual VOID RenderContent( IGraphicContext* pGraphicContext, const RenderContext* context );

    private:
        std::list<Geometry2D*>    m_2DObjs;
        Render2D                  m_Render2D;
    };

    class LayerCollection
        : public GraphicLayer
    {
    public:
        LayerCollection();
        ~LayerCollection();

        VOID AddLayerTop( GraphicLayer* pLayer );
        VOID AddLayerBottom( GraphicLayer* pLayer );
        VOID RemoveLayer( GraphicLayer* pLayer );
        VOID ClearLayer();

        virtual VOID SetResController( UtilGraphicResourceController* resController );
    protected:
        virtual VOID RenderContent( IGraphicContext* pGraphicContext, const RenderContext* context );

    private:
        std::list<GraphicLayer*>        m_Layers;
    };

    class DECLSPEC_NOVTABLE GraphicManager
    {
    public:
        GraphicManager();
        ~GraphicManager();

        BOOL Initialize();

        VOID AddFilter( GraphicFilter* filter );
        VOID RemoveFilter( GraphicFilter* filter );
        VOID ClearFilter();

        VOID AddLayerTop( GraphicLayer* pLayer );
        VOID AddLayerBottom( GraphicLayer* pLayer );
        VOID RemoveLayer( GraphicLayer* pLayer );
        VOID ClearLayer();

        VOID Render( IGraphicContext* pGraphicContext, UINT32 timeInterval );

        inline VOID SetResController( UtilGraphicResourceController* resController )
            { m_LayerCollection.SetResController( resController ); }
        inline UtilGraphicResourceController* GetResController() const { return m_LayerCollection.GetResController(); }
    private:
        LayerCollection   m_LayerCollection;
        RenderContext     m_RenderContext;
    };
}

#endif
