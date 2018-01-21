//
//  SHOT04AppDelegate.m
//  NOKOGI Rider
//
//  Created by Yoji Suzuki on 2013/03/06.
//  Copyright (c) 2013年 SUZUKI PLAN. All rights reserved.
//

#import "VGSAppDelegate.h"
#import "VGSViewController.h"
#import "VGSView.h"
#import "VGSWindow.h"
#import "vge.h"

extern int isBackground;
extern int _forcePause;
extern int g_playing;
extern int g_background;


@implementation VGSAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // initialize window
//    [[UIApplication sharedApplication] setStatusBarStyle:UIStatusBarStyleLightContent];
    window = [[VGSWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    viewController = [[VGSViewController alloc] init];
    window.rootViewController=viewController;
    [viewController awakeFromNib];
    [window addSubview:viewController.view];
    [window makeKeyAndVisible];
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];

    // initialize audio session
    [self loadALC];
    AudioSessionInitialize(NULL, NULL, interruptionListener, (__bridge void*) self);
    AVAudioSession *session = [AVAudioSession sharedInstance];
    [session setCategory:AVAudioSessionCategoryPlayback error:nil];

    return YES;
}

- (void)loadALC {
    UInt32 category = kAudioSessionCategory_AmbientSound;
    AudioSessionSetProperty (kAudioSessionProperty_AudioCategory, sizeof (category), &category);
    AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange,propertyListener,(__bridge void *)self);
    AudioSessionSetActive(YES);
    
    ALCdevice*  device;
    ALCcontext* alContext = alcGetCurrentContext();
    if (alContext == nil) {
        device = alcOpenDevice(NULL);
        alContext = alcCreateContext(device, NULL);
        alcMakeContextCurrent(alContext);
    }
}

void propertyListener (void *inClientData,
                         AudioSessionPropertyID inID,
                         UInt32 inDataSize,
                         const void *inData
                         ) {
    if (inID != kAudioSessionProperty_AudioRouteChange) {
        return;
    }
    CFDictionaryRef dic=inData;
    CFNumberRef res1 = CFDictionaryGetValue(dic,CFSTR(kAudioSession_AudioRouteChangeKey_Reason));
    SInt32 res2;
    CFNumberGetValue (res1,kCFNumberSInt32Type,&res2);

    if(res2==kAudioSessionRouteChangeReason_OldDeviceUnavailable) {
        CFStringRef oldRouteRef = CFDictionaryGetValue (dic,CFSTR(kAudioSession_AudioRouteChangeKey_OldRoute));
        NSString* route=(__bridge NSString*)oldRouteRef;
        NSLog(@"Lost an audio route: %@",route);
        if(strcasecmp(route.UTF8String,"Headphone")==0) {
            if(0==_forcePause) _forcePause=1;
            vge_term();
            _exit(0);
        }
    }
}

void interruptionListener(void *inUserData, UInt32 inInterruption) {
    if (inInterruption == kAudioSessionBeginInterruption) {
        printf("BeginInterruption\n");
        vge_term();
        _exit(0);
    }
    if (inInterruption == kAudioSessionEndInterruption) {
        printf("EndInterruption\n");
    }
}
                                    
                                    

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    NSLog(@"App will region active.");
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    if(0==g_playing) {
        NSLog(@"App did exit because not playing.");
        vge_term();
        _exit(0);
    } else {
        NSLog(@"App did enter background.");
        isBackground=1;
        g_background=1;
    }
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
    NSLog(@"App did enter foreground.");
    isBackground=0;
    g_background=0;
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    NSLog(@"App did become active.");
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    NSLog(@"Touhou VGS now be ended.");
    vge_term();
    [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
}

@end
