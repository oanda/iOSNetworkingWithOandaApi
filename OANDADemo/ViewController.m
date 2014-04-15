//
//  ViewController.m
//  OANDADemo
//
//  Created by Jack Xu on 4/8/14.
//  Copyright (c) 2014 OANDA. All rights reserved.
//

#import "ViewController.h"
#import "OTNetworkController.h"

@interface ViewController ()
@property (nonatomic,strong) NSArray *array;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    OTNetworkController *testController = [[OTNetworkController alloc]init];
    
    [self.networkDelegate rateListSymbolsSuccess:^(NSDictionary *responseObject)
     {
         //NSLog(@"Success!  %@", responseObject);
         self.listSymbols = [responseObject objectForKey:@"instruments"];
         
         // Build the array of strings to pass into rateQuote
         self.symbolsArray = [[NSMutableArray alloc] initWithCapacity:self.listSymbols.count];
         for (NSDictionary *symbolDict in self.listSymbols)
         {
             [self.symbolsArray addObject:[NSString stringWithString:[symbolDict valueForKey:@"instrument"]]];
         }
         
         allowRatesFetching = YES;
     } failure:^(NSDictionary *error) {
         NSLog(@"Failure");
     }]
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
