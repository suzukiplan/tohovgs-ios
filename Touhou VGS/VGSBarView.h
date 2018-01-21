//
//  VGSBarView.h
//  Battle Marine
//
//  Created by Yoji Suzuki on 2013/11/15.
//  Copyright (c) 2013年 SUZUKI PLAN. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface VGSBarView : UIView {
    UILabel* myclock;
    NSTimer* myTicker;
}

- (void)showDate;

@end
