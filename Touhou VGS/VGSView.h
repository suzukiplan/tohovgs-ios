//
//  VGSView.h
//  NOKOGI Rider
//
//  Created by Yoji Suzuki on 2013/03/06.
//  Copyright (c) 2013年 SUZUKI PLAN. All rights reserved.
//

#import <pthread.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <QuartzCore/CALayer.h>
#import <UIKit/UIKit.h>

@interface VGSLayer : CALayer
- (void) orientationChanged:(NSNotification *)notification;

@end

@interface VGSView : UIView
{
    CADisplayLink* mpDisplayLink;
}
- (id)initWithFrame:(CGRect)frame;
- (void)drawRect:(CGRect)rect;

@end
