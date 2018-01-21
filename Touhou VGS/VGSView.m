//  Created by Yoji Suzuki on 2013/03/06.
//  Copyright(C) 2013, SUZUKI PLAN. All rights reserved.

#import <Foundation/NSURL.h>
#import "VGSView.h"
#import "VGSViewController.h"
#import "vge.h"
#import "vgeint.h"

extern int g_request;
extern volatile int isBGActive;
static VGSView* sharedInstance = nil;
static unsigned short imgbuf[2][320 * 240 * 2];
static CGContextRef img[2];
static pthread_t tid=0;
static volatile int bno;
static volatile bool event_flag=false;
static volatile bool alive_flag=true;
static volatile bool end_flag=false;

int get_hiscore(); // game.c

@implementation VGSLayer

// Main loop
static void* GameLoop(void* args)
{
    int i;
    unsigned short* buf;
    unsigned char* bg;
    unsigned char* sp;
    
    while(alive_flag) {
        while(event_flag) usleep(100);
        // call AP procedure
        if(!isBGActive) {
            if(_pause) {
                vge_pause();
            } else {
                vge_loop();
            }
        }
        
        // NOTES: does not implements the exit procedure of VGS.
        // REASON: Apple rule.
        
        // VRAM to CG image buffer
        buf=(unsigned short*)imgbuf[bno];
        bg=_vram.bg;
        sp=_vram.sp;
        for(i=0;i<76800;i++) { // 240 x 320
            if(*sp) {
                *buf=ADPAL[*sp];
                *sp=0;
            } else {
                *buf=ADPAL[*bg];
            }
            bg++;
            sp++;
            buf++;
        }
        event_flag=true;
        if(g_request) {
            switch(g_request) {
                case 1:
                {
                    // http://hp.vector.co.jp/authors/VA040196/
                    NSURL *url = [ NSURL URLWithString :
                                  @"https://googledrive.com/host/0B7c8xPyv4KQWZ1g0R0tkMHNPS3M/index.html"
                                  ];
                    [[UIApplication sharedApplication] openURL:url];
                    break;
                }
                case 2:
                {
                    break;
                }
                case 3:
                {
                    NSURL *url = [ NSURL URLWithString :
                                  @"https://itunes.apple.com/us/artist/yoji-suzuki/id619627004?uo=4"
                                  ];
                    [[UIApplication sharedApplication] openURL:url];
                    break;
                }
            }
            g_request=0;
        }
    }
    end_flag=true;
    return NULL;
}

+ (id) defaultActionForKey:(NSString *)key
{
    return nil;
}

- (id)init {
    if (self = [super init]) {
        img[0] = nil;
        img[1] = nil;
        // create image buffer
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        for(int i=0;i<2;i++) {
            img[i] = CGBitmapContextCreate(imgbuf[i],
                                    240,320,
                                    5,
                                    2*240,
                                    colorSpace,
                                    kCGImageAlphaNoneSkipFirst|
                                    kCGBitmapByteOrder16Little
                                    );
        }
        CFRelease(colorSpace);
        // start buffering thread
        pthread_create(&tid, NULL, GameLoop, NULL);
        struct sched_param param;
        memset(&param,0,sizeof(param));
        param.sched_priority = 46;
        pthread_setschedparam(tid,SCHED_OTHER,&param);
    }
    return self;
}

- (void) orientationChanged:(NSNotification *)notification
{
    // ignore because NOKOGI Rider is normal portrait only.
}

- (void)display {
    while(!event_flag) usleep(100); // wait for sync
    bno=1-bno;                      // flip to another vram
    event_flag=false;               // start another buffering
    // visible the previous buffer
    CGImageRef cgImage = CGBitmapContextCreateImage(img[1-bno]);
    self.contents = (__bridge id)cgImage;
    CFRelease(cgImage);
}

- (void)dealloc
{
    alive_flag=false;
    while(!end_flag) usleep(100);
    for(int i=0;i<2;i++) {
        if(img[i]!=nil) {
            CFRelease(img[i]);
            img[i]=nil;
        }
    }
}
@end

@implementation VGSView

+ (Class) layerClass
{
    return [VGSLayer class];
}

- (id)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])!=nil) {
        // slow interfaces are no thanks (this view is video only)
        self.opaque = NO;
        self.clearsContextBeforeDrawing = NO;
        self.multipleTouchEnabled = NO;
        self.userInteractionEnabled = NO;
        sharedInstance=self;
        // call setNeedsDisplay by VSYNC
        mpDisplayLink=[CADisplayLink displayLinkWithTarget:self selector:@selector(setNeedsDisplay)];
        [mpDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
    return self;
}

- (void)dealloc
{
    // not need destroy procedure of CADisplayLink
}

- (void)drawRect:(CGRect)rect
{
    // no draw in view, because VERY slow
}

@end
