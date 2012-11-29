//
//  OTAppDelegate.h
//  OTNetworkLayer
//
//  Created by Johnny Li, Adam Chan on 12-11-12.
//  Copyright (c) 2012 OANDA Corporation. All rights reserved.
//

#import <UIKit/UIKit.h>


@class OTNetworkController;

@interface OTAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) OTNetworkController* networkController;

@end
