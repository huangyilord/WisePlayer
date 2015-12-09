//
//  GraphicDisplayDestination.h
//  WisePlayer
//
//  Created by Yi Huang on 1/6/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#if !defined ( _GRAPHIC_DISPLAY_DESTINATION_H_ )
#define _GRAPHIC_DISPLAY_DESTINATION_H_

#include "GraphicTypes.h"
#include "PlayerDestination.h"
#include "datatype/RGBATexture.h"

class GraphicDisplayDestination
    : public PlayerDestination
{
public:
    GraphicDisplayDestination( PlayerContext* context );
    virtual ~GraphicDisplayDestination();

    bool Initialize();
    void Destroy();

    PROCEDURE_DESC_START( GraphicDisplayDestination )
        PROCEDURE_INPUT_START()
            PROCEDURE_INPUT( STRETCHED_RGBA_TEXTURE_NAME )
        PROCEDURE_INPUT_END()
        PROCEDURE_OUTPUT_START()
        PROCEDURE_OUTPUT_END()
    PROCEDURE_DESC_END( GraphicDisplayDestination )
protected:
    // implement PlayerDestination
    virtual bool ProcessInput( const PlayerProcedureKey& key, void* data );
    virtual bool UpdateContent( const PlayerDestinationContext& context );
private:
    class DisplayShader
        : public Graphic::IShader
    {
    public:
        DisplayShader();
        virtual ~DisplayShader();

        bool Initialize( Graphic::IGraphicContext* context );
        void Destroy();

        virtual VOID SetTextureTransform( UINT32 stage, const Graphic::Matrix16& mtrx );
        virtual VOID SetWorldTransform( const Graphic::Matrix16& mtrx );
        virtual VOID SetViewTransform( const Graphic::Matrix16& mtrx );
        virtual VOID SetProjectionTransform( const Graphic::Matrix16& mtrx );
        virtual VOID CommitChanges( Graphic::IGraphicContext* pGraphicContext );
        virtual Graphic::SHADER_PROGRAM_HANDLE GetProgram();

        Graphic::SHADER_PROGRAM_HANDLE      m_program;
        Graphic::Matrix16                   m_worldTransform;
        Graphic::Matrix16                   m_viewTransform;
        Graphic::Matrix16                   m_projectionTransform;
        Graphic::Matrix16                   m_textureTransform;

        UINT32                              m_transformConstantLocation;
        UINT32                              m_textureTransformConstantLocation;
    };

    class DisplayGeometry
        : public Graphic::Geometry2D
    {
    public:
        DisplayGeometry();
        virtual ~DisplayGeometry();

        bool Initialize( Graphic::IGraphicContext* context );
        void Destroy();

        void SetPos( FLOAT32 x, FLOAT32 y );
        void SetSize( FLOAT32 x, FLOAT32 y );

        virtual VOID Render( Graphic::Render2D* render, const Graphic::RenderContext* context );

        Graphic::Box2D                  m_target;
        Graphic::TextureInfo            m_textureInfo;
        UINT32                          m_textureWidth;
        UINT32                          m_textureHeight;
        DisplayShader                   m_shader;
    };

    Graphic::GraphicManager         m_graphicManager;
    Graphic::Scene2D                m_scene;
    DisplayGeometry                 m_geometry;
    int64                           m_progress;
};

#endif /* defined(_PLAYER_DESTINATION_H_) */
