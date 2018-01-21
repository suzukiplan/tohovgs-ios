//
//  VGSViewController.m
//  NOKOGI Rider
//
//  Created by Yoji Suzuki on 2013/03/06.
//  Copyright (c) 2013年 SUZUKI PLAN. All rights reserved.
//

#import "VGSViewController.h"
#import "VGSScreenView.h"
#import "VGSView.h"
#import "VGSBarView.h"
#import "vgeint.h"
#import "vge.h"

int vgsint_init(const char* bin);
double vertical_zoom;

@interface VGSViewController ()

@end

@implementation VGSViewController

/*
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}
*/

- (void)viewDidLoad
{
    static int initialized=0;
    if(initialized) return;
    initialized=1;

    [super viewDidLoad];

    // get iOS version
    const float version = [[[UIDevice currentDevice] systemVersion] floatValue];
    NSLog(@"iOS version: %f", version);
    if(7.0f<=version) {
        [[UIApplication sharedApplication] setStatusBarStyle:UIStatusBarStyleLightContent];
    }
    
    bool iPhone5=false;
    CGSize result = [[UIScreen mainScreen] bounds].size;
    CGFloat scale = [UIScreen mainScreen].scale;
    result = CGSizeMake(result.width * scale, result.height * scale);
    
    UIApplication *app = [UIApplication sharedApplication];
    app.statusBarHidden=YES;

    NSLog(@"result.height %f", result.height);

    if (result.height == 1136.0 || result.height == 1704.0) {
        iPhone5=true;
        isIphone5=1;
    } else {
        iPhone5=false;
        isIphone5=0;
    }
    self.view=[[VGSScreenView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    // initialize VGS
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *docs_dir = [paths objectAtIndex:0];
    vgsint_setdir([docs_dir UTF8String]);
    NSString* fullPath;
    fullPath=[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"romdata.bin"];
    if(vgsint_init([fullPath UTF8String])) {
        _exit(-1);
    }
    // set vram window
    if(iPhone5) {
#if 1
        vram=[[VGSView alloc] initWithFrame:CGRectMake(0, 20, 320, 480)];
        [self.view addSubview:vram];
        vertical_zoom = 320.0 / 480.0;
        UIImage *imageA= [UIImage imageNamed:@"iPhone5.png"];
        UIImageView *ivA= [[UIImageView alloc] initWithImage:imageA];
        ivA.frame=CGRectMake(0,500,320,68);
        [self.view addSubview:ivA];
#else
        vram=[[VGSView alloc] initWithFrame:CGRectMake(0, 44+iOS7, 320, 416)];
        [self.view addSubview:vram];
        UIImage *imageA= [UIImage imageNamed:@"iPhone4.png"];
        UIImageView *ivA= [[UIImageView alloc] initWithImage:imageA];
        ivA.frame=CGRectMake(0,iOS7,320,44);
        [self.view addSubview:ivA];
        UIImage *imageB = [UIImage imageNamed:@"iPhone5.png"];
        UIImageView *ivB = [[UIImageView alloc] initWithImage:imageB];
        ivB.frame=CGRectMake(0,460+iOS7,320,88);
        [self.view addSubview:ivB];
#endif
    } else {
#if 0
        vram=[[VGSView alloc] initWithFrame:CGRectMake(0, 20, 320, 460)];
        [self.view addSubview:vram];
        vertical_zoom = 320.0 / 460.0;
#else
        vram=[[VGSView alloc] initWithFrame:CGRectMake(0, 20, 320, 416)];
        vertical_zoom = 320.0 / 416.0;
        [self.view addSubview:vram];
        UIImage *imageA= [UIImage imageNamed:@"iPhone4.png"];
        UIImageView *ivA= [[UIImageView alloc] initWithImage:imageA];
        ivA.frame=CGRectMake(0,416+20,320,44);
        [self.view addSubview:ivA];
#endif
    }

    // set status bar
    VGSBarView *bar=[[VGSBarView alloc] initWithFrame:CGRectMake(0,0,320,20)];
    [self.view addSubview:bar];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden
{
    return NO;
}

/*
- (UIStatusBarStyle)preferredStatusBarStyle
{
    return UIStatusBarStyleLightContent;
}*/

@end
