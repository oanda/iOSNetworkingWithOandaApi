//
//  OTNetworkLayerSpec.m
//  OTNetworkLayerTest
//
//  Created by Johnny Li, Adam Chan on 12-11-19.
//  Copyright (c) 2012 OANDA Corporation. All rights reserved.
//

#import "Kiwi.h"
#import "OTNetworkController.h"

SPEC_BEGIN(OTNetworkLayerSpec)

//const float nanosecondToSeconds = 1e9;

describe(@"The Network Controller", ^{

    OTNetworkController *networkController = [[OTNetworkController alloc] init];
    
    __block NSMutableArray *symbolsArray = nil;
    __block NSNumber *gAccountId = nil;
    
    context(@"when created", ^{
        it(@"should not be nil", ^{
            [networkController shouldNotBeNil];
        });
        
        context(@"when login in user and password", ^{

            it(@"should have one or more active accounts", ^{
                
                __block NSDictionary *account = nil;
                
                [networkController accountListForUsername:@"kyley"
                                                  success:^(NSDictionary *responseObject)
                 {
                     //NSLog(@"Success! %@", responseObject);
                     account = [[responseObject objectForKey:@"array"] lastObject];
                     gAccountId = [account valueForKey:@"id"];
                 } failure:^(NSDictionary *error) {
                     NSLog(@"Failure");
                 }];
                
                [[expectFutureValue(account) shouldEventually] beNonNil];
                [[expectFutureValue(gAccountId) shouldEventually] beNonNil];
            });
            
            it(@"should be able to query an active account", ^{
                
                __block NSDictionary *fetchedData = nil;
                
                [networkController accountStatusForAccountId:gAccountId
                                                   success:^(NSDictionary *responseObject)
                 {
                     //NSLog(@"Success! %@", responseObject);
                     fetchedData = responseObject;
                 } failure:^(NSDictionary *error) {
                     NSLog(@"Failure");
                 }];
                
                [[expectFutureValue(fetchedData) shouldEventually] beNonNil];
            });
        }); //context(@"when login in user and password"
        
        context(@"when working with Rates", ^{
            
            it(@"should receive the rates list asynchronously", ^{
                
                [networkController rateListSymbolsSuccess:^(NSDictionary *responseObject)
                {
                     //NSLog(@"Success!  %@", responseObject);
                     NSArray *listSymbols = [responseObject objectForKey:@"instruments"];
                     
                     // Build the array of strings to pass into rateQuote
                     symbolsArray = [[NSMutableArray alloc] initWithCapacity:listSymbols.count];
                     for (NSDictionary *symbolDict in listSymbols)
                     {
                         [symbolsArray addObject:[NSString stringWithString:[symbolDict valueForKey:@"instrument"]]];
                     }
                     
                 } failure:^(NSDictionary *error) {
                     NSLog(@"Failure");
                 }];
                
                [[expectFutureValue(symbolsArray) shouldEventually] haveCountOfAtLeast:1];
            });
            
            it(@"should receive quote for rates list asynchronously", ^{
                
                __block NSArray *listRates = nil;
                
                [networkController rateQuote:symbolsArray
                                   success:^(NSDictionary *responseObject)
                 {
                     //NSLog(@"Success!  %@", responseObject);
                     listRates = [responseObject objectForKey:@"prices"];
                     
                 } failure:^(NSDictionary *error) {
                     NSLog(@"Failure");
                 }];
                
                [[expectFutureValue(listRates) shouldEventually] haveCountOfAtLeast:1];
            });
            
            it(@"should receive a list of candles asynchronously", ^{
                
                __block NSArray *candlesList = nil;
                
             [networkController rateCandlesForSymbol:@"EUR_USD"
                                         granularity:@"S30"
                                         numberOfPoints:[NSNumber numberWithInt:5]
                                         success:^(NSDictionary *responseObject)
                 {
                     //NSLog(@"Success!  %@", responseObject);
                     candlesList = [responseObject objectForKey:@"candles"];
                     
                 } failure:^(NSDictionary *error) {
                     NSLog(@"Failure");
                 }];
                
                [[expectFutureValue(candlesList) shouldEventually] haveCountOfAtLeast:5];
            });
        }); //context(@"when working with Rates"
        
        context(@"when asking for reports", ^{
            
            it(@"should receive the list of transactions asynchronously", ^{
                
                __block NSDictionary *fetchedData = nil;
                
                [networkController transactionListForAccountId:gAccountId
                                                     success:^(NSDictionary *responseObject)
                 {
                     //NSLog(@"Success!  %@", responseObject);
                     fetchedData = responseObject;
                     
                 } failure:^(NSDictionary *error) {
                     NSLog(@"Failure");
                 }];
                
                [[expectFutureValue(fetchedData) shouldEventually] beNonNil];
            });
            
            it(@"should receive the list of trades asynchronously", ^{
                
                __block NSDictionary *fetchedData = nil;
                
                [networkController tradesListForAccountId:gAccountId
                                                     success:^(NSDictionary *responseObject)
                 {
                     //NSLog(@"Success!  %@", responseObject);
                     fetchedData = responseObject;
                     
                 } failure:^(NSDictionary *error) {
                     NSLog(@"Failure");
                 }];
                
                [[expectFutureValue(fetchedData) shouldEventually] beNonNil];
            });
            
            it(@"should receive the list of open orders asynchronously", ^{
                
                __block NSDictionary *fetchedData = nil;
                
                [networkController ordersListForAccountId:gAccountId
                                                success:^(NSDictionary *responseObject)
                 {
                     //NSLog(@"Success!  %@", responseObject);
                     fetchedData = responseObject;
                     
                 } failure:^(NSDictionary *error) {
                     NSLog(@"Failure");
                 }];
                
                [[expectFutureValue(fetchedData) shouldEventually] beNonNil];
            });
            
            it(@"should receive the list of price alerts asynchronously", ^{
                
                __block NSDictionary *fetchedData = nil;
                
                [networkController priceAlertsListForAccountId:gAccountId
                                                success:^(NSDictionary *responseObject)
                 {
                     //NSLog(@"Success!  %@", responseObject);
                     fetchedData = responseObject;
                     
                 } failure:^(NSDictionary *error) {
                     NSLog(@"Failure");
                 }];
                
                [[expectFutureValue(fetchedData) shouldEventually] beNonNil];
            });
            
            it(@"should receive the list of open positions asynchronously", ^{
                
                __block NSDictionary *fetchedData = nil;
                
                [networkController positionsListForAccountId:gAccountId
                                                success:^(NSDictionary *responseObject)
                 {
                     //NSLog(@"Success!  %@", responseObject);
                     fetchedData = responseObject;
                     
                 } failure:^(NSDictionary *error) {
                     NSLog(@"Failure");
                 }];
                
                [[expectFutureValue(fetchedData) shouldEventually] beNonNil];
            });

            
        }); //context(@"when asking for reports"
        
        
        
        
        
    }); //context(@"when created"
    
});

SPEC_END
