//
//  VGSAppDelegate.h
//  Hopper
//
//  Created by Yoji Suzuki on 2013/04/29.
//  Copyright (c) 2013年 SUZUKI PLAN. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVAudioSession.h>
#import <AudioToolbox/AudioToolbox.h>

@class VGSViewController;

@interface VGSAppDelegate : UIResponder <UIApplicationDelegate>
{
    UIWindow* window;
    VGSViewController* viewController;
}
@end
