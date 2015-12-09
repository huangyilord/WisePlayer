//
//  WisePlayer.cpp
//  WisePlayer
//
//  Created by Yi Huang on 27/5/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#import "WisePlayerEventListener.h"
#import "VideoPlayer.h"
#import "GLESGraphicContext.h"
#import "GraphicTypes.h"
#import "GraphicResourceMgr.h"
#import "OpenALAudioContext.h"

@class PlayerView;

@protocol PlayerViewDelegate <NSObject>
@required
- (void) frameChanged:(PlayerView*)view withNewFrame:(CGRect)frame;
@end

@interface PlayerView : UIView
@property (nonatomic, weak) id<PlayerViewDelegate>  delegate;
@end

@implementation PlayerView
@synthesize delegate;

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (void) setFrame:(CGRect)frame
{
    [super setFrame:frame];
    if ( delegate )
    {
        [delegate frameChanged:self withNewFrame:frame];
    }
}
@end

@interface WisePlayer () <PlayerViewDelegate>
{
    VideoPlayer::VideoPlayer*       player;
    PlayerView*                     view;
    WisePlayerEventListener*        eventListener;

    // OpenGLGLES
    Graphic::GLESGraphicContext*    pGraphicContext;
    Graphic::GraphicResourceMgr*    pResourceManager;
    EAGLContext*                    pGLContext;

    // OpenAL
    Audio::OpenALAudioContext*      pAudioContext;
}
@end

@implementation WisePlayer

@synthesize view;

- (void)setDelegate:(id<WisePlayerDelegate>)delegate
{
    eventListener->delegate = delegate;
}

- (id<WisePlayerDelegate>) delegate
{
    return eventListener->delegate;
}

- (NSTimeInterval)duration
{
    return player->GetDuration();
}

- (void)setProgress:(NSTimeInterval)progress
{
    player->Seek( progress );
}

- (NSTimeInterval)progress
{
    return player->GetProgress();
}

- (instancetype) init
{
    self = [super init];
    if ( self )
    {
        view = [[PlayerView alloc] initWithFrame:CGRectMake(0.f, 0.f, 320.f, 480.f)];
        view.backgroundColor = [UIColor colorWithWhite:0.f alpha:1.f];
        pGraphicContext = new Graphic::GLESGraphicContext();
        if ( NULL == pGraphicContext || !pGraphicContext->Initialize( (__bridge VOID*)view ) )
        {
            return nil;
        }
        pGLContext = (__bridge EAGLContext*)pGraphicContext->GetContext();
        if (!pGLContext)
        {
            return nil;
        }
        Graphic::ViewPort viewport;
        viewport.uMinX = 0;
        viewport.uMinY = 0;
        Graphic::RENDER_TARGET_HANDLE target = pGraphicContext->GetRenderTarget();
        Graphic::SURFACE_HANDLE surface = pGraphicContext->GetColorSurface( target );
        pGraphicContext->GetSurfaceSize( surface, viewport.uWidth, viewport.uHeight );
        pGraphicContext->SetViewPort(viewport);
        pResourceManager = new Graphic::GraphicResourceMgr();
        if ( NULL == pResourceManager || !pResourceManager->Initialize() )
        {
            return nil;
        }
        pAudioContext = new Audio::OpenALAudioContext();
        if ( NULL == pAudioContext || !pAudioContext->Initialize() )
        {
            return nil;
        }
        eventListener = new WisePlayerEventListener();
        if ( NULL == eventListener )
        {
            return nil;
        }
        eventListener->player = self;

        VideoPlayerConfig config = {0};
        config.pGraphicContext = pGraphicContext;
        config.resource = pResourceManager;
        config.pAudioContext = pAudioContext;
        config.pEventListener = eventListener;
        player = new VideoPlayer::VideoPlayer();
        if ( NULL == player || !player->Initialize( config ) )
        {
            return nil;
        }
        player->SetDisplaySize( view.frame.size.width, view.frame.size.height );
        view.delegate = self;
    }
    return self;
}

- (void)dealloc
{
    if ( NULL != player )
    {
        delete player;
    }
    if ( NULL != eventListener )
    {
        delete eventListener;
    }
    if ( NULL != pResourceManager )
    {
        delete pResourceManager;
    }
    if ( NULL != pGraphicContext )
    {
        delete pGraphicContext;
    }
}

- (void) setURL:(NSString*)url;
{
    player->SetURL( [url UTF8String] );
}

- (BOOL) prepareToPlay
{
    return player->PrepareToPlay();
}

- (void) start
{
    player->Start();
}

- (void) stop
{
    player->Stop();
}

- (void) pause
{
    player->Pause();
}

#pragma -- mark PlayerViewDelegate
- (void) frameChanged:(PlayerView*)view withNewFrame:(CGRect)frame
{
    player->SetDisplaySize( frame.size.width, frame.size.height );
}

@end
