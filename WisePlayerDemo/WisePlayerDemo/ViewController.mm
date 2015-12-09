//
//  ViewController.m
//  WisePlayerDemo
//
//  Created by Yi Huang on 27/5/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#import "ViewController.h"
#import "WisePlayer.h"

@interface ViewController ()
{
    IBOutlet UIButton*      playBtn;
    IBOutlet UIButton*      stopBtn;
    IBOutlet UISlider*      progressBar;
    WisePlayer*             player;
    int64_t                 duration;
    NSTimer*                progressUpdateTimer;
    BOOL                    isPrepared;
    BOOL                    isPlaying;
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    player = [[WisePlayer alloc] init];
    [self.view addSubview:player.view];
    CGRect bound = self.view.bounds;
    player.view.frame = CGRectMake( bound.origin.x, bound.origin.y, bound.size.width, bound.size.height - 200 );
    //NSString* path = [[NSBundle mainBundle] pathForResource:@"IMG_0147" ofType:@"MOV"];
    //NSString* path = @"rtmp://103.231.144.125:1935/live/test1";
    //NSString* path = @"http://103.231.144.125:1935/live/test1/playlist.m3u8";
    NSString* path = @"http://vodtest.8686c.com/media/mobile/1000/1110/0000/0000/0000/0000/0022/ryBMbmsOcyPjMkJlWwdUsneVQHwydAe3930_1.mp4";
    [player setURL:path];
    isPrepared = NO;
    isPlaying = NO;
    [progressBar addTarget:self action:@selector(seek) forControlEvents:UIControlEventValueChanged];
    progressBar.enabled = NO;
    //progressBar.userInteractionEnabled = NO;
    progressBar.value = 0.f;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)play:(id)sender
{
    if ( !isPrepared )
    {
        isPrepared = YES;
        [player prepareToPlay];
        duration = player.duration;
        if ( duration > 0 )
        {
            progressBar.enabled = YES;
        }
        else
        {
            progressBar.enabled = NO;
        }
        progressBar.value = 0.f;
    }
    if ( isPlaying )
    {
        [playBtn setTitle:@"Play" forState:UIControlStateNormal];
        [player pause];
        isPlaying = NO;
        if ( progressUpdateTimer )
        {
            [progressUpdateTimer invalidate];
            progressUpdateTimer = nil;
        }
    }
    else
    {
        [playBtn setTitle:@"Pause" forState:UIControlStateNormal];
        [player start];
        isPlaying = YES;
        if ( progressBar.enabled )
        {
            progressUpdateTimer = [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(updateProgress:) userInfo:nil repeats:YES];
        }
    }
}

- (IBAction)stop:(id)sender
{
    [player stop];
    isPlaying = NO;
    isPrepared = NO;
    [playBtn setTitle:@"Play" forState:UIControlStateNormal];
    if ( progressUpdateTimer )
    {
        [progressUpdateTimer invalidate];
        progressUpdateTimer = nil;
    }
    progressBar.enabled = NO;
    progressBar.value = 0.f;
}

- (void)updateProgress:(id)timer
{
    NSTimeInterval currentProgress = player.progress;
    [progressBar setValue: (float)currentProgress / (float)duration animated:YES];
}

- (void)seek
{
    player.progress = progressBar.value * duration;
}

@end
