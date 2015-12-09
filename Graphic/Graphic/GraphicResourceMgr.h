#if !defined ( _GRAPHIC_RESOURCE_MGR_H_ )
#define _GRAPHIC_RESOURCE_MGR_H_

#include "GraphicTypeDef.h"
#include <map>
#include <set>
#include <list>
#include <vector>

namespace Graphic
{
    class IGraphicContext;

    class GraphicResourceController
    {
    public:
        enum GraphicResourceControllerID
        {
            GRCID_UTIL = 0
            , GRCID_FONT

            , GRCID_MIN
        };

        GraphicResourceController();
        virtual ~GraphicResourceController();

        BOOL InitializeResource( IGraphicContext* pGraphicContext );
        BOOL ResetResourceDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext );
        VOID DestroyResource( IGraphicContext* pGraphicContext );

        inline VOID AddSubController( GraphicResourceController* pController ) { m_subControllers.push_back( pController ); }
        inline VOID RemoveSubController( GraphicResourceController* pController ) { m_subControllers.remove( pController ); }
    protected:
        /* This function will be called when the controller first be registered.
           Use the given IGraphicContext to initialize it own graphic resoiurce.
        */
        virtual BOOL OnInitializeResource( IGraphicContext* pGraphicContext ) = 0;

        /* This function is called when the IGraphicContext changed.
           The Controller need to reset resources if needed.
        */
        virtual BOOL OnResetResourceDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext ) = 0;

        /* This function is called before the controller is unregistered.
           Use the given IGraphicContext to release graphic resoiurce.
        */
        virtual VOID OnDestroyResource( IGraphicContext* pGraphicContext ) = 0;

        IGraphicContext*                                m_pGraphicContext;
    private:
        std::list<GraphicResourceController*>           m_subControllers;
    };

    class DECLSPEC_NOVTABLE GraphicResourceMgr
    {
    public:
        GraphicResourceMgr();
        ~GraphicResourceMgr();

        BOOL Initialize();
        VOID Destroy();

        BOOL ResetDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext, BOOL hasDestroy = FALSE );
        BOOL DestroyDevice(IGraphicContext* pGraphicContext );

        BOOL RegisterController( UINT32 id, IGraphicContext* pGraphicContext, GraphicResourceController* controller );
        BOOL UnregisterController( UINT32 id );
        
        GraphicResourceController* GetController( UINT32 id );
    private:
        typedef std::map<IGraphicContext*, std::set<GraphicResourceController*>* > ResControllerMap;
        typedef std::map<UINT32, std::pair<IGraphicContext*, GraphicResourceController*> > ControllerIDMap;

        ResControllerMap    m_controllers;
        ControllerIDMap     m_controllerIDs;
    };

#define INVALID_SCREEN_TEXTURE_ID   0xFFFFFFFF
#define MAX_SCREEN_TEXTURE_COUNT    4
#define SCREEN_TEXTURE_WIDTH        1024
#define SCREEN_TEXTURE_HEIGHT       1024

    class UtilGraphicResourceController
        : public GraphicResourceController
    {
    public:
        enum VertexBufferFormatType
        {
            FormatType_XYZ          = 0
            , FormatType_XYZ_N
            , FormatType_XYZ_N_T
            , FormatType_XYZ_N_T2
            , FormatType_XYZ_T
            , FormatType_XYZ_T2

            , FormatType_Num
        };

        UtilGraphicResourceController();
        ~UtilGraphicResourceController();

        inline VERTEX_BUFFER_HANDLE GetScreenStrchQuadVtx() { return m_screenStrchQuad; }
        inline VERTEX_BUFFER_FORMAT_HANDLE GetRestoredVertexBufferFormat( VertexBufferFormatType type ) { return m_vertexBufferFormat[type]; }
        inline TEXTURE_HANDLE GetScreenTexture( UINT32 index ) { return m_allScreenTexture[index]; }
        inline VOID GetScreenTextureSize( UINT32 &width, UINT32 &height ) { width = SCREEN_TEXTURE_WIDTH; height = SCREEN_TEXTURE_HEIGHT; }
        inline SURFACE_HANDLE GetScreenDepthSurface() { return m_depthSurface; }
        inline RENDER_TARGET_HANDLE GetScreenRenderTarget( UINT32 index ) { return m_allScreenRenderTarget[index]; }

        BOOL GetUnengagedRenderTargetID( UINT32 ids[], UINT32 count );
        BOOL EngageARenderTargetID( UINT32 index );
        VOID FreeARenderTargetID( UINT32 index );
    protected:
        // GraphicResourceController interface
        virtual BOOL OnInitializeResource( IGraphicContext* pGraphicContext );
        virtual BOOL OnResetResourceDevice( IGraphicContext* pOldGraphicContext, IGraphicContext* pNewGraphicContext );
        virtual VOID OnDestroyResource( IGraphicContext* pGraphicContext );
    private:
        VERTEX_BUFFER_HANDLE                m_screenStrchQuad;
        std::vector<TEXTURE_HANDLE>         m_allScreenTexture;
        std::vector<RENDER_TARGET_HANDLE>   m_allScreenRenderTarget;
        std::set<UINT32>                    m_unengagedRenderTargetIDs;
        VERTEX_BUFFER_FORMAT_HANDLE         m_vertexBufferFormat[FormatType_Num];
        UINT32                              m_screenTexMaxCount;
        SURFACE_HANDLE                      m_depthSurface;
    };
}

#endif
