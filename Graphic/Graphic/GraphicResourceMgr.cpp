#include "GraphicResourceMgr.h"
#include "IGraphicContext.h"
#include "GraphicTypes.h"
#include "GraphicMath.h"
#include "GraphicCommonDefs.h"

namespace Graphic
{
    GraphicResourceController::GraphicResourceController()
        : m_pGraphicContext( NULL )
    {
    }

    GraphicResourceController::~GraphicResourceController()
    {
    }

    BOOL GraphicResourceController::InitializeResource( IGraphicContext* pGraphicContext )
    {
        m_pGraphicContext = pGraphicContext;
        BOOL ret = OnInitializeResource( pGraphicContext );
        if ( ret )
        {
            std::list<GraphicResourceController*>::iterator iter = m_subControllers.begin();
            std::list<GraphicResourceController*>::iterator iterEnd = m_subControllers.end();
            while ( iter != iterEnd )
            {
                GraphicResourceController* pSubController = *iter;
                if ( !pSubController->InitializeResource( pGraphicContext ) )
                {
                    ret = FALSE;
                    break;
                }
                iter ++;
            }
        }
        return ret;
    }

    BOOL GraphicResourceController::ResetResourceDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext )
    {
        m_pGraphicContext = pNewGraphicContext;
        BOOL ret = OnResetResourceDevice( pOldGraphicContext, pNewGraphicContext );
        std::list<GraphicResourceController*>::iterator iter = m_subControllers.begin();
        std::list<GraphicResourceController*>::iterator iterEnd = m_subControllers.end();
        while ( iter != iterEnd )
        {
            GraphicResourceController* pSubController = *iter;
            if ( !pSubController->ResetResourceDevice( pOldGraphicContext, pNewGraphicContext ) )
            {
                ret = FALSE;
                break;
            }
            iter ++;
        }
        return ret;
    }

    VOID GraphicResourceController::DestroyResource( IGraphicContext* pGraphicContext )
    {
        std::list<GraphicResourceController*>::iterator iter = m_subControllers.begin();
        std::list<GraphicResourceController*>::iterator iterEnd = m_subControllers.end();
        while ( iter != iterEnd )
        {
            GraphicResourceController* pSubController = *iter;
            pSubController->DestroyResource( pGraphicContext );
            iter ++;
        }
        OnDestroyResource( pGraphicContext );
        m_pGraphicContext = NULL;
    }

    GraphicResourceMgr::GraphicResourceMgr()
        : m_controllers()
        , m_controllerIDs()
    {
    }

    GraphicResourceMgr::~GraphicResourceMgr()
    {
        Destroy();
    }

    BOOL GraphicResourceMgr::Initialize()
    {
        Destroy();
        return TRUE;
    }

    VOID GraphicResourceMgr::Destroy()
    {
        ResControllerMap::iterator iter = m_controllers.begin();
        ResControllerMap::iterator iterEnd = m_controllers.end();
        while ( iter != iterEnd )
        {
            IGraphicContext* pGraphicContext = iter->first;
            std::set<GraphicResourceController*>* controllerSet = iter->second;
            std::set<GraphicResourceController*>::iterator iterSet = controllerSet->begin();
            std::set<GraphicResourceController*>::iterator iterSetEnd = controllerSet->end();
            while ( iterSet != iterSetEnd )
            {
                GraphicResourceController* controller = *iterSet;
                controller->DestroyResource( pGraphicContext );
                iterSet ++;
            }
            iter ++;
        }
        m_controllers.clear();
        m_controllerIDs.clear();
    }

    BOOL GraphicResourceMgr::ResetDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext, BOOL hasDestroy )
    {
        ResControllerMap::iterator iter = m_controllers.find( pOldGraphicContext );
        if ( iter == m_controllers.end() )
        {
            return TRUE;
        }
        std::set<GraphicResourceController*>* controllerSet = iter->second;
        m_controllers.erase(iter);

        m_controllers.insert( std::pair<IGraphicContext*, std::set<GraphicResourceController*>* >(pNewGraphicContext, controllerSet));
        

        std::set<GraphicResourceController*>::iterator iterSet = controllerSet->begin();
        std::set<GraphicResourceController*>::iterator iterSetEnd = controllerSet->end();
        while ( iterSet != iterSetEnd )
        {
            GraphicResourceController* controller = *iterSet;
            
            if (hasDestroy)
            {
                if ( !controller->InitializeResource(pNewGraphicContext) )
                {
                    return FALSE;
                }
            }
            else
            {
                if ( controller->ResetResourceDevice( pOldGraphicContext, pNewGraphicContext ) )
                {
                    return FALSE;
                }
            }
               
            iterSet ++;
        }

        // replace old id infomation with new device
        ControllerIDMap::iterator iterID = m_controllerIDs.begin();
        ControllerIDMap::iterator iterIDEnd = m_controllerIDs.end();
        while ( iterID != iterIDEnd )
        {
            if ( iterID->second.first == pOldGraphicContext )
            {
                iterID->second.first = pNewGraphicContext;
            }
            iterID ++;
        }
        return TRUE;
    }

    BOOL GraphicResourceMgr::DestroyDevice(IGraphicContext* pGraphicContext )
    {
        ResControllerMap::iterator iter = m_controllers.find( pGraphicContext );
        if ( iter == m_controllers.end() )
        {
            return TRUE;
        }

        std::set<GraphicResourceController*>* controllerSet = iter->second;              
        
        std::set<GraphicResourceController*>::iterator iterSet = controllerSet->begin();
        std::set<GraphicResourceController*>::iterator iterSetEnd = controllerSet->end();
        while ( iterSet != iterSetEnd )
        {
            GraphicResourceController* controller = *iterSet;
            
            controller->DestroyResource( pGraphicContext );
            iterSet++;
        
        }

        return TRUE;
    }

    
    BOOL GraphicResourceMgr::RegisterController( UINT32 id, IGraphicContext* pGraphicContext, GraphicResourceController* controller )
    {
        if ( NULL == controller )
        {
            return FALSE;
        }
        // if this ID has been registered, return
        ControllerIDMap::iterator iterID = m_controllerIDs.find( id );
        if ( iterID != m_controllerIDs.end() )
        {
            return FALSE;
        }
        ResControllerMap::iterator iter = m_controllers.find( pGraphicContext );
        if ( iter != m_controllers.end() )
        {
            std::set<GraphicResourceController*>* controllerSet = iter->second;
            std::set<GraphicResourceController*>::iterator iterSet = controllerSet->find( controller );
            if ( iterSet != controllerSet->end() )
            {
                // if this controller has been registered before, return
                return FALSE;
            }
            if ( !controller->InitializeResource( pGraphicContext ) )
            {
                return FALSE;
            }
            controllerSet->insert( controller );
        }
        else
        {
            if ( !controller->InitializeResource( pGraphicContext ) )
            {
                return FALSE;
            }

            std::set<GraphicResourceController*>* controllerSet = new std::set<GraphicResourceController*>();
            m_controllers.insert(std::pair<IGraphicContext*, std::set<GraphicResourceController*>* >(pGraphicContext,controllerSet));
            controllerSet->insert( controller );
        }
        m_controllerIDs[id] = std::pair<IGraphicContext*, GraphicResourceController*>( pGraphicContext, controller );
        return TRUE;
    }

    BOOL GraphicResourceMgr::UnregisterController( UINT32 id )
    {
        ControllerIDMap::iterator iterID = m_controllerIDs.find( id );
        if ( iterID == m_controllerIDs.end() )
        {
            return FALSE;
        }
        IGraphicContext* pGraphicContext = iterID->second.first;
        GraphicResourceController* controller = iterID->second.second;

        ResControllerMap::iterator iter = m_controllers.find( pGraphicContext );
        ASSERT( iter != m_controllers.end() );

        std::set<GraphicResourceController*>* controllerSet = iter->second;
        std::set<GraphicResourceController*>::iterator iterSet = controllerSet->find( controller );

        ASSERT( iterSet != controllerSet->end() );
        controller->DestroyResource( pGraphicContext );
        controllerSet->erase( iterSet );
        if ( controllerSet->empty() )
        {
            m_controllers.erase( iter );
            delete controllerSet;
        }
        m_controllerIDs.erase( iterID );
        return TRUE;
    }

    GraphicResourceController* GraphicResourceMgr::GetController( UINT32 id )
    {
        ControllerIDMap::iterator iterID = m_controllerIDs.find( id );
        if ( iterID == m_controllerIDs.end() )
        {
            return NULL;
        }
        return iterID->second.second;
    }

    UtilGraphicResourceController::UtilGraphicResourceController()
        : m_screenStrchQuad( NULL )
        , m_allScreenTexture()
        , m_unengagedRenderTargetIDs()
        , m_depthSurface( NULL )
    {
        memset( &m_vertexBufferFormat, 0, sizeof(m_vertexBufferFormat) );
    }

    UtilGraphicResourceController::~UtilGraphicResourceController()
    {
    }

    struct StrchQuadVertex
    {
        Vector3 pos;
        Vector2 texCoord;
    };

    BOOL UtilGraphicResourceController::OnInitializeResource( IGraphicContext* pGraphicContext )
    {
        //DestroyResource( pGraphicContext );

        for ( UINT32 i = 0; i < MAX_SCREEN_TEXTURE_COUNT; ++i )
        {
            TEXTURE_HANDLE tex = pGraphicContext->CreateTexture( SCREEN_TEXTURE_WIDTH, SCREEN_TEXTURE_HEIGHT );
            if ( NULL == tex )
            {
                DestroyResource( pGraphicContext );
                return FALSE;
            }
            m_allScreenTexture.push_back( tex );
        }

        m_depthSurface = pGraphicContext->CreateDepthStencilSurface( SCREEN_TEXTURE_WIDTH, SCREEN_TEXTURE_HEIGHT );
        if ( NULL == m_depthSurface )
        {
            DestroyResource( pGraphicContext );
            return FALSE;
        }

        for ( UINT32 i = 0; i < MAX_SCREEN_TEXTURE_COUNT; ++i )
        {
            RENDER_TARGET_HANDLE target = pGraphicContext->CreateRenderTarget();
            if ( NULL == target )
            {
                DestroyResource( pGraphicContext );
                return FALSE;
            }
            SURFACE_HANDLE surface = pGraphicContext->GetSurfaceFromTexture( m_allScreenTexture[i] );
            pGraphicContext->SetColorSurface( target, surface );
            pGraphicContext->SetDepthStencilSurface( target, m_depthSurface );
            m_unengagedRenderTargetIDs.insert( (UINT32)m_allScreenRenderTarget.size() );
            m_allScreenRenderTarget.push_back( target );
        }

        m_screenStrchQuad = pGraphicContext->CreateVertexBuffer( 4 * sizeof(StrchQuadVertex) );
        if ( NULL == m_screenStrchQuad )
        {
            DestroyResource( pGraphicContext );
            return FALSE;
        }
        StrchQuadVertex* buffer = (StrchQuadVertex*)pGraphicContext->GetVertexBufferPointer( m_screenStrchQuad );
        if ( NULL == buffer )
        {
            DestroyResource( pGraphicContext );
            return FALSE;
        }
        memset( buffer, 0, 4 * sizeof( StrchQuadVertex ) );
        buffer[0].pos.x = SCREEN_COORD_X_MIN;
        buffer[0].pos.y = SCREEN_COORD_Y_MAX;
        buffer[0].texCoord.x = 0.f;
        buffer[0].texCoord.y = 0.f;

        buffer[1].pos.x = SCREEN_COORD_X_MIN;
        buffer[1].pos.y = SCREEN_COORD_Y_MIN;
        buffer[1].texCoord.x = 0.f;
        buffer[1].texCoord.y = 1.f;

        buffer[2].pos.x = SCREEN_COORD_X_MAX;
        buffer[2].pos.y = SCREEN_COORD_Y_MAX;
        buffer[2].texCoord.x = 1.f;
        buffer[2].texCoord.y = 0.f;

        buffer[3].pos.x = SCREEN_COORD_X_MAX;
        buffer[3].pos.y = SCREEN_COORD_Y_MIN;
        buffer[3].texCoord.x = 1.f;
        buffer[3].texCoord.y = 1.f;

        if ( !pGraphicContext->CommitVertexBuffer( buffer, m_screenStrchQuad ) )
        {
            DestroyResource( pGraphicContext );
            return FALSE;
        }

        static const IGraphicContext::VertexElement s_vertexBufferFormat_XYZ[] =
        {
            { IGraphicContext::DataUsage_Position, IGraphicContext::DataType_Float, 3, 0 }
        };

        static const IGraphicContext::VertexElement s_vertexBufferFormat_XYZ_N[] =
        {
            { IGraphicContext::DataUsage_Position, IGraphicContext::DataType_Float, 3, 0 }
            , { IGraphicContext::DataUsage_Normal, IGraphicContext::DataType_Float, 3, sizeof(float) * 3 }
        };

        static const IGraphicContext::VertexElement s_vertexBufferFormat_XYZ_N_T[] =
        {
            { IGraphicContext::DataUsage_Position, IGraphicContext::DataType_Float, 3, 0 }
            , { IGraphicContext::DataUsage_Normal, IGraphicContext::DataType_Float, 3, sizeof(float) * 3 }
            , { IGraphicContext::DataUsage_TexCoord0, IGraphicContext::DataType_Float, 2, sizeof(float) * 6 }
        };

        static const IGraphicContext::VertexElement s_vertexBufferFormat_XYZ_N_T2[] =
        {
            { IGraphicContext::DataUsage_Position, IGraphicContext::DataType_Float, 3, 0 }
            , { IGraphicContext::DataUsage_Normal, IGraphicContext::DataType_Float, 3, sizeof(float) * 3 }
            , { IGraphicContext::DataUsage_TexCoord0, IGraphicContext::DataType_Float, 2, sizeof(float) * 6 }
            , { IGraphicContext::DataUsage_TexCoord1, IGraphicContext::DataType_Float, 2, sizeof(float) * 8 }
        };

        static const IGraphicContext::VertexElement s_vertexBufferFormat_XYZ_T[] =
        {
            { IGraphicContext::DataUsage_Position, IGraphicContext::DataType_Float, 3, 0 }
            , { IGraphicContext::DataUsage_TexCoord0, IGraphicContext::DataType_Float, 2, sizeof(float) * 3 }
        };

        static const IGraphicContext::VertexElement s_vertexBufferFormat_XYZ_T2[] =
        {
            { IGraphicContext::DataUsage_Position, IGraphicContext::DataType_Float, 3, 0 }
            , { IGraphicContext::DataUsage_TexCoord0, IGraphicContext::DataType_Float, 2, sizeof(float) * 3 }
            , { IGraphicContext::DataUsage_TexCoord1, IGraphicContext::DataType_Float, 2, sizeof(float) * 5 }
        };

        static const IGraphicContext::VertexElement* const s_vertexBufferFormatElements[] =
        {
            s_vertexBufferFormat_XYZ
            , s_vertexBufferFormat_XYZ_N
            , s_vertexBufferFormat_XYZ_N_T
            , s_vertexBufferFormat_XYZ_N_T2
            , s_vertexBufferFormat_XYZ_T
            , s_vertexBufferFormat_XYZ_T2
        };

        static const UINT32 s_vertexBufferFormatElementCounts[] =
        {
            1
            , 2
            , 3
            , 4
            , 2
            , 3
        };

        for ( UINT32 i = 0; i < FormatType_Num; ++i )
        {
            m_vertexBufferFormat[i] = pGraphicContext->CreateVertexBufferFormat( s_vertexBufferFormatElements[i], s_vertexBufferFormatElementCounts[i]);
            if ( NULL == m_vertexBufferFormat[i] )
            {
                DestroyResource( pGraphicContext );
                return FALSE;
            }
        }
        return TRUE;
    }

    BOOL UtilGraphicResourceController::OnResetResourceDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext )
    {
        DestroyResource( pOldGraphicContext );
        return InitializeResource( pNewGraphicContext );
    }

    VOID UtilGraphicResourceController::OnDestroyResource( IGraphicContext* pGraphicContext )
    {
        for ( UINT32 i = 0; i < FormatType_Num; ++i )
        {
            if ( NULL != m_vertexBufferFormat[i] )
            {
                pGraphicContext->DestroyVertexBufferFormat( m_vertexBufferFormat[i] );
                m_vertexBufferFormat[i] = NULL;
            }
        }

        if ( NULL != m_depthSurface )
        {
            pGraphicContext->DestroyDepthStencilSurface( m_depthSurface );
            m_depthSurface = NULL;
        }

        std::vector<TEXTURE_HANDLE>::iterator iter = m_allScreenTexture.begin();
        std::vector<TEXTURE_HANDLE>::iterator iterEnd = m_allScreenTexture.end();
        while ( iter != iterEnd )
        {
            TEXTURE_HANDLE tex = *iter;
            if ( NULL != tex )
            {
                pGraphicContext->DestroyTexture( tex );
            }
            iter++;
        }
        m_allScreenTexture.clear();

        std::vector<RENDER_TARGET_HANDLE>::iterator iterTarget = m_allScreenRenderTarget.begin();
        std::vector<RENDER_TARGET_HANDLE>::iterator iterTargetEnd = m_allScreenRenderTarget.end();
        while ( iterTarget != iterTargetEnd )
        {
            RENDER_TARGET_HANDLE target = *iterTarget;
            if ( NULL != target )
            {
                pGraphicContext->DestroyRenderTarget( target );
            }
            iterTarget++;
        }
        m_allScreenTexture.clear();
        m_unengagedRenderTargetIDs.clear();

        if ( NULL != m_screenStrchQuad )
        {
            pGraphicContext->DestroyVertexBuffer( m_screenStrchQuad );
            m_screenStrchQuad = NULL;
        }
    }

    BOOL UtilGraphicResourceController::GetUnengagedRenderTargetID( UINT32 ids[], UINT32 count )
    {
        std::set<UINT32>::iterator iter = m_unengagedRenderTargetIDs.begin();
        std::set<UINT32>::iterator iterEnd = m_unengagedRenderTargetIDs.end();
        UINT32 index = 0;
        while ( iter != iterEnd && index < count )
        {
            ids[index++] = *iter;
            iter ++;
        }
        return index == count;
    }

    BOOL UtilGraphicResourceController::EngageARenderTargetID( UINT32 id )
    {
        std::set<UINT32>::iterator iter = m_unengagedRenderTargetIDs.find( id );
        if ( iter != m_unengagedRenderTargetIDs.end() )
        {
            m_unengagedRenderTargetIDs.erase( iter );
            return TRUE;
        }
        return FALSE;
    }

    VOID UtilGraphicResourceController::FreeARenderTargetID( UINT32 id )
    {
        if ( id < m_allScreenTexture.size() )
        {
            m_unengagedRenderTargetIDs.insert( id );
        }
    }
}
