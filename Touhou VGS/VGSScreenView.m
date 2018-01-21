//
//  VGSScreenView.m
//  NOKOGI Rider
//
//  Created by Yoji Suzuki on 2013/03/12.
//  Copyright (c) 2013年 SUZUKI PLAN. All rights reserved.
//

#import "VGSScreenView.h"
#import "vgeint.h"
#import "vge.h"
extern int _flingY;
extern int _flingX;
extern double vertical_zoom;

struct MoveDir {
    NSTimeInterval time;
    int movedX;
    int movedY;
};
static struct MoveDir _moveDir[256];
static int _moveCur;

@implementation VGSScreenView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        NSLog(@"width=%f, height=%f"
              ,self.frame.size.width
              ,self.frame.size.height
              );
        self.backgroundColor=[UIColor blackColor]; /*VERY IMPORTANT*/
        self.multipleTouchEnabled=YES;
        self.exclusiveTouch=YES;
    }
    return self;
}

// detect touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{    
    UITouch *touch = [touches anyObject];
    _timestampBegan = event.timestamp;
    _pointBegan = [touch locationInView:self];
    _moveCur = 0;
    memset(_moveDir, 0, sizeof(_moveDir));
    
    touches=[event allTouches];
    UITouch* aTouch=[touches anyObject];
    CGPoint point=[aTouch locationInView:self];
    if(2<=touches.count) {
        memset(&_touch,0,sizeof(_touch));
        return;
    }
    _touch.s=1;
    _touch.t++;
    _touch.cx=(int)(point.x*0.75);
    _touch.cy=(int)((point.y-20)*vertical_zoom);
    _touch.px=_touch.cx;
    _touch.py=_touch.cy;
    _touch.dx=0;
    _touch.dy=0;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    touches=[event allTouches];
    UITouch* aTouch=[touches anyObject];
    CGPoint point=[aTouch locationInView:self];
    if(2<=touches.count) {
        memset(&_touch,0,sizeof(_touch));
        return;
    }
/*  if(3<=touches.count && 0==_pause) {
        vge_setPause(1);
        memset(&_touch,0,sizeof(_touch));
        return;
    }*/
    _touch.s=1;
    _touch.t++;
    _touch.px=_touch.cx;
    _touch.py=_touch.cy;
    _touch.cx=(int)(point.x*0.75);
    _touch.cy=(int)((point.y-20)*vertical_zoom);
    _moveDir[_moveCur].time = [[NSDate date] timeIntervalSince1970];
    _moveDir[_moveCur].movedX = _touch.cx-_touch.px;
    _moveDir[_moveCur].movedY = _touch.cy-_touch.py;
    _moveCur++;
    _moveCur &= 0xff;
    if(_touch.px) _touch.dx+=_touch.cx-_touch.px;
    if(_touch.py) _touch.dy+=_touch.cy-_touch.py;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    memset(&_touch,0,sizeof(_touch));
    
    int sp = (_moveCur + 1) & 0xff;
    _flingX = 0;
    _flingY = 0;
    NSTimeInterval now = [[NSDate date] timeIntervalSince1970];
    while (sp != _moveCur) {
        if (now - _moveDir[sp].time < 0.1) {
            _flingX += _moveDir[sp].movedX;
            _flingY += _moveDir[sp].movedY;
        }
        sp++;
        sp &= 0xff;
    }
    if (abs(_flingY) < abs(_flingX)) {
        _flingY = 0;
    } else {
        _flingX = 0;
    }
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

@end
