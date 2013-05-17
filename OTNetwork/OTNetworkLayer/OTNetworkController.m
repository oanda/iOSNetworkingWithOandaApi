//
//  OTNetworkController.m
//  OTNetworkLayer
//
//  Created by Johnny Li, Adam Chan on 12-11-12.
//  Copyright (c) 2012 OANDA Corporation. (http://www.oanda.com/)
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#import "OTNetworkController.h"
//#import "AFJSONRequestOperation.h"
#import "AFHTTPRequestOperation.h"
#import "JSONKit.h"

// TODO: for now we keep all these properties as private, need to review overall design to decide which to expose, if any.
@interface OTNetworkController ()

@property (atomic, strong) AFHTTPClient *afc;
@property (atomic, copy) NSString *userName;
@property (atomic, copy) NSString *userAccountId;
//@property (atomic, copy) NSString *userPassword;
@property (nonatomic, copy) NSString *serverUrl;
@end

static NSDateFormatter *sRFC3339DateFormatter;

@implementation OTNetworkController

//#define USE_JSONKIT     // comment out to parse with iOS5 NSJSONSerialization


- (id)init
{
    self = [super init];
    if (self) {
        _serverUrl = @"http://api-sandbox.oanda.com/v1/";
        
        NSURL *url = [NSURL URLWithString:_serverUrl];
        _afc = [AFHTTPClient clientWithBaseURL:url];
    }
    
    return self;
}

#pragma mark Accessing and Managing User Accounts

- (void)accountListForUsername:(NSString *)username
                       success:(NetworkSuccessBlock)successBlock
                       failure:(NetworkFailBlock)failureBlock
{
    // TODO: authentication has been temporarily disabled (ie. do not call userLogin).  For now, we use the following hardcoded account value:
    _userName = username;
    
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    NSString *pathString = [@"users" stringByAppendingFormat:@"/%@/accounts", _userName];
    [_afc getPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // parse and extract the list from the JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSArray* jsonArray = [decoder objectWithData:responseObject];
        NSAssert1(jsonArray, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSArray *jsonArray = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonArray, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        NSDictionary *jsonDict = [NSDictionary dictionaryWithObject:jsonArray forKey:@"array"];
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)accountStatusForAccountId:(NSNumber *)accountId
                          success:(NetworkSuccessBlock)successBlock
                          failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@", [accountId stringValue]];
    [_afc getPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // parse and return the whole response, which represents the whole status info
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary* jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

#pragma mark Quoting Tradable Instruments
- (void)rateListSymbolsSuccess:(NetworkSuccessBlock)successBlock
                       failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    [_afc getPath:@"instruments" parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // extract the list of all symbol pairs available for trading
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary* jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)rateQuote:(NSArray *)symbolPairList
          success:(NetworkSuccessBlock)successBlock
          failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    // extract from passed-in strings, construct the list of symbol lists as a single string
    NSString *symbolsString = @"";
    for (NSString *symbol in symbolPairList)
    {
        symbolsString = [symbolsString stringByAppendingString:symbol];
        symbolsString = [symbolsString stringByAppendingString:@","];
    }
    symbolsString = [symbolsString substringToIndex:[symbolsString length] - 1];
    [parameters setObject:symbolsString forKey:@"instruments"];
    
    [_afc getPath:@"instruments/price"
        parameters:parameters
           success:^(AFHTTPRequestOperation *operation, id responseObject)
     {
         // extract the list of prices, then pass it up the chain
#if defined(USE_JSONKIT)
         JSONDecoder* decoder = [[JSONDecoder alloc]
                                 initWithParseOptions:JKParseOptionNone];
         NSDictionary* jsonDict = [decoder objectWithData:responseObject];
         NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
         NSError *error = nil;
         NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
         NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
         
         successBlock(jsonDict);
         
     } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
         [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
     }];
}

- (void)rateCandlesForSymbol:(NSString *)symbol
                 granularity:(NSString *)granularity
              numberOfPoints:(NSNumber *)count
                     success:(NetworkSuccessBlock)successBlock
                     failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
	//[parameters setObject:symbol forKey:@"symbol"];
	[parameters setObject:granularity forKey:@"granularity"];
    
    if (count)
    {
        [parameters setObject:[count stringValue] forKey:@"count"];
    }
    
    NSString *pathString = [NSString stringWithFormat:@"instruments/%@/candles", symbol];
    [_afc getPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // return the whole parsed JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary* jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

#pragma mark Getting Reports on Past and Current Activities
- (void)transactionListForAccountId:(NSNumber *)accountId
                            success:(NetworkSuccessBlock)successBlock
                            failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
	
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/transactions", [accountId stringValue]];    
    [_afc getPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // parse and extract the list from the JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary* jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)tradesListForAccountId:(NSNumber *)accountId
                       success:(NetworkSuccessBlock)successBlock
                       failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/trades", [accountId stringValue]];
    [_afc getPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // parse and extract the list from the JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary *jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)ordersListForAccountId:(NSNumber *)accountId
                       success:(NetworkSuccessBlock)successBlock
                       failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/orders", [accountId stringValue]];
    [_afc getPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // parse and extract the list from the JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary *jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)priceAlertsListForAccountId:(NSNumber *)accountId
                            success:(NetworkSuccessBlock)successBlock
                            failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];

    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/alerts", [accountId stringValue]];
    [_afc getPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // parse and extract the list from the JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary *jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)positionsListForAccountId:(NSNumber *)accountId
                          success:(NetworkSuccessBlock)successBlock
                          failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
	[parameters setObject:[accountId stringValue] forKey:@"account_id"];
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/positions", [accountId stringValue]];
    [_afc getPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // parse and extract the list from the JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary *jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)rateLimitsListSuccess:(NetworkSuccessBlock)successBlock
                      failure:(NetworkFailBlock)failureBlock
{
    // TODO: OANDA API currently does not support this feature
    /*
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/limits", [accountId stringValue]];
    [_afc getPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // parse and extract the list from the JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary *jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
     } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
     }];
    */
}

#pragma mark Handling Positions
- (void)closePositionForAccount:(NSNumber *)accountId
                         symbol:(NSString *)symbol
                          price:(NSDecimalNumber *)price
                        success:(NetworkSuccessBlock)successBlock
                        failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    // set the mandatory params
	[parameters setObject:[accountId stringValue] forKey:@"account_id"];
    
    // set the optional params
	if (price) {
        [parameters setObject:[price description] forKey:@"price"];
	}
    
    [_afc postPath:@"position/close.json" parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // return the whole parsed JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary* jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

#pragma mark Creating and Managing LimitOrders
- (void)createOrderForAccount:(NSNumber *)accountId
                       symbol:(NSString *)symbol
                        units:(NSNumber *)units
                         type:(NSString *)type
                        price:(NSDecimalNumber *)price
                       expiry:(NSNumber *)expiryInSeconds
            minExecutionPrice:(NSDecimalNumber *)lowPrice
            maxExecutionPrice:(NSDecimalNumber *)highPrice
                     stopLoss:(NSDecimalNumber *)stopLoss
                   takeProfit:(NSDecimalNumber *)takeProfit
                 trailingStop:(NSDecimalNumber *)trailingStop
                      success:(NetworkSuccessBlock)successBlock
                      failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    // set the mandatory params
	[parameters setObject:symbol forKey:@"instrument"];
    [parameters setObject:[units stringValue] forKey:@"units"];
    [parameters setObject:[NSString stringWithFormat:@"%.5f", [price floatValue]] forKey:@"price"];
	[parameters setObject:type forKey:@"side"];

    NSString *expiryTimeTemp = [NSString stringWithFormat:@"%ld", (long)[[NSDate date] timeIntervalSince1970] + [expiryInSeconds intValue]];
    NSString *expiryTime = [self dateFromRFC3339Date:expiryTimeTemp];

    //[parameters setObject:expiryTime forKey:@"expiry"];
    [parameters setObject:@"2013-05-17T22%3A12%3A34Z" forKey:@"expiry"];
    
    // set the optional params
	if (lowPrice) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [lowPrice floatValue]] forKey:@"lowLimit"];
	}
	if (highPrice) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [highPrice floatValue]] forKey:@"highLimit"];
	}
	if (stopLoss) {
		[parameters setObject:[NSString stringWithFormat:@"%.5f", [stopLoss floatValue]] forKey:@"stopLoss"];
	}
	if (takeProfit) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [takeProfit floatValue]] forKey:@"takeProfit"];
	}
	if (trailingStop) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [trailingStop floatValue]] forKey:@"trailingStop"];
	}
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/orders", [accountId stringValue]];
    [_afc postPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // return the whole parsed JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary* jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)changeOrderForAccount:(NSNumber *)accountId
                      orderId:(NSNumber *)orderId
                       symbol:(NSString *)symbol
                        units:(NSNumber *)units
                         type:(NSString *)type
                        price:(NSDecimalNumber *)price
                       expiry:(NSNumber *)expiryInSeconds
            minExecutionPrice:(NSDecimalNumber *)lowPrice
            maxExecutionPrice:(NSDecimalNumber *)highPrice
                     stopLoss:(NSDecimalNumber *)stopLoss
                   takeProfit:(NSDecimalNumber *)takeProfit
                 trailingStop:(NSDecimalNumber *)trailingStop
                      success:(NetworkSuccessBlock)successBlock
                      failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    // set the mandatory params
	[parameters setObject:symbol forKey:@"instrument"];
    [parameters setObject:[units stringValue] forKey:@"units"];
    [parameters setObject:[NSString stringWithFormat:@"%.5f", [price floatValue]] forKey:@"price"];
	[parameters setObject:type forKey:@"side"];
    
    NSString *expiryTime = [NSString stringWithFormat:@"%ld", (long)[[NSDate date] timeIntervalSince1970] + [expiryInSeconds intValue]];
    [parameters setObject:expiryTime forKey:@"expiry"];
    
    // set the optional params
	if (lowPrice) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [lowPrice floatValue]] forKey:@"lowLimit"];
	}
	if (highPrice) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [highPrice floatValue]] forKey:@"highLimit"];
	}
	if (stopLoss) {
		[parameters setObject:[NSString stringWithFormat:@"%.5f", [stopLoss floatValue]] forKey:@"stopLoss"];
	}
	if (takeProfit) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [takeProfit floatValue]] forKey:@"takeProfit"];
	}
	if (trailingStop) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [trailingStop floatValue]] forKey:@"trailingStop"];
	}
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/orders/%@", [accountId stringValue], [orderId stringValue]];
    [_afc putPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // the response would be empty in this case
        successBlock(nil);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)pollOrderForAccount:(NSNumber *)accountId
                 maxOrderId:(NSNumber *)maxOrderId
                    success:(NetworkSuccessBlock)successBlock
                    failure:(NetworkFailBlock)failureBlock
{
    // TODO: for now it is same as ordersList, except that you could pass in a maxOrderId.
    // TODO: consider creating another function that checks for a specific order Id
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    // set the mandatory params
	[parameters setObject:[maxOrderId stringValue] forKey:@"maxOrderId"];
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/orders", [accountId stringValue]];
    [_afc getPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // return the whole parsed JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary* jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)deleteOrderForAccount:(NSNumber *)accountId
                      orderId:(NSNumber *)orderId
                      success:(NetworkSuccessBlock)successBlock
                      failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/orders/%@", [accountId stringValue], [orderId stringValue]];
    [_afc deletePath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // return the whole parsed JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary* jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

#pragma mark Creating and Managing MarketOrders Trades
- (void)openTradeForAccount:(NSNumber *)accountId
                     symbol:(NSString *)symbol
                      units:(NSNumber *)units
                       type:(NSString *)type
                      price:(NSDecimalNumber *)price
          minExecutionPrice:(NSDecimalNumber *)lowPrice
          maxExecutionPrice:(NSDecimalNumber *)highPrice
                   stopLoss:(NSDecimalNumber *)stopLoss
                 takeProfit:(NSDecimalNumber *)takeProfit
               trailingStop:(NSDecimalNumber *)trailingStop
                    success:(NetworkSuccessBlock)successBlock
                    failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    // set the mandatory params
	[parameters setObject:symbol forKey:@"instrument"];
    [parameters setObject:[units stringValue] forKey:@"units"];
    
    // set the optional params
    if (price) {
		[parameters setObject:[NSString stringWithFormat:@"%.5f", [price floatValue]] forKey:@"price"];
 	}
    if (type) {
        [parameters setObject:type forKey:@"side"];
    }
	if (lowPrice) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [lowPrice floatValue]] forKey:@"lowLimit"];
	}
	if (highPrice) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [highPrice floatValue]] forKey:@"highLimit"];
	}
	if (stopLoss) {
		[parameters setObject:[NSString stringWithFormat:@"%.5f", [stopLoss floatValue]] forKey:@"stopLoss"];
	}
	if (takeProfit) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [takeProfit floatValue]] forKey:@"takeProfit"];
	}
	if (trailingStop) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [trailingStop floatValue]] forKey:@"trailingStop"];
	}
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/trades", [accountId stringValue]];
    [_afc postPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // return the whole parsed JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary* jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)changeTradeForAccount:(NSNumber *)accountId
                      tradeId:(NSNumber *)tradeId
                     stopLoss:(NSDecimalNumber *)stopLoss
                   takeProfit:(NSDecimalNumber *)takeProfit
                 trailingStop:(NSDecimalNumber *)trailingStop
                      success:(NetworkSuccessBlock)successBlock
                      failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
        
    // set the optional params
	if (stopLoss) {
		[parameters setObject:[NSString stringWithFormat:@"%.5f", [stopLoss floatValue]] forKey:@"stopLoss"];
	}
	if (takeProfit) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [takeProfit floatValue]] forKey:@"takeProfit"];
	}
	if (trailingStop) {
        [parameters setObject:[NSString stringWithFormat:@"%.5f", [trailingStop floatValue]] forKey:@"trailingStop"];
	}
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/trades/%@", [accountId stringValue], [tradeId stringValue]];
    [_afc putPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // the response would be empty in this case
        successBlock(nil);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)pollTradeForAccount:(NSNumber *)accountId
                 maxTradeId:(NSNumber *)maxTradeId
                    success:(NetworkSuccessBlock)successBlock
                    failure:(NetworkFailBlock)failureBlock
{
    // TODO: for now it is same as ordersList, except that you could pass in a maxTradeId.
    // TODO: consider creating another function that checks for a specific trade Id
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];
    
    // set the mandatory params
	[parameters setObject:[maxTradeId stringValue] forKey:@"maxTradeId"];
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/trades", [accountId stringValue]];
    [_afc getPath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // return the whole parsed JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary* jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

- (void)closeTradeForAccount:(NSNumber *)accountId
                     tradeId:(NSNumber *)tradeId
                       price:(NSDecimalNumber *)price
                     success:(NetworkSuccessBlock)successBlock
                     failure:(NetworkFailBlock)failureBlock
{
    NSMutableDictionary *parameters;
    parameters = [self setupDefaultParams];

    // set the optional param
    if (price) {
		[parameters setObject:[NSString stringWithFormat:@"%.5f", [price floatValue]] forKey:@"price"];
 	}
    
    NSString *pathString = [NSString stringWithFormat:@"accounts/%@/trades/%@", [accountId stringValue], [tradeId stringValue]];
    [_afc deletePath:pathString parameters:parameters success:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        // return the whole parsed JSON object
#if defined(USE_JSONKIT)
        JSONDecoder* decoder = [[JSONDecoder alloc]
                                initWithParseOptions:JKParseOptionNone];
        NSDictionary* jsonDict = [decoder objectWithData:responseObject];
        NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
        NSError *error = nil;
        NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:responseObject options: NSJSONReadingMutableContainers error:&error];
        NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
        
        successBlock(jsonDict);
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        [self handleFailureUsingBlock:failureBlock withOperation:operation withError:error];
    }];
}

///////////////////////////////////////////////////////////////
//
// Helper functions
//
///////////////////////////////////////////////////////////////
#pragma mark Helper/Private functions
/*
-(void)setupHeaderDefaults:(NSDictionary *)jsonDict
{
    //[_afc setDefaultHeader:kSessionToken value:sessionToken];
    
    _userName = [jsonDict objectForKey:@"username"];
    _userPassword = [jsonDict objectForKey:@"password"];
    
    [_afc setAuthorizationHeaderWithUsername:_userName password:_userPassword];
}
*/

- (NSMutableDictionary *)setupDefaultParams
{
    NSMutableDictionary *parameters = [NSMutableDictionary dictionary];
    
    // TODO: OANDA Open API currently does not require authentication
    /*
	[parameters setObject:_userName forKey:@"username"];
    [parameters setObject:_userPassword forKey:@"password"];
    */
    return parameters;
}

- (void) handleFailureUsingBlock:(NetworkFailBlock)failureBlock
                   withOperation:(AFHTTPRequestOperation *)operation
                       withError:(NSError *)error
{
    // parse and extract the struct describing the error
#if defined(USE_JSONKIT)
    NSDictionary *jsonDict = [operation.responseString objectFromJSONString];
    NSAssert1(jsonDict, @"%@: Error parsing with JSONKit", [self class]);
#else
    NSError *jsonParseError = nil;
    NSDictionary *jsonDict =
    [NSJSONSerialization JSONObjectWithData: [operation.responseString dataUsingEncoding:NSUTF8StringEncoding]
                                    options: NSJSONReadingMutableContainers
                                      error: &jsonParseError];
    NSAssert2(jsonDict, @"%@: Error parsing JSON: %@", [self class], [error localizedDescription]);
#endif
    
    NSMutableDictionary *returnDict = [jsonDict mutableCopy];
    [returnDict setObject:[NSNumber numberWithInteger:[operation.response statusCode]] forKey:@"http status code"];
    [returnDict setObject:error forKey:@"net error"];
    
    NSLog(@"%@ FAILURE : %@", NSStringFromSelector(_cmd), returnDict);
    
    failureBlock(returnDict);
}

-(NSString *)dateFromRFC3339Date:(NSString *)date
{
    if (sRFC3339DateFormatter == nil) {
        sRFC3339DateFormatter = [[NSDateFormatter alloc] init];
        NSAssert(sRFC3339DateFormatter != nil, @"Could not allocate NSDateFormatter");
        [sRFC3339DateFormatter setDateFormat:@"yyyy'-'MM'-'dd'T'HH':'mm':'ss'Z'"];
        [sRFC3339DateFormatter setTimeZone:[NSTimeZone timeZoneForSecondsFromGMT:0]];
        [sRFC3339DateFormatter setLocale:[[NSLocale alloc] initWithLocaleIdentifier:@"en_US_POSIX"]];
    }

    NSDate *dateD = [[NSDate alloc] initWithTimeIntervalSince1970:[date doubleValue]];
    return [sRFC3339DateFormatter stringFromDate:dateD];
}

@end
