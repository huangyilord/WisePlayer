//
//  WisePlayer.h
//  WisePlayer
//
//  Created by Yi Huang on 27/5/15.
//  Copyright (c) 2015 IPTV. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@class WisePlayer;

typedef enum WisePlayerStatus
{
    WisePlayerStatus_Stop    = 0
    , WisePlayerStatus_Loading
    , WisePlayerStatus_Playing
} WisePlayerStatus;

@protocol WisePlayerDelegate <NSObject>

@optional
- (void) playerStatusChanged:(WisePlayer*)player status:(WisePlayerStatus)status;

@end

@interface WisePlayer : NSObject

@property (nonatomic, readonly) UIView* view;
@property (nonatomic, readwrite, weak) id<WisePlayerDelegate> delegate;

@property (nonatomic, readonly) NSTimeInterval duration;
@property (nonatomic, readwrite) NSTimeInterval progress;

- (void) setURL:(NSString*)url;

- (BOOL) prepareToPlay;

- (void) start;
- (void) stop;
- (void) pause;

@end
