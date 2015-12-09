#include "GraphicTypes.h"
#include "GraphicMath.h"
#include "GraphicResourceMgr.h"
#include "GraphicCommonDefs.h"
#include "IGraphicContext.h"

namespace Graphic
{
    GraphicFilter::GraphicFilter()
    {
    }

    GraphicFilterContainer::GraphicFilterContainer()
        : m_resController( NULL )
        , m_uCurrentBuffer( 0 )
        , m_originTarget( NULL )
    {
        memset( m_uTexIndex, 0, sizeof(m_uTexIndex) );
    }

    GraphicFilterContainer::~GraphicFilterContainer()
    {
    }

    BOOL GraphicFilterContainer::FilterStart( IGraphicContext* pGraphicContext, const RenderContext* )
    {
        if ( !m_filterList.empty() )
        {
            m_originTarget = pGraphicContext->GetRenderTarget();
            // do not save the texture handle, because texture is managered by rescontroller, it may be changed with out
            // any notifycation, just save the index and get it when need it. (not costly)
            if ( !m_resController->GetUnengagedRenderTargetID( m_uTexIndex, 2 ) )
            {
                return FALSE;
            }
            for ( UINT32 i = 0; i < 2; ++i )
            {
                m_swapRenderTarget[i] = m_resController->GetScreenRenderTarget( m_uTexIndex[i] );
            }
            m_uCurrentBuffer = ( ( m_originTarget == m_swapRenderTarget[0] ) & ( m_filterList.size() & 1 ) );
            pGraphicContext->SetRenderTarget( m_swapRenderTarget[m_uCurrentBuffer] );
            pGraphicContext->GetViewPort( m_originViewport );
            Graphic::ViewPort viewPort = {0};
            m_resController->GetScreenTextureSize( viewPort.uWidth, viewPort.uHeight );
            pGraphicContext->SetViewPort( viewPort );
            ColorF_RGBA clearColor = {0};
            pGraphicContext->Clear( IGraphicContext::Clear_Color | IGraphicContext::Clear_Depth, clearColor, 1.f, 0 );
        }
        return TRUE;
    }

    VOID GraphicFilterContainer::FilterEnd( IGraphicContext* pGraphicContext, const RenderContext* context )
    {
        if ( m_originTarget != NULL )
        {
            ColorF_RGBA clearColor = {0};
            ASSERT( !m_filterList.empty() );
            std::list<GraphicFilter*>::reverse_iterator iter = m_filterList.rbegin();
            std::list<GraphicFilter*>::reverse_iterator iterPrev = iter;
            iter ++;
            std::list<GraphicFilter*>::reverse_iterator iterEnd = m_filterList.rend();
            while ( iter != iterEnd )
            {
                GraphicFilter* pFilter = *iterPrev;
                pGraphicContext->SetRenderTarget( m_swapRenderTarget[!m_uCurrentBuffer] );
                pGraphicContext->Clear( IGraphicContext::Clear_Color | IGraphicContext::Clear_Depth, clearColor, 1.f, 0 );
                pFilter->Render( pGraphicContext, context, m_resController->GetScreenTexture( m_uTexIndex[m_uCurrentBuffer] ) );
                m_uCurrentBuffer = !m_uCurrentBuffer;
                iterPrev = iter;
                iter++;
            }
            pGraphicContext->SetRenderTarget( m_originTarget );
            pGraphicContext->SetViewPort( m_originViewport );
            pGraphicContext->Clear( IGraphicContext::Clear_Color | IGraphicContext::Clear_Depth, clearColor, 1.f, 0 );
            (*iterPrev)->Render( pGraphicContext, context, m_resController->GetScreenTexture( m_uTexIndex[m_uCurrentBuffer] ) );
            m_originTarget = NULL;
        }
    }

    VOID GraphicFilterContainer::AddFilter( GraphicFilter* filter )
    {
        m_filterList.push_back( filter );
    }

    VOID GraphicFilterContainer::RemoveFilter( GraphicFilter* filter )
    {
        m_filterList.remove( filter );
    }

    VOID GraphicFilterContainer::ClearFilter()
    {
        m_filterList.clear();
    }

    BOOL GraphicFilterContainer::EngageCurrentTarget()
    {
        if ( m_filterList.empty() )
        {
            return TRUE;
        }
        return m_resController->EngageARenderTargetID( m_uTexIndex[m_uCurrentBuffer] );
    }

    VOID GraphicFilterContainer::FreeCurrentTarget()
    {
        if ( !m_filterList.empty() )
        {
            m_resController->FreeARenderTargetID( m_uTexIndex[m_uCurrentBuffer] );
        }
    }

    GraphicLayer::GraphicLayer()
        : m_FilterContainer()
        , m_pParent( NULL )
    {
    }

    GraphicLayer::~GraphicLayer()
    {
    }

    VOID GraphicLayer::Render( IGraphicContext* pGraphicContext, const RenderContext* context )
    {
        if ( m_FilterContainer.FilterStart( pGraphicContext, context ) )
        {
            RenderContent( pGraphicContext, context );
            m_FilterContainer.FilterEnd( pGraphicContext, context );
        }
    }

    Object3D::Object3D()
        : m_parent( NULL )
    {
        MatrixIdentity( &m_transform );
    }

    VOID Object3D::ResetTransform()
    {
        MatrixIdentity( &m_transform );
    }

    VOID Object3D::Translate( const Vector3& offset )
    {
        Matrix16 mtrx;
        MatrixTranslate( &mtrx, offset.x, offset.y, offset.z );
        MatrixMultiply( &m_transform, &m_transform, &mtrx );
    }

    VOID Object3D::Rotate( const Vector3& axis, FLOAT32 angle )
    {
        Matrix16 mtrx;
        MatrixRotate( &mtrx, &axis, angle );
        MatrixMultiply( &m_transform, &m_transform, &mtrx );
    }

    VOID Object3D::RotateX( FLOAT32 angle )
    {
        Matrix16 mtrx;
        MatrixRotateX( &mtrx, angle );
        MatrixMultiply( &m_transform, &m_transform, &mtrx );
    }

    VOID Object3D::RotateY( FLOAT32 angle )
    {
        Matrix16 mtrx;
        MatrixRotateY( &mtrx, angle );
        MatrixMultiply( &m_transform, &m_transform, &mtrx );
    }

    VOID Object3D::RotateZ( FLOAT32 angle )
    {
        Matrix16 mtrx;
        MatrixRotateZ( &mtrx, angle );
        MatrixMultiply( &m_transform, &m_transform, &mtrx );
    }

    VOID Object3D::Scale( FLOAT32 sx, FLOAT32 sy, FLOAT32 sz )
    {
        Matrix16 mtrx;
        MatrixScale( &mtrx, sx, sy, sz );
        MatrixMultiply( &m_transform, &m_transform, &mtrx );
    }

    VOID Object3D::GetPosition( Vector3& pos ) const
    {
        memset( &pos, 0, sizeof(Vector3) );
        Point3Transform( &pos, &pos, &m_transform );
    }

    BOOL Object3D::IsChildOf( Object3D* pObj ) const
    {
        Object3D* parent = m_parent;
        while ( parent != NULL )
        {
            if ( pObj == parent )
            {
                return TRUE;
            }
            parent = parent->m_parent;
        }
        return FALSE;
    }

    VOID Object3D::GetAbsoluteTransform( Matrix16& mtrx ) const
    {
        mtrx = m_transform;
        Object3D* parent = m_parent;
        while ( parent != NULL )
        {
            MatrixMultiply( &mtrx, &mtrx, &parent->m_transform );
            parent = parent->m_parent;
        }
    }

    VOID Object3D::GetAbsolutePosition( Vector3& pos ) const
    {
        Matrix16 mtrxTransform;
        memset( &pos, 0, sizeof(Vector3) );
        GetAbsoluteTransform( mtrxTransform );
        Point3Transform( &pos, &pos, &mtrxTransform );
    }

    VOID Object3D::GetAbsolutePosition( Vector2& pos ) const
    {
        Vector3 tmpPos;
        GetAbsolutePosition(tmpPos);
        pos.x = tmpPos.x;
        pos.y = tmpPos.y;
    }

    VOID Camera3D::SetPerspective( FLOAT32 fov, FLOAT32 width, FLOAT32 height, FLOAT32 n, FLOAT32 f )
    {
        m_fFOV = fov;
        m_fWidth = width;
        m_fHeight = height;
		m_fNear = n;
        m_fFar = f;
        if ( m_fFOV > 0.f )
        {
            MatrixPerspective( &m_mtrxProj, m_fFOV, m_fWidth / m_fHeight, m_fNear, m_fFar );
        }
        else
        {
            MatrixOrtho( &m_mtrxProj, m_fWidth, m_fHeight, m_fNear, m_fFar );
        }
        MatrixInverse( &m_mtrxProjR, &m_mtrxProj );
    }

    VOID Camera3D::GetDirection( Vector3& dir ) const
    {
        memset( &dir, 0, sizeof(Vector3) );
        dir.z = -1.f;
        Vector3Transform( &dir, &dir, &m_transform );
    }

    VOID Camera3D::GetAbsoluteDirection( Vector3& dir ) const
    {
        Matrix16 mtrxTransform;
        memset( &dir, 0, sizeof(Vector3) );
        dir.z = -1.f;
        GetAbsoluteTransform( mtrxTransform );
        Vector3Transform( &dir, &dir, &mtrxTransform );
    }

    VOID Camera3D::GetUpDirection( Vector3& up ) const
    {
        memset( &up, 0, sizeof(Vector3) );
        up.y = 1.f;
        Vector3Transform( &up, &up, &m_transform );
    }

    VOID Camera3D::GetAbsoluteUpDirection( Vector3& up ) const
    {
        Matrix16 mtrxTransform;
        memset( &up, 0, sizeof(Vector3) );
        up.y = 1.f;
        GetAbsoluteTransform( mtrxTransform );
        Vector3Transform( &up, &up, &mtrxTransform );
    }

    VOID Camera3D::GetViewMatrix( Matrix16& mtrx ) const
    {
        Vector3 eye, lookAt, up;
        memset( &eye, 0, sizeof(Vector3) );
        memset( &lookAt, 0, sizeof(Vector3) );
        memset( &up, 0, sizeof(Vector3) );
        GetAbsoluteTransform( mtrx );

        Point3Transform( &eye, &eye, &mtrx );
        lookAt.z = -1.f;
        Vector3Transform( &lookAt, &lookAt, &mtrx );
        Vector3Add( &lookAt, &lookAt, &eye );
        up.y = 1.f;
        Vector3Transform( &up, &up, &mtrx );
        MatrixLookAt( &mtrx, &eye, &lookAt, &up );
    }

    VOID Camera3D::GetProjectionMatrix( Matrix16& mtrx ) const
    {
        mtrx = m_mtrxProj;
    }

    VOID Camera3D::GetViewMatrixR( Matrix16& mtrx )
    {
        Matrix16 mtrxView;
        GetViewMatrix( mtrxView );
        if ( ::memcmp( &m_mtrxView, &mtrxView, sizeof( Matrix16 ) ) )
        {
            m_mtrxView = mtrxView;
            MatrixInverse( &m_mtrxViewR, &mtrxView );
        }
        mtrx = m_mtrxViewR;
    }

    VOID Camera3D::GetProjectionMatrixR( Matrix16& mtrx ) const
    {
        mtrx = m_mtrxProjR;
    }

    Light3D::Light3D()
    {
        m_Diffuse.r = m_Diffuse.g = m_Diffuse.b = m_Diffuse.a = 1.f;
        m_Specular.r = m_Specular.g = m_Specular.b = m_Specular.a = 1.f;
        m_Ambient.r = m_Ambient.g = m_Ambient.b = m_Ambient.a = 1.f;
        m_Range = 1000.f;
    }

    VOID Light3D::GetDirection( Vector3& dir ) const
    {
        memset( &dir, 0, sizeof(Vector3) );
        dir.z = -1.f;
        Vector3Transform( &dir, &dir, &m_transform );
    }

    VOID Light3D::GetAbsoluteDirection( Vector3& dir ) const
    {
        Matrix16 mtrxTransform;
        memset( &dir, 0, sizeof(Vector3) );
        dir.z = -1.f;
        GetAbsoluteTransform( mtrxTransform );
        Vector3Transform( &dir, &dir, &mtrxTransform );
    }

    Renderable3D::Renderable3D()
        : m_dwStatusFlag( 0 )
    {
        SetVisible( TRUE );
    }

    Renderable3D::~Renderable3D()
    {
    }

    VOID Renderable3D::DrawShape( IGraphicContext* pGraphicContext, IShader* pShader )
    {
    }

    VOID Renderable3D::Update( const RenderContext* context )
    {
    }

    BOOL Renderable3D::Cull( const Frustum& frustum )
    {
        return IsVisible();
    }


    VOID GeometryGroup3D::Update( const RenderContext* context )
    {
        std::list<Renderable3D*>::const_iterator iterObj = m_objects.begin();
        std::list<Renderable3D*>::const_iterator iterObjEnd = m_objects.end();
        while ( iterObj != iterObjEnd )
        {
            Renderable3D* pRenderable = *iterObj;
            pRenderable->Update( context );
            iterObj ++;
        }
    }

    BOOL GeometryGroup3D::Cull( const Frustum& frustum )
    {
        // update draw list
        m_drawList.clear();
        if ( IsVisible() )
        {
            std::list<Renderable3D*>::const_iterator iterObj = m_objects.begin();
            std::list<Renderable3D*>::const_iterator iterObjEnd = m_objects.end();
            while ( iterObj != iterObjEnd )
            {
                Renderable3D* pRenderable = *iterObj;
                // cull objects if bounding box is not in frustum
                if ( pRenderable->Cull( frustum ) )
                {
                    m_drawList.push_back( pRenderable );
                }
                iterObj ++;
            }
        }
        return !m_drawList.empty();
    }

    VOID GeometryGroup3D::Render( IGraphicContext* pGraphicContext, const RenderContext* context, const Matrix16 &viewMtrx, const Matrix16 &projMtrx )
    {
        RenderStart( pGraphicContext, context );
        std::list<Renderable3D*>::const_iterator iterDraw = m_drawList.begin();
        std::list<Renderable3D*>::const_iterator iterDrawEnd = m_drawList.end();
        while ( iterDraw != iterDrawEnd )
        {
            Renderable3D* pRenderable = *iterDraw;
            pRenderable->Render( pGraphicContext, context, viewMtrx, projMtrx );
            iterDraw ++;
        }
        RenderEnd( pGraphicContext, context );
    }

    VOID GeometryGroup3D::DrawShape( IGraphicContext* pGraphicContext, IShader* pShader )
    {
        std::list<Renderable3D*>::const_iterator iterDraw = m_drawList.begin();
        std::list<Renderable3D*>::const_iterator iterDrawEnd = m_drawList.end();
        while ( iterDraw != iterDrawEnd )
        {
            Renderable3D* pRenderable = *iterDraw;
            pRenderable->DrawShape( pGraphicContext, pShader );
            iterDraw ++;
        }
    }

    VOID GeometryGroup3D::RenderStart( IGraphicContext*, const RenderContext* )
    {
    }

    VOID GeometryGroup3D::RenderEnd( IGraphicContext*, const RenderContext* )
    {
    }

    Scene3D::Scene3D()
        : m_objects()
        , m_drawList()
        , m_pCamera( NULL )
    {
    }

    Scene3D::~Scene3D()
    {
    }

    VOID Scene3D::DrawShapes( IGraphicContext* pGraphicContext, IShapeRender* pRender )
    {
        if ( m_pCamera != NULL && pGraphicContext->BeginScene() )
        {
            pGraphicContext->EnableDepthTest( TRUE );
            pGraphicContext->EnableCullFace( TRUE );
            pGraphicContext->SetCullMode( IGraphicContext::CullMode_ClockWise );
            // render all objects in draw list
            Matrix16 viewMtrx, projMtrx;
            m_pCamera->GetViewMatrix( viewMtrx );
            m_pCamera->GetProjectionMatrix( projMtrx );
            std::list<Renderable3D*>::const_iterator iterDraw = m_drawList.begin();
            std::list<Renderable3D*>::const_iterator iterDrawEnd = m_drawList.end();
            while ( iterDraw != iterDrawEnd )
            {
                Renderable3D* pRenderable = *iterDraw;
                if ( pRenderable->IsShapeVisible() )
                {
                    pRender->PrepareGraphicContextForShape( pGraphicContext, pRenderable );
                    pRenderable->DrawShape( pGraphicContext, pRender->GetShader() );
                }
                iterDraw ++;
            }
            pGraphicContext->EndScene();
        }
    }

    VOID Scene3D::RenderContent( IGraphicContext* pGraphicContext, const RenderContext* context )
    {
        if ( m_pCamera != NULL && pGraphicContext->BeginScene() )
        {
            pGraphicContext->EnableDepthTest( TRUE );
            pGraphicContext->EnableCullFace( TRUE );
            pGraphicContext->SetCullMode( IGraphicContext::CullMode_ClockWise );
            // update all objects
            std::list<Renderable3D*>::const_iterator iterUpdate = m_objects.begin();
            std::list<Renderable3D*>::const_iterator iterUpdateEnd = m_objects.end();
            while ( iterUpdate != iterUpdateEnd )
            {
                Renderable3D* pRenderable = *iterUpdate;
                pRenderable->Update( context );
                iterUpdate ++;
            }
            FrustumCull();
            // render all objects in draw list
            Matrix16 viewMtrx, projMtrx;
            m_pCamera->GetViewMatrix( viewMtrx );
            m_pCamera->GetProjectionMatrix( projMtrx );
            std::list<Renderable3D*>::const_iterator iterDraw = m_drawList.begin();
            std::list<Renderable3D*>::const_iterator iterDrawEnd = m_drawList.end();
            while ( iterDraw != iterDrawEnd )
            {
                Renderable3D* pRenderable = *iterDraw;
                pRenderable->Render( pGraphicContext, context, viewMtrx, projMtrx );
                iterDraw ++;
            }
            pGraphicContext->EndScene();
        }
    }

    VOID Scene3D::FrustumCull()
    {
        // calculate frustum
        Frustum frustum;
        Matrix16 mtrxViewR, mtrxProjR, mtrx;
        m_pCamera->GetViewMatrixR( mtrxViewR );
        m_pCamera->GetProjectionMatrixR( mtrxProjR );
        MatrixMultiply( &mtrx, &mtrxProjR, &mtrxViewR );
        /*
                     p[6]-----------p[7]
                     /|             /|
				    / |            / |
			      p[2]-----------p[3]|
				   |  |           |  |
				   |  |           |  |
				   |  |           |  |
				   | p[4]---------|-p[5]
				   | /            | /
				   |/             |/
				  p[0]-----------p[1]
        */
        enum POSITION
        {
            FRONT = 0
            , RIGHT
            , LEFT
            , TOP
            , BOTTOM
            , BACK
        };
        frustum.vertex[0].x = frustum.vertex[2].x = frustum.vertex[4].x = frustum.vertex[6].x = SCREEN_COORD_X_MIN;
        frustum.vertex[1].x = frustum.vertex[3].x = frustum.vertex[5].x = frustum.vertex[7].x = SCREEN_COORD_X_MAX;
        frustum.vertex[0].y = frustum.vertex[1].y = frustum.vertex[4].y = frustum.vertex[5].y = SCREEN_COORD_Y_MIN;
        frustum.vertex[2].y = frustum.vertex[3].y = frustum.vertex[6].y = frustum.vertex[7].y = SCREEN_COORD_Y_MAX;
        frustum.vertex[0].z = frustum.vertex[1].z = frustum.vertex[2].z = frustum.vertex[3].z = SCREEN_COORD_Z_MIN;
        frustum.vertex[4].z = frustum.vertex[5].z = frustum.vertex[6].z = frustum.vertex[7].z = SCREEN_COORD_Z_MAX;
        
        for ( UINT32 i = 0; i < 8; ++i )
        {
            Point3Transform( &frustum.vertex[i], &frustum.vertex[i], &mtrx );
        }

        Graphic::Vector3 vec1;
        Graphic::Vector3 vec2;
        Graphic::Vector3Subtract( &vec1, &frustum.vertex[1], &frustum.vertex[0] );
        Graphic::Vector3Subtract( &vec2, &frustum.vertex[2], &frustum.vertex[0] );
        Graphic::Vector3Cross( &frustum.normal[FRONT], &vec1, &vec2 );

        Graphic::Vector3Subtract( &vec1, &frustum.vertex[5], &frustum.vertex[1] );
        Graphic::Vector3Subtract( &vec2, &frustum.vertex[3], &frustum.vertex[1] );
        Graphic::Vector3Cross( &frustum.normal[RIGHT], &vec1, &vec2 );

        Graphic::Vector3Subtract( &vec1, &frustum.vertex[2], &frustum.vertex[0] );
        Graphic::Vector3Subtract( &vec2, &frustum.vertex[4], &frustum.vertex[0] );
        Graphic::Vector3Cross( &frustum.normal[LEFT], &vec1, &vec2 );

        Graphic::Vector3Subtract( &vec1, &frustum.vertex[3], &frustum.vertex[2] );
        Graphic::Vector3Subtract( &vec2, &frustum.vertex[6], &frustum.vertex[2] );
        Graphic::Vector3Cross( &frustum.normal[TOP], &vec1, &vec2 );

        Graphic::Vector3Subtract( &vec1, &frustum.vertex[4], &frustum.vertex[0] );
        Graphic::Vector3Subtract( &vec2, &frustum.vertex[1], &frustum.vertex[0] );
        Graphic::Vector3Cross( &frustum.normal[BOTTOM], &vec1, &vec2 );

        Graphic::Vector3Subtract( &vec1, &frustum.vertex[6], &frustum.vertex[4] );
        Graphic::Vector3Subtract( &vec2, &frustum.vertex[5], &frustum.vertex[4] );
        Graphic::Vector3Cross( &frustum.normal[BACK], &vec1, &vec2 );

        // update draw list
        m_drawList.clear();
        std::list<Renderable3D*>::const_iterator iterObj = m_objects.begin();
        std::list<Renderable3D*>::const_iterator iterObjEnd = m_objects.end();
        while ( iterObj != iterObjEnd )
        {
            Renderable3D* pRenderable = *iterObj;
            // cull objects if bounding box is not in frustum
            if ( pRenderable->Cull( frustum ) )
            {
                m_drawList.push_back( pRenderable );
            }
            iterObj ++;
        }
    }

    LayerCollection::LayerCollection()
    {
    }

    LayerCollection::~LayerCollection()
    {
    }

    VOID LayerCollection::RenderContent( IGraphicContext* pGraphicContext, const RenderContext* context )
    {
        if ( m_pParent == NULL || m_Layers.size() < 2 )
        {
            std::list<GraphicLayer*>::reverse_iterator iter = m_Layers.rbegin();
            std::list<GraphicLayer*>::reverse_iterator iterEnd = m_Layers.rend();
            while ( iter != iterEnd )
            {
                (*iter)->Render( pGraphicContext, context );
                iter ++;
            }
        }
        else
        {
            std::list<GraphicLayer*>::reverse_iterator iter = m_Layers.rbegin();
            std::list<GraphicLayer*>::reverse_iterator iterEnd = m_Layers.rend();
            (*iter)->Render( pGraphicContext, context );
            if ( !m_pParent->m_FilterContainer.EngageCurrentTarget() )
            {
                return;
            }
            iter ++;
            while ( iter != iterEnd )
            {
                (*iter)->Render( pGraphicContext, context );
                iter ++;
            }
            m_pParent->m_FilterContainer.FreeCurrentTarget();
        }
    }

    VOID LayerCollection::AddLayerTop( GraphicLayer* pLayer )
    {
        if ( pLayer != NULL )
        {
            m_Layers.push_front( pLayer );
            pLayer->m_pParent = this;
        }
    }

    VOID LayerCollection::AddLayerBottom( GraphicLayer* pLayer )
    {
        if ( pLayer != NULL )
        {
            m_Layers.push_back( pLayer );
            pLayer->m_pParent = this;
        }
    }

    VOID LayerCollection::RemoveLayer( GraphicLayer* pLayer )
    {
        if ( pLayer != NULL && pLayer->m_pParent == this )
        {
            m_Layers.remove( pLayer );
            pLayer->m_pParent = NULL;
        }
    }

    VOID LayerCollection::ClearLayer()
    {
        std::list<GraphicLayer*>::iterator iter = m_Layers.begin();
        std::list<GraphicLayer*>::iterator iterEnd = m_Layers.end();
        while ( iter != iterEnd )
        {
            (*iter)->m_pParent = NULL;
            iter ++;
        }
        m_Layers.clear();
    }

    VOID LayerCollection::SetResController( UtilGraphicResourceController* resController )
    {
        m_FilterContainer.SetResController( resController );
        std::list<GraphicLayer*>::iterator iter = m_Layers.begin();
        std::list<GraphicLayer*>::iterator iterEnd = m_Layers.end();
        while ( iter != iterEnd )
        {
            (*iter)->SetResController( resController );
            iter ++;
        }
    }

    const Box2D Render2D::DEFAULT_CLIP_RECT = { Vector2( -1.f, -1.f ), Vector2( 2.f, 2.f ) };

    Render2D::Render2D()
        : m_resController( NULL )
        , m_pGraphicContext( NULL )
        , m_pShader( NULL )
    {
        m_clipRect = DEFAULT_CLIP_RECT;
    }

    VOID Render2D::DrawTexture( const Box2D& boxTarget, const TextureInfo &texInfo )
    {
        Box2D target;
        if ( Box2D::Intersect( m_clipRect, boxTarget, target ) )
        {
            Matrix16 mtrx;
            memset( &mtrx, 0, sizeof(Matrix16) );
            mtrx.m[0][0] = target.size.x * texInfo.texCoord.size.x / boxTarget.size.x;
            mtrx.m[1][1] = target.size.y * texInfo.texCoord.size.y / boxTarget.size.y;
            mtrx.m[3][0] = texInfo.texCoord.origin.x + ( target.origin.x - boxTarget.origin.x ) * texInfo.texCoord.size.x / boxTarget.size.x;
            mtrx.m[3][1] = texInfo.texCoord.origin.y + ( target.origin.y - boxTarget.origin.y ) * texInfo.texCoord.size.y / boxTarget.size.y;
            mtrx.m[2][2] = 1.f;
            mtrx.m[3][3] = 1.f;
            m_pShader->SetTextureTransform( 0, mtrx );

            memset( &mtrx, 0, sizeof(Matrix16) );
            mtrx.m[0][0] = target.size.x / 2.f;
            mtrx.m[1][1] = target.size.y / 2.f;
            mtrx.m[3][0] = target.origin.x + mtrx.m[0][0];
            mtrx.m[3][1] = target.origin.y + mtrx.m[1][1];
            mtrx.m[2][2] = 1.f;
            mtrx.m[3][3] = 1.f;

            m_pShader->SetWorldTransform( mtrx );
            m_pShader->CommitChanges( m_pGraphicContext );

            m_pGraphicContext->SetTexture( 0, texInfo.texture );

            m_pGraphicContext->DrawPrimitive( IGraphicContext::Primitive_TriagnleStrip, 0, 4, 2 );
        }
    }

    VOID Render2D::DrawTexture( const Matrix16& transform, const TextureInfo &texInfo )
    {
        Matrix16 mtrx;
        memset( &mtrx, 0, sizeof(Matrix16) );
        mtrx.m[0][0] = texInfo.texCoord.size.x;
        mtrx.m[1][1] = texInfo.texCoord.size.y;
        mtrx.m[3][0] = texInfo.texCoord.origin.x;
        mtrx.m[3][1] = texInfo.texCoord.origin.y;
        mtrx.m[2][2] = 1.f;
        mtrx.m[3][3] = 1.f;
        m_pShader->SetTextureTransform( 0, mtrx );
        m_pShader->SetWorldTransform( transform );
        m_pShader->CommitChanges( m_pGraphicContext );

        m_pGraphicContext->SetTexture( 0, texInfo.texture );

        m_pGraphicContext->DrawPrimitive( IGraphicContext::Primitive_TriagnleStrip, 0, 4, 2 );
    }

    VOID Render2D::AddMask( const Box2D& boxMask )
    {
        Matrix16 mtrx;
        memset( &mtrx, 0, sizeof(Matrix16) );
        mtrx.m[0][0] = boxMask.size.x;
        mtrx.m[1][1] = boxMask.size.y;
        mtrx.m[0][3] = -boxMask.origin.x;
        mtrx.m[1][3] = -boxMask.origin.y;
        mtrx.m[2][2] = 1.f;
        mtrx.m[3][3] = 1.f;

        m_pShader->SetWorldTransform( mtrx );
        m_pShader->CommitChanges( m_pGraphicContext );

        m_pGraphicContext->ColorWriteEnable( FALSE, FALSE, FALSE, FALSE );
        m_pGraphicContext->SetStencilWriteMask( 0x1 );
        m_pGraphicContext->SetStencilTestFunc( IGraphicContext::StencilTest_Always, 1, 0x1 );
        m_pGraphicContext->SetStencilOperation( IGraphicContext::StencilOp_Keep, IGraphicContext::StencilOp_Keep, IGraphicContext::StencilOp_Replace );

        m_pGraphicContext->DrawPrimitive( IGraphicContext::Primitive_TriagnleStrip, 0, 4, 2 );

        m_pGraphicContext->ColorWriteEnable( TRUE, TRUE, TRUE, TRUE );
        m_pGraphicContext->SetStencilTestFunc( IGraphicContext::StencilTest_Greater, 0, 0x1 );
        m_pGraphicContext->SetStencilOperation( IGraphicContext::StencilOp_Keep, IGraphicContext::StencilOp_Keep, IGraphicContext::StencilOp_Keep );
    }

    VOID Render2D::ClearMask( const Box2D& boxMask )
    {
        Matrix16 mtrx;
        memset( &mtrx, 0, sizeof(Matrix16) );
        mtrx.m[0][0] = boxMask.size.x;
        mtrx.m[1][1] = boxMask.size.y;
        mtrx.m[0][3] = -boxMask.origin.x;
        mtrx.m[1][3] = -boxMask.origin.y;
        mtrx.m[2][2] = 1.f;
        mtrx.m[3][3] = 1.f;

        m_pShader->SetWorldTransform( mtrx );
        m_pShader->CommitChanges( m_pGraphicContext );

        m_pGraphicContext->ColorWriteEnable( FALSE, FALSE, FALSE, FALSE );
        m_pGraphicContext->SetStencilWriteMask( 0x1 );
        m_pGraphicContext->SetStencilTestFunc( IGraphicContext::StencilTest_Always, 0, 0x1 );
        m_pGraphicContext->SetStencilOperation( IGraphicContext::StencilOp_Keep, IGraphicContext::StencilOp_Keep, IGraphicContext::StencilOp_Replace );

        m_pGraphicContext->DrawPrimitive( IGraphicContext::Primitive_TriagnleStrip, 0, 4, 2 );

        m_pGraphicContext->ColorWriteEnable( TRUE, TRUE, TRUE, TRUE );
        m_pGraphicContext->SetStencilTestFunc( IGraphicContext::StencilTest_Greater, 0, 0x1 );
        m_pGraphicContext->SetStencilOperation( IGraphicContext::StencilOp_Keep, IGraphicContext::StencilOp_Keep, IGraphicContext::StencilOp_Keep );
    }

    VOID Render2D::ClearMask()
    {
        ColorF_RGBA clearColor = {0};
        m_pGraphicContext->Clear( IGraphicContext::Clear_Stencil, clearColor, 1.f, 0 );
    }

    BOOL Render2D::RenderStart( IGraphicContext* pGraphicContext )
    {
        m_pGraphicContext = pGraphicContext;
        m_pGraphicContext->SetVertexBuffer( m_resController->GetScreenStrchQuadVtx()
                                           , m_resController->GetRestoredVertexBufferFormat( UtilGraphicResourceController::FormatType_XYZ_T ));

        ColorF_RGBA color = {0};
        m_pGraphicContext->EnableDepthTest( FALSE );
        m_pGraphicContext->Clear( IGraphicContext::Clear_Depth | IGraphicContext::Clear_Stencil, color, 1.f, 0 );
        m_pGraphicContext->DepthBufferWriteEnable( FALSE );
        m_pGraphicContext->EnableStencilTest( TRUE );
        m_pGraphicContext->SetStencilTestFunc( IGraphicContext::StencilTest_Always, 0, 0x1 );
        m_pGraphicContext->SetStencilOperation( IGraphicContext::StencilOp_Keep, IGraphicContext::StencilOp_Keep, IGraphicContext::StencilOp_Keep );
        m_pGraphicContext->EnableCullFace( FALSE );
        return m_pGraphicContext->BeginScene();
    }

    VOID Render2D::RenderEnd()
    {
        m_pGraphicContext->DepthBufferWriteEnable( TRUE );
        m_pGraphicContext->EnableStencilTest( FALSE );
        m_pGraphicContext->EndScene();
    }

    VOID Render2D::SetClipRect( const Box2D& clipRect )
    {
        if ( !Box2D::Intersect( DEFAULT_CLIP_RECT, clipRect, m_clipRect ) )
        {
            memset( &m_clipRect, 0, sizeof( Box2D ) );
        }
    }

    VOID Render2D::SetShader( IShader* pShader )
    {
        m_pShader = pShader;
        m_pGraphicContext->SetShaderProgram( m_pShader->GetProgram() );
    }

    VOID Render2D::PercentCoord2PixelCoord( const Vector2 pt, Vector2 &out )
    {
        Graphic::ViewPort viewport;
        m_pGraphicContext->GetViewPort( viewport );

        out.x = ( pt.x + 1.f ) / 2.f * viewport.uWidth;
        out.y = ( 1.f - pt.y ) / 2.f * viewport.uHeight;
    }

    VOID Render2D::PixelCoord2PercentCoord( const Vector2 pt, Vector2 &out )
    {
        Graphic::ViewPort viewport;
        m_pGraphicContext->GetViewPort( viewport );

        out.x = pt.x * 2.f / (FLOAT32)viewport.uWidth - 1.f;
        out.y = 1.f - pt.y * 2.f / (FLOAT32)viewport.uHeight;
    }

    Geometry2D::Geometry2D()
        : m_dwStatusFlag( 0 )
    {
        SetVisible( TRUE );
    }

    GeometryGroup2D::GeometryGroup2D()
    {
    }

    VOID GeometryGroup2D::Render( Render2D* render, const RenderContext* context )
    {
        std::list<Geometry2D*>::reverse_iterator iter = m_geometries.rbegin();
        std::list<Geometry2D*>::reverse_iterator iterEnd = m_geometries.rend();
        while ( iter != iterEnd )
        {
            Geometry2D* pGeometry = *iter;
            if ( pGeometry->IsVisible() )
            {
                pGeometry->Render( render, context );
            }
            iter ++;
        }
    }

    Scene2D::Scene2D()
    {
    }

    Scene2D::~Scene2D()
    {
    }

    VOID Scene2D::RenderContent( IGraphicContext* pGraphicContext, const RenderContext* context )
    {
        m_Render2D.SetResController( GetResController() );
        if ( m_Render2D.RenderStart( pGraphicContext ) )
        {
            std::list<Geometry2D*>::reverse_iterator iter = m_2DObjs.rbegin();
            std::list<Geometry2D*>::reverse_iterator iterEnd = m_2DObjs.rend();
            while ( iter != iterEnd )
            {
                Geometry2D* pGeometry = *iter;
                if ( pGeometry->IsVisible() )
                {
                    pGeometry->Render( &m_Render2D, context );
                }
                iter ++;
            }

            m_Render2D.RenderEnd();
        }
    }

    GraphicManager::GraphicManager()
        : m_LayerCollection()
        , m_RenderContext()
    {
    }

    GraphicManager::~GraphicManager()
    {
    }

    BOOL GraphicManager::Initialize()
    {
        memset( &m_RenderContext, 0, sizeof(RenderContext) );
        ClearFilter();
        ClearLayer();
        return TRUE;
    }

    VOID GraphicManager::AddFilter( GraphicFilter* filter )
    {
        m_LayerCollection.AddFilter( filter );
    }

    VOID GraphicManager::RemoveFilter( GraphicFilter* filter )
    {
        m_LayerCollection.RemoveFilter( filter );
    }

    VOID GraphicManager::ClearFilter()
    {
        m_LayerCollection.ClearFilter();
    }

    VOID GraphicManager::AddLayerTop( GraphicLayer* pLayer )
    {
        m_LayerCollection.AddLayerTop( pLayer );
        pLayer->SetResController( m_LayerCollection.GetResController() );
    }

    VOID GraphicManager::AddLayerBottom( GraphicLayer* pLayer )
    {
        m_LayerCollection.AddLayerBottom( pLayer );
        pLayer->SetResController( m_LayerCollection.GetResController() );
    }

    VOID GraphicManager::RemoveLayer( GraphicLayer* pLayer )
    {
        m_LayerCollection.RemoveLayer( pLayer );
        pLayer->SetResController( NULL );
    }

    VOID GraphicManager::ClearLayer()
    {
        m_LayerCollection.ClearLayer();
    }

    VOID GraphicManager::Render( IGraphicContext* pGraphicContext, UINT32 timeInterval )
    {
        m_RenderContext.timeInterval = timeInterval;
        m_RenderContext.timeStamp += timeInterval;
        ViewPort viewPort = { 0 };
        pGraphicContext->GetViewPort( viewPort );
        m_RenderContext.width = (FLOAT32)viewPort.uWidth;
        m_RenderContext.height = (FLOAT32)viewPort.uHeight;
        m_LayerCollection.Render( pGraphicContext, &m_RenderContext );
    }
}