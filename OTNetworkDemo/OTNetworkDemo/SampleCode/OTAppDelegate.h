//
//  OTAppDelegate.h
//  OTNetworkLayer
//
//  Created by Johnny Li on 12-11-12.
//  Copyright (c) 2012 Johnny Li. All rights reserved.
//

#import <UIKit/UIKit.h>


@class OTNetworkController;

@interface OTAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) OTNetworkController* networkController;

@end
