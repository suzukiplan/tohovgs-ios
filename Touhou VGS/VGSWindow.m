//
//  VGSWindow.m
//  NOKOGI Rider
//
//  Created by Yoji Suzuki on 2013/03/11.
//  Copyright (c) 2013年 SUZUKI PLAN. All rights reserved.
//

#import "VGSWindow.h"
#import "VGSAppDelegate.h"
#import "VGSViewController.h"

@implementation VGSWindow

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

- (void)sendEvent:(UIEvent *)event
{
    UIView* gesture=self.rootViewController.view;
    if (gesture /*TransformGesture *gesture in transformGestures*/) {
        // 該当するタッチをすべて、イベントから収集
        NSSet *touches = [event allTouches];
        NSMutableSet *began = nil;
        NSMutableSet *moved = nil;
        NSMutableSet *ended = nil;
        NSMutableSet *cancelled = nil;
        // タッチをフェーズ順に整列して、通常のイベントディスパッチと同様に処理できるようにする
        for(UITouch *touch in touches) {
            switch ([touch phase]) {
                case UITouchPhaseBegan:
                    if (!began) began = [NSMutableSet set];
                    [began addObject:touch];
                    break;
                case UITouchPhaseMoved:
                    if (!moved) moved = [NSMutableSet set];
                    [moved addObject:touch];
                    break;
                case UITouchPhaseEnded:
                    if (!ended) ended = [NSMutableSet set];
                    [ended addObject:touch];
                    break;
                case UITouchPhaseCancelled:
                    if (!cancelled) cancelled = [NSMutableSet set];
                    [cancelled addObject:touch];
                    break;
                default:
                    break;
            }
        }
        // タッチを処理するメソッドを呼び出し
        if (began) [gesture touchesBegan:began withEvent:event];
        if (moved) [gesture touchesMoved:moved withEvent:event];
        if (ended) [gesture touchesEnded:ended withEvent:event];
        if (cancelled) [gesture touchesCancelled:cancelled withEvent:event];
    }
    [super sendEvent:event];
}

/*
- (void) sendEvent:(UIEvent *)event
{
    switch ([event type])
    {
        case UIEventTypeMotion:
            [self catchUIEventTypeMotion: event];
            break;
            
        case UIEventTypeTouches:
            [self catchUIEventTypeTouches: event];
            break;
            
        default:
            break;
    }
    [super sendEvent:(UIEvent *)event];
}

- (void) catchUIEventTypeTouches: (UIEvent *)event
{
    for (UITouch *touch in [event allTouches])
    {
        switch ([touch phase])
        {
            case UITouchPhaseBegan:
                break;
                
            case UITouchPhaseMoved:
                break;
                
            case UITouchPhaseEnded:
                break;
                
            case UITouchPhaseStationary:
                break;
                
            case UITouchPhaseCancelled:
                break;
                
            default:
                break;
        }
    }
}

- (void) catchUIEventTypeMotion: (UIEvent *)event
{
    switch ([event subtype]) {
        case UIEventSubtypeMotionShake:
            break;
            
        default:
            break;
    }
}*/

@end
