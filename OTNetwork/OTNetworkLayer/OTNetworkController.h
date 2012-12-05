//
//  OTNetworkController.h
//  OTNetworkLayer
//
//  Created by Johnny Li, Adam Chan on 12-11-12.
//  Copyright (c) 2012 OANDA Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AFHTTPClient.h"

#define REST_API_VERSION @"v1"
#define kSessionToken @"session_token"
#define kUserName @"username"

typedef void (^NetworkSuccessBlock)(NSDictionary *result);
typedef void (^NetworkFailBlock)(NSDictionary *error); //(NSDictionary *error);

/** This class is a wrapper for low level REST API network calls, and is meant to provide a consistent means for higher networking layers to send and receive data.
  
 As seen from all the methods, the intention is for the higher level caller to pass in
 
 - all required and optional parameters, as needed for the particular network request
 - a SuccessBlock
 - a FailBlock
 
 Please be aware of these typedef for the blocks mentioned:
 
    typedef void (^NetworkSuccessBlock)(NSDictionary *result);
    typedef void (^NetworkFailBlock)(NSDictionary *error);
 
 If the network replies our request by delivering the data we seek, the data is converted to NSDictionary, and
 the SuccessBlock will be triggered with this NSDictionary passed in as argument.
 
 Similarly, a failed network request will trigger the FailBlock with an NSDictionary describing the error.  An example of this structure would look this:
     {
        code = 9;
        "http status code" = 500;
        message = "Internal Server Error";
        "net error" = "Error Domain=AFNetworkingErrorDomain Code=-1011 \"Expected status code in (200-299), got 500\" 
            UserInfo=0x905bd60 {NSLocalizedRecoverySuggestion={\n\t\"code\" : 9,\n\t\"message\" : \"Internal 
            Server Error\"\n} \n, AFNetworkingOperationFailingURLRequestErrorKey=<NSMutableURLRequest 
            http://api-sandbox.oanda.com/users/kyley1/accounts?>, NSErrorFailingURLKey=http://api-sandbox.
            oanda.com/users/kyley1/accounts?, NSLocalizedDescription=Expected status code in (200-299), got 500, 
            AFNetworkingOperationFailingURLResponseErrorKey=<NSHTTPURLResponse: 0x71465d0>}";
     }

 where:
    "code" : OANDA error code, may or may not be the same as the HTTP status code
    "http status code" : response of the HTTP request to the network
    "message" : a description of the error which occurred, intended for developers
    "net error" : full error string received, intended for developers
 
 For additional information on Objective-C blocks, please refer to
 http://developer.apple.com/library/ios/documentation/Cocoa/Conceptual/Blocks/
 
 Although it is not a singleton, we strongly discourage having multiple instances operating simultaneously, because their competition to access the same network would result in us not being able to guarantee an acceptable Quality of Service.
*/
@interface OTNetworkController : NSObject

#pragma mark Accessing and Managing User Accounts
/** @name Accessing and Managing User Accounts */

/** To retrieve a list of accounts belonging to the user.

 @param username **Required**.  Name of the user to obtain the accounts from.  NOTE: this will likely be removed once proper login/authentication is supported.
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing
 back an NSDictionary* as argument.  The NSDictionary should contain an array of NSDictionary, each describing an account belonging to the user.
 @return Example of a returned NSDictionary:
     {
         array =     (
             {
                 accountPropertyName = (
                 );
                 homecurr = USD;
                 id = 506005;
                 marginRate = "0.05";
                 name = Primary;
             }
         );
     }
 
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 */
- (void)accountListForUsername:(NSString *)username
                       success:(NetworkSuccessBlock)successBlock
                       failure:(NetworkFailBlock)failureBlock;

/** To retrieve a particular account's current status.
 
 @param accountId **Required**. ID of the account to get status of (must be owned by the user).
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain details describing an account belonging to the user.
 @return Example of a returned NSDictionary:
     {
         accountId = 506005;
         accountName = Primary;
         balance = "99997.7752";
         homecurr = USD;
         marginAvail = "99997.7129";
         marginRate = "0.05";
         marginUsed = "0.0649";
         openOrders = 0;
         openTrades = 1;
         unrealizedPl = "0.0026";
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see accountListForUsername:success:failure:
 */
- (void)accountStatusForAccountId:(NSNumber*)accountId
                          success:(NetworkSuccessBlock)successBlock
                          failure:(NetworkFailBlock)failureBlock;


#pragma mark Quoting Tradable Instruments
/** @name Quoting Tradable Instruments */

/** To retrieve a list of tradable symbol pairs.
 
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of symbols the client can trade on and where the pip location and pippettes are for that pair.
 @return Example of a returned NSDictionary:
     {
         instruments =     (
             {
                 displayName = "EUR/USD";
                 instrument = "EUR_USD";
                 maxTradeUnits = 1000000;
                 pip = "0.0001";
             },
             {
                 displayName = "GBP/CAD";
                 instrument = "GBP_CAD";
                 maxTradeUnits = 10000000;
                 pip = "0.0001";
             },
             {
                 displayName = Soybeans;
                 instrument = "SOYBN_USD";
                 maxTradeUnits = 60000;
                 pip = "0.01";
             },
             {
                 displayName = "West Texas Oil";
                 instrument = "WTICO_USD";
                 maxTradeUnits = 10000;
                 pip = "0.01";
             },
             {
                 displayName = "XXX/USD";
                 instrument = "XXX_USD";
                 maxTradeUnits = 10;
                 pip = "0.0001";
             }
         );
     }
 
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see rateQuote:success:failure:
 */
- (void)rateListSymbolsSuccess:(NetworkSuccessBlock)successBlock
                       failure:(NetworkFailBlock)failureBlock;

/** To retrieve the current market rate for a set of symbols.
 
 @param symbolPairList **Required**.  An NSArray of NSStrings, representing the symbols to retrieve prices for.  An example of this list would look like this:
     (
         AUD/JPY,
         CAD/HKD,
         EUR/USD,
         XAU/USD
     )
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of symbols the client can trade on and where the pip location and pippettes are for that pair.
 @return Example of a returned NSDictionary:
     {
         prices =     (
             {
                 ask = "85.57299999999999";
                 bid = "85.533";
                 instrument = "AUD_JPY";
                 time = "1354208555.370971";
             },
             {
                 ask = "7.8087";
                 bid = "7.80622";
                 instrument = "CAD_HKD";
                 time = "1354208555.354307";
             },
             {
                 ask = "1.29596";
                 bid = "1.29564";
                 instrument = "EUR_USD";
                 time = "1354208555.548539";
             },
             {
                 ask = "1725.789";
                 bid = "1725.389";
                 instrument = "XAU_USD";
                 time = "1354208555.47021";
             }
         );
     }
 
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see rateListSymbolsSuccess:failure:
 */
- (void)rateQuote:(NSArray *)symbolPairList
          success:(NetworkSuccessBlock)successBlock
          failure:(NetworkFailBlock)failureBlock;

/** To retrieve the historical pricing for a symbol (ie. candles).
 
 @param symbol **Required**.  Which symbol to retrieve prices for (eg. EUR/USD)
 @param granularity **Optional**.  Specifies the pricing interval to use. This must be one of the "named" THS granularities which include:
        Second-based: S5,S10,S15,S30
        Minute-based: M1,M2,M3,M4,M5,M10,M15,M30
        Hour-based: H1,H2,H3,H4,H6,H8,H12
        Daily: D
        Weekly: W
        Monthly: M The default for granularity is "S5"
 @param count **Optional**.  Specifies the maximum number of price points to return. Default is 500, max is 5000.
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of past prices for the given symbol pair (ie. candles).
 @return Example of a returned NSDictionary:
     {
         candles =     (
             {
                 closeMid = "1.29757";
                 complete = true;
                 highMid = "1.29757";
                 lowMid = "1.29757";
                 openMid = "1.29757";
                 time = 1354215210;
             },
             {
                 closeMid = "1.29763";
                 complete = true;
                 highMid = "1.29763";
                 lowMid = "1.2976";
                 openMid = "1.2976";
                 time = 1354215300;
             },
             {
                 closeMid = "1.29766";
                 complete = true;
                 highMid = "1.29766";
                 lowMid = "1.29766";
                 openMid = "1.29766";
                 time = 1354215330;
             },
             {
                 closeMid = "1.29762";
                 complete = true;
                 highMid = "1.29763";
                 lowMid = "1.29759";
                 openMid = "1.29763";
                 time = 1354215360;
             },
             {
                 closeMid = "1.29759";
                 complete = false;
                 highMid = "1.29759";
                 lowMid = "1.29759";
                 openMid = "1.29759";
                 time = 1354215420;
             }
         );
         granularity = S30;
         instrument = "EUR_USD";
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see rateQuote:success:failure:
 */
- (void)rateCandlesForSymbol:(NSString *)symbol
                 granularity:(NSString *)granularity
              numberOfPoints:(NSNumber *)count
                     success:(NetworkSuccessBlock)successBlock
                     failure:(NetworkFailBlock)failureBlock;


#pragma mark Getting Reports on Past and Current Activities
/** @name Getting Reports on Past and Current Activities */

/** To retrieve the recent transactions for the given account.
 
 @param accountId **Required**.  Account Id to get transactions of (must be owned by the user).
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of NSDictionary, each describing a past transaction.
 @return Example of a returned NSDictionary:
     {
         nextPage = "http://api-sandbox.oanda.com/accounts/506005/transactions?maxTransId=177809412";
         transactions =     (
             {
                 accountId = 506005;
                 amount = "1.29428";
                 balance = "99997.9978";
                 completionCode = 107;
                 diaspora = 0;
                 duration = 0;
                 highOrderLimit = 0;
                 id = 177809672;
                 instrument = "EUR/USD";
                 interest = 0;
                 lowOrderLimit = 0;
                 marginUsed = 0;
                 orderLink = 0;
                 price = 0;
                 profitLoss = 0;
                 stopLoss = 0;
                 takeProfit = 0;
                 time = 1354136400;
                 trailingStop = 0;
                 transactionLink = 0;
                 type = Interest;
                 units = 1;
             },
             {
                 accountId = 506005;
                 amount = 0;
                 balance = 0;
                 completionCode = 200;
                 diaspora = 0;
                 duration = 0;
                 highOrderLimit = 0;
                 id = 177809413;
                 instrument = na;
                 interest = 0;
                 lowOrderLimit = 0;
                 marginUsed = 0;
                 orderLink = 0;
                 price = 0;
                 profitLoss = 0;
                 stopLoss = 0;
                 takeProfit = 0;
                 time = 1354025883;
                 trailingStop = 0;
                 transactionLink = 0;
                 type = CreateAccount;
                 units = 0;
             }
         );
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 */
- (void)transactionListForAccountId:(NSNumber *)accountId
                            success:(NetworkSuccessBlock)successBlock
                            failure:(NetworkFailBlock)failureBlock;

/** To retrieve the open trades for the given account.
 
 @param accountId **Required**.  Account Id to get trades of (must be owned by the user).
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of NSDictionary, each describing an open trade.
 @return Example of a returned NSDictionary:
     {
         nextPage = "http://api-sandbox.oanda.com/accounts/506005/trades?maxTradeId=177809414";
         trades =     (
             {
                 direction = long;
                 id = 177809801;
                 instrument = "AUD/JPY";
                 price = "85.568";
                 stopLoss = 0;
                 takeProfit = 0;
                 time = 1354212801;
                 trailingStop = 0;
                 units = 456;
             },
             {
                 direction = long;
                 id = 177809415;
                 instrument = "EUR/USD";
                 price = "1.29428";
                 stopLoss = 0;
                 takeProfit = 0;
                 time = 1354025935;
                 trailingStop = 0;
                 units = 1;
             }
         );
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see openTradeForAccount:symbol:units:type:price:minExecutionPrice:maxExecutionPrice:stopLoss:takeProfit:trailingStop:success:failure:
 */
- (void)tradesListForAccountId:(NSNumber *)accountId
                       success:(NetworkSuccessBlock)successBlock
                       failure:(NetworkFailBlock)failureBlock;

/** To retrieve the open orders for the given account.
 
 @param accountId **Required**.  Account Id to get orders of (must be owned by the user).
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of NSDictionary, each describing an open order.
 @return Example of a returned NSDictionary:
     {
         nextPage = "http://api-sandbox.oanda.com/accounts/506005/orders?maxOrderId=177809794";
         orders =     (
             {
                 direction = long;
                 expiry = 1354212879;
                 highLimit = 0;
                 id = 177809795;
                 instrument = "EUR/GBP";
                 lowLimit = 0;
                 ocaGroupId = 0;
                 price = "0.80443";
                 stopLoss = 0;
                 takeProfit = 0;
                 time = 1354212278;
                 trailingStop = 0;
                 units = 123;
             }
         );
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see createOrderForAccount:symbol:units:type:price:expiry:minExecutionPrice:maxExecutionPrice:stopLoss:takeProfit:trailingStop:success:failure:
 */
- (void)ordersListForAccountId:(NSNumber *)accountId
                       success:(NetworkSuccessBlock)successBlock
                       failure:(NetworkFailBlock)failureBlock;

/** To retrieve the open price alerts for the given account.
 
 @param accountId **Required**.  Account Id to get price alerts of (must be owned by the user).
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of NSDictionary, each describing an open trade.
 @return Example of a returned NSDictionary:
     {
         "alerts" =
         (
             {
                 id = 12345;
                 "type" = "PriceAlert";
                 "symbol" = "EUR/USD";
                 "time" = 1234567891;
                 "price_type" = "BID";
                 "price" = 1.5;
                 "expiry" = 1234569890
             },
             {
                 id = 12344;
                 "type" = "PriceAlert";
                 "symbol" = "USD/CAD";
                 "time" = 1234567890;
                 "price_type" = "ASK";
                 "price" = 1.0;
                 "expiry" = 1234569890
             }
         );
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 */
- (void)priceAlertsListForAccountId:(NSNumber *)accountId
                            success:(NetworkSuccessBlock)successBlock
                            failure:(NetworkFailBlock)failureBlock;

/** To retrieve the open positions for the given account.
 
 @param accountId **Required**.  Account Id to get open positions of (must be owned by the user).
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of NSDictionary, each describing an open position.
 @return Example of a returned NSDictionary:
     {
         positions = (
             {
                 avgPrice = "1.29428";
                 direction = long;
                 instrument = "EUR/USD";
                 units = 1;
             }
         );
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see closePositionForAccount:symbol:price:success:failure:
 */
- (void)positionsListForAccountId:(NSNumber *)accountId
                          success:(NetworkSuccessBlock)successBlock
                          failure:(NetworkFailBlock)failureBlock;

/** To retrieve the user's rate limits for the various requests.
 
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of NSDictionary, each describing a rate limit.
 @return Example of a returned NSDictionary:
     {
         "rate_limits" =
         (
             {
                 limit = 0;
                 remaining = 0;
                 type = IPRateLimiter;
             },
             {
                 limit = 100;
                 remaining = 0;
                 type = UsernameRateLimiter;
             }
         );
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 */
- (void)rateLimitsListSuccess:(NetworkSuccessBlock)successBlock
                      failure:(NetworkFailBlock)failureBlock;


#pragma mark Handling Positions
/** @name Handling Positions */

/** To close a position for the given account
 
 @param accountId **Required**. Account whose position should be closed (must be owned by the user).
 @param symbol **Required**.  Which position to close. All open trades on this symbol will be closed (eg. EUR/USD).
 @param price **Optional**.  Price the user/client would like to close the position at. Default: Current market rate
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of ids of closed positions, plus details of the overall close operation.
 @return Example of a returned NSDictionary:
     {
         ids =
         (
             12345,
             12346,
             12347
         );
             symbol = "EUR/USD";
             total_units = 1234;
             price = 1.2345;
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see positionsListForAccountId:success:failure:
 */
- (void)closePositionForAccount:(NSNumber *)accountId
                         symbol:(NSString *)symbol
                          price:(NSDecimalNumber *)price
                        success:(NetworkSuccessBlock)successBlock
                        failure:(NetworkFailBlock)failureBlock;


#pragma mark Creating and Managing LimitOrders
/** @name Creating and Managing LimitOrders */

/** To create a LimitOrder for the given account
 
 @param accountId **Required**. Account Id to create the order for (must be owned by the user).
 @param symbol **Required**.  Symbol to buy/sell when the order triggers. (eg. EUR/USD).
 @param units **Required**.  Number of units to buy/sell when the order triggers.
 @param type **Required**.  Should be either "long" (buy) or "short" (sell).
 @param price **Required**.  The price at which the order will trigger and create a trade.
 @param expiryInSeconds **Required**.  A period of time measured in seconds (from the moment this function is called), after which this order would be cancelled.
 @param lowPrice **Optional**.  Minimum execution price.
 @param highPrice **Optional**.  Maximum execution price.
 @param stopLoss **Optional**.  Stop Loss value.
 @param takeProfit **Optional**.  Take Profit value.
 @param trailingStop **Optional**.  Trailing Stop distance (in pipettes).
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain details describing the outcome of this operation.
 @return Example of a returned NSDictionary:
     {
         direction = long;
         id = 177809795;
         instrument = "EUR/GBP";
         ocaGroupId = 0;
         price = "0.80443";
         units = 123;
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see ordersListForAccountId:success:failure:
 */
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
                      failure:(NetworkFailBlock)failureBlock;

/** To modify an existing LimitOrder for the given account
 
 @param accountId **Required**. Account Id to create the order for (must be owned by the user).
 @param orderId **Required**. Id of order to modify (must belong to the account specified by accountId)
 @param symbol **Required**.  Symbol to buy/sell when the order triggers. (eg. EUR/USD).
 @param units **Required**.  Number of units to buy/sell when the order triggers.
 @param type **Required**.  Should be either "long" (buy) or "short" (sell).
 @param price **Required**.  The price at which the order will trigger and create a trade.
 @param expiryInSeconds **Required**.  A period of time measured in seconds (from the moment this function is called), after which this order would be cancelled.
 @param lowPrice **Optional**.  Minimum execution price.
 @param highPrice **Optional**.  Maximum execution price.
 @param stopLoss **Optional**.  Stop Loss value.
 @param takeProfit **Optional**.  Take Profit value.
 @param trailingStop **Optional**.  Trailing Stop distance (in pipettes).
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back a *nil* NSDictionary* as argument (limitation of the REST API being called underneath).
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see ordersListForAccountId:success:failure:
 @see deleteOrderForAccount:orderId:success:failure:
 */
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
                      failure:(NetworkFailBlock)failureBlock;

/** To poll for new, deleted, or changed orders.
 
 This is the mechanism to detect triggered orders. This also allows the account to be used by multiple client sessions simultaneously (eg. with Web GUI, mobile app, etc.).
 
 @param accountId **Required**. Account Id to poll changes for (must be owned by the user).
 @param maxOrderId **Required**.  This is the maximum order id the client was given in the previous pollOrder call (pass in 0 if calling for very first time). This also means the client should save and latest maxOrderId returned by the server for subsequent pollOrder requests.
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of  orders created.  As mentioned, a "maxOrderId" value is also returned for use in the next poll.  In essence, if the server's max_id is greater than the client's max_id, the server has changes the client is interested in.
 
 @return Example of a returned NSDictionary:
     {
         nextPage = "http://api-sandbox.oanda.com/accounts/506005/orders?maxOrderId=177809794";
         orders =     (
             {
                 direction = long;
                 expiry = 1354213265;
                 highLimit = 0;
                 id = 177809797;
                 instrument = "EUR/GBP";
                 lowLimit = 0;
                 ocaGroupId = 0;
                 price = "0.80443";
                 stopLoss = "0.78443";
                 takeProfit = "0.88443";
                 time = 1354212565;
                 trailingStop = 0;
                 units = 456;
             },
             {
                 direction = long;
                 expiry = 1354213187;
                 highLimit = 0;
                 id = 177809795;
                 instrument = "EUR/GBP";
                 lowLimit = 0;
                 ocaGroupId = 0;
                 price = "0.80443";
                 stopLoss = "0.78443";
                 takeProfit = "0.88443";
                 time = 1354212486;
                 trailingStop = 0;
                 units = 456;
             }
         );
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see ordersListForAccountId:success:failure:
 */
- (void)pollOrderForAccount:(NSNumber *)accountId
                 maxOrderId:(NSNumber *)maxOrderId
                    success:(NetworkSuccessBlock)successBlock
                    failure:(NetworkFailBlock)failureBlock;

/** To cancel an existing LimitOrder.
 
 @param accountId **Required**. Account Id to cancel the order for (must be owned by the user).
 @param orderId **Required**.  Id of the order to cancel
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain details regarding the order cancelled.
 
 @return Example of a returned NSDictionary:
     {
         direction = long;
         id = 177809797;
         instrument = "EUR/GBP";
         ocaGroupId = 0;
         price = "0.80443";
         units = 456;
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see createOrderForAccount:symbol:units:type:price:expiry:minExecutionPrice:maxExecutionPrice:stopLoss:takeProfit:trailingStop:success:failure:
 */
- (void)deleteOrderForAccount:(NSNumber *)accountId
                      orderId:(NSNumber *)orderId
                      success:(NetworkSuccessBlock)successBlock
                      failure:(NetworkFailBlock)failureBlock;


#pragma mark Creating and Managing MarketOrders Trades
/** @name Creating and Managing MarketOrders Trades */

/** To create a MarketOrder trade for the given account
 
 @param accountId **Required**. Account Id to execute the trade as (must be owned by the user).
 @param symbol **Required**.  Symbol to buy/sell when the order triggers. (eg. EUR/USD).
 @param units **Required**.  Number of units to buy/sell when the trade triggers.
 @param type **Optional**.  Should be either "long" (buy) or "short" (sell).  Default is "long"
 @param price **Optional**.  User price (informational, will be executed at server price).
 @param lowPrice **Optional**.  Minimum execution price.
 @param highPrice **Optional**.  Maximum execution price.
 @param stopLoss **Optional**.  Stop Loss value.
 @param takeProfit **Optional**.  Take Profit value.
 @param trailingStop **Optional**.  Trailing Stop distance (in pipettes).
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain details describing the outcome of this operation.
 @return Example of a returned NSDictionary:
     {
         direction = long;
         ids = (
             177809801
         );
         instrument = "AUD/JPY";
         marginUsed = "23.7685";
         price = "85.568";
         units = 456;
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see tradesListForAccountId:success:failure:
 @see closeTradeForAccount:tradeId:price:success:failure:
 */
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
                    failure:(NetworkFailBlock)failureBlock;

/** To modify an existing MarketOrder trade for the user
 
 @param accountId **Required**. Account Id to which the trade belongs (must be owned by the user).
 @param tradeId **Required**. Id of trade to modify (must belong to the account specified by accountId)
 @param stopLoss **Optional**.  Stop Loss value.
 @param takeProfit **Optional**.  Take Profit value.
 @param trailingStop **Optional**.  Trailing Stop distance (in pipettes).
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back a *nil* NSDictionary* as argument (limitation of the REST API being called underneath).
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see tradesListForAccountId:success:failure:
 @see closeTradeForAccount:tradeId:price:success:failure:
 */
- (void)changeTradeForAccount:(NSNumber *)accountId
                      tradeId:(NSNumber *)tradeId
                     stopLoss:(NSDecimalNumber *)stopLoss
                   takeProfit:(NSDecimalNumber *)takeProfit
                 trailingStop:(NSDecimalNumber *)trailingStop
                      success:(NetworkSuccessBlock)successBlock
                      failure:(NetworkFailBlock)failureBlock;

/** To poll for new, deleted, or changed trades.
 
 This is the mechanism to detect triggered stop loss, take profit, and trailing stop orders. This also allows the account to be used by multiple client sessions simultaneously (eg. with Web GUI, mobile app, etc.).
 
 @param accountId **Required**. Account Id to poll changes for (must be owned by the user).
 @param maxTradeId **Required**.  This is the maximum trade id the client was given in the previous pollTrade call (pass in 0 if calling for very first time). This also means the client should save and latest maxTradeId returned by the server for subsequent pollTrade requests.
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of  open trades.  As mentioned, a "maxTradeId" value is also returned for use in the next poll.  In essence, if the server's max_id is greater than the client's max_id, the server has changes the client is interested in.
 
 @return Example of a returned NSDictionary:
     {
         nextPage = "http://api-sandbox.oanda.com/accounts/506005/trades?maxTradeId=177809414";
         trades =     (
             {
                 direction = long;
                 id = 177809801;
                 instrument = "AUD/JPY";
                 price = "85.568";
                 stopLoss = 0;
                 takeProfit = 2000;
                 time = 1354212801;
                 trailingStop = 0;
                 units = 456;
             },
             {
                 direction = long;
                 id = 177809415;
                 instrument = "EUR/USD";
                 price = "1.29428";
                 stopLoss = 0;
                 takeProfit = 0;
                 time = 1354025935;
                 trailingStop = 0;
                 units = 1;
             }
         );
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see tradesListForAccountId:success:failure:
 @see closeTradeForAccount:tradeId:price:success:failure:
 */
- (void)pollTradeForAccount:(NSNumber *)accountId
                 maxTradeId:(NSNumber *)maxTradeId
                    success:(NetworkSuccessBlock)successBlock
                    failure:(NetworkFailBlock)failureBlock;

/** To close an existing trade for the user account.
 
 @param accountId **Required**. Account Id to close the trade for (must be owned by the user).
 @param tradeId **Required**.  Id of the trade to close
 @param price **Optional**.  Price the user would like to close the trade at. Default: Current market rate.
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain details regarding the trade closed.
 
 @return Example of a returned NSDictionary:
     {
         direction = long;
         id = 177809825;
         instrument = "AUD/JPY";
         price = "85.58499999999999";
         profit = "0.0944";
     }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see openTradeForAccount:symbol:units:type:price:minExecutionPrice:maxExecutionPrice:stopLoss:takeProfit:trailingStop:success:failure:
 */
- (void)closeTradeForAccount:(NSNumber *)accountId                  //required
                     tradeId:(NSNumber *)tradeId                    //required
                       price:(NSDecimalNumber *)price               //optional
                     success:(NetworkSuccessBlock)successBlock
                     failure:(NetworkFailBlock)failureBlock;

@end
