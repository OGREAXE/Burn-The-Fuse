//
//  InlineAppstore.m
//  FusePuzzle
//
//  Created by 梁志远 on 11/27/14.
//
//

#import "InlineAppstore.h"
#import "AppDelegate.h"
#import "AppController.h"
#import <StoreKit/StoreKit.h>
#import <Foundation/Foundation.h>

@interface InlineAppstore : NSObject
+ (InlineAppstore*)sharedInstance;

- (void)showAppInApp:(NSString *)_appId;
@end

@implementation InlineAppstore
+ (InlineAppstore*)sharedInstance {
    static InlineAppstore* pInlineAppstore = nil;
    
    @synchronized(self) {
        if (pInlineAppstore == nil) {
            pInlineAppstore = [[self alloc] init];
        }
    }
    return pInlineAppstore;
}

- (void)showAppInApp:(NSString *)_appId {

    NSString *webLink = [NSString stringWithFormat:@"itms-apps://phobos.apple.com/WebObjects/MZStore.woa/wa/viewSoftware?id=%@",_appId];
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:webLink]];
}

@end

CCAppStore * CCAppStore::_appStore = NULL;
void CCAppStore::ShowInAppstore(char * app_id)
{
    [[InlineAppstore sharedInstance]showAppInApp:[NSString stringWithCString:app_id]];
}
