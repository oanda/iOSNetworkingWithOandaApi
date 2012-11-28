//
//  OTNetworkController.h
//  OTNetworkLayer
//
//  Created by Johnny Li on 12-11-12.
//  Copyright (c) 2012 Johnny Li. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AFHTTPClient.h"

#define REST_API_VERSION @"v1"
#define kSessionToken @"session_token"
#define kUserName @"username"

typedef void (^NetworkSuccessBlock)(NSDictionary *result);
typedef void (^NetworkFailBlock)(NSError *error);

/** This class is a wrapper for low level REST API network calls, and is meant to provide a consistent means for higher networking layers to send and receive data.
  
 As seen from all the methods, the intention is for the higher level caller to pass in
 
 - all required and optional parameters, as needed for the particular network request
 - a SuccessBlock
 - a FailBlock
 
 If the network replies our request by delivering the data we seek, the data is converted to NSDictionary, and
 the SuccessBlock will be triggered with this NSDictionary passed in as argument.  Similarly, a failed network
 request will trigger the FailBlock with an error code.
 
 Also please be aware of these typedef for the blocks mentioned:
 
 typedef void (^NetworkSuccessBlock)(NSDictionary *result);
 typedef void (^NetworkFailBlock)(NSError *error);
 
 For additional information on Objective-C blocks, please refer to
 http://developer.apple.com/library/ios/documentation/Cocoa/Conceptual/Blocks/
 
 Although it is not a singleton, we strongly discourage having multiple instances operating simultaneously, because their competition to access the same network would result in us not being able to guarantee an acceptable Quality of Service.
*/
@interface OTNetworkController : NSObject

#pragma mark User
/** @name Core */

/** To retrieve a list of accounts belonging to the user.

 @param username **Required**.  Name of the user to obtain the accounts from.  NOTE: this will likely be removed once proper login/authentication is supported.
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing
 back an NSDictionary* as argument.  The NSDictionary should contain an array of NSDictionary, each describing an account belonging to the user.
 @return Example of a returned NSDictionary:
 {
 "account_list" =
 (
 {
 "account_property_name" = ();
 homecurr = USD;
 id = 928766;
 "margin_rate" = "0.05";
 name = mamanager1;
 },
 {
 "account_property_name" = ();
 homecurr = USD;
 id = 701048;
 "margin_rate" = "0.0333";
 name = "testusr2_5";
 }
 );
 "division_id" = 2;
 "markup_group_id" = 102;
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
 "account_id" = 701048;
 "account_name" = "testusr2_5";
 balance = "99808.30959999999";
 homecurr = USD;
 "margin_avail" = "86110.2283";
 "margin_rate" = "0.0333";
 "margin_used" = "13858.1447";
 "markup_group_id" = 102;
 nav = "99968.37300000001";
 "open_orders" = 0;
 "open_trades" = 7;
 "realized_pl" = "-102.8888";
 "unrealized_pl" = "160.0634";
 }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see accountListSuccess:failure:
 */
- (void)accountStatusForAccountId:(NSNumber*)accountId
                          success:(NetworkSuccessBlock)successBlock
                          failure:(NetworkFailBlock)failureBlock;


#pragma mark Rate
/** @name Rate */

/** To retrieve a list of tradable symbol pairs.
 
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of symbols the client can trade on and where the pip location and pippettes are for that pair.
 @return Example of a returned NSDictionary:
 {
 symbols =     (
 {
 "max_trade_units" = 10000000;
 piploc = "0.0001";
 precision = 5;
 symbol = "AUD/CAD";
 },
 {
 "max_trade_units" = 10000000;
 piploc = "0.0001";
 precision = 5;
 symbol = "AUD/CHF";
 },
 {
 "max_trade_units" = 10000000;
 piploc = "0.0001";
 precision = 5;
 symbol = "AUD/HKD";
 },
 {
 "max_trade_units" = 10000000;
 piploc = "0.01";
 precision = 3;
 symbol = "AUD/JPY";
 },
 {
 "max_trade_units" = 10000000;
 piploc = "0.0001";
 precision = 5;
 symbol = "AUD/NZD";
 },
 {
 "max_trade_units" = 10000000;
 piploc = "0.0001";
 precision = 5;
 symbol = "USD/HKD";
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
 prices =
 (
 {
 ask = "1.03601";
 bid = "1.03596";
 "new_ladder" = 1;
 symbol = "AUD/CAD";
 time = 1353610975;
 },
 {
 ask = "0.97243";
 bid = "0.97205";
 "new_ladder" = 1;
 symbol = "AUD/CHF";
 time = 1353611194;
 },
 {
 ask = "1.28697";
 bid = "1.28626";
 "new_ladder" = 1;
 symbol = "EUR/USD";
 time = 1353611236;
 }
 );
 }
 
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see rateListSymbolsSuccess:failure:
 */
- (void)rateQuote:(NSArray *)symbolPairList
          success:(NetworkSuccessBlock)successBlock
          failure:(NetworkFailBlock)failureBlock;

/** To retrieve the historical pricing for a symbol.
 
 @param symbol **Required**.  Which symbol to retrieve prices for (eg. EUR/USD)
 @param granularity **Required**.  Specifies the pricing interval to use. (eg. 2)
 @param points **Required**.  Specifies the maximum number of price points to return. (eg. 100)
 @param markupGroupId **Optional**.  Specifies the markup group for the user
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of past prices for the given symbol pair (ie. candles).
 @return Example of a returned NSDictionary:
 
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see rateQuote:success:failure:
 */
- (void)rateHistoryForSymbol:(NSString *)symbol
                 granularity:(NSNumber *)granularity
              numberOfPoints:(NSNumber *)points
               markupGroupId:(NSNumber *)markupGroupId
                     success:(NetworkSuccessBlock)successBlock
                     failure:(NetworkFailBlock)failureBlock;


#pragma mark Reports
/** @name Reports */

/** To retrieve the recent transactions for the given account.
 
 @param accountId **Required**.  Account Id to get transactions of (must be owned by the user).
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain a list of NSDictionary, each describing a past transaction.
 @return Example of a returned NSDictionary:
 {
 transactions =     (
 {
 "account_id" = 701048;
 amount = "128.2351";
 balance = "99808.30959999999";
 "completion_code" = 100;
 diaspora = 177809312;
 duration = 20;
 "high_order_limit" = 0;
 id = 177809312;
 interest = 0;
 "low_order_limit" = 0;
 "margin_used" = "4.272";
 "order_link" = 0;
 price = "1.2368";
 "profit_loss" = "-0.0415";
 "stop_loss" = 0;
 symbol = "EUR/AUD";
 "take_profit" = 0;
 time = 1353535764;
 "trailing_stop" = 0;
 "transaction_link" = 177809311;
 type = SellMarket;
 units = 100;
 },
 {
 "account_id" = 701048;
 amount = "128.3063";
 balance = "99808.3511";
 "completion_code" = 100;
 diaspora = 0;
 duration = 0;
 "high_order_limit" = 0;
 id = 177809311;
 interest = 0;
 "low_order_limit" = 0;
 "margin_used" = "4.272";
 "order_link" = 0;
 price = "1.2372";
 "profit_loss" = 0;
 "stop_loss" = 0;
 symbol = "EUR/AUD";
 "take_profit" = 0;
 time = 1353535744;
 "trailing_stop" = 0;
 "transaction_link" = 0;
 type = BuyMarket;
 units = 100;
 },
 {
 "account_id" = 701048;
 amount = "1.2829";
 balance = "99808.3511";
 "completion_code" = 100;
 diaspora = 177809310;
 duration = 1384238;
 "high_order_limit" = 0;
 id = 177809310;
 interest = 0;
 "low_order_limit" = 0;
 "margin_used" = "0.0426";
 "order_link" = 0;
 price = "1.28286";
 "profit_loss" = "0.0037";
 "stop_loss" = 0;
 symbol = "EUR/USD";
 "take_profit" = 0;
 time = 1353535228;
 "trailing_stop" = 0;
 "transaction_link" = 177805677;
 type = SellMarket;
 units = 1;
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
 "open_trades" =
 (
 {
 id = 1913099015;
 units = 10;
 dir = "L";
 symbol = "EUR/USD";
 time = 1353616691;
 price = 1.2878;
 "stop_loss" = 0;
 "take_profit" = 0;
 "trailing_stop" = 0;
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
 "open_pricealerts" =
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
 @see createPriceAlertForAccount:symbol:priceType:price:expiry:success:failure:
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
 "open_positions" =
 (
 {
 dir = "l";
 symbol = "EUR/USD";
 units = 1000;
 "avg_price" = 25.23;
 },
 {
 dir = "s";
 symbol = "USD/CAD";
 units = 10000;
 "avg_price" = 325.56;
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


#pragma mark Positions
/** @name Positions */

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


#pragma mark Orders
/** @name Orders */

/** To create a LimitOrder for the given account
 
 @param accountId **Required**. Account Id to create the order for (must be owned by the user).
 @param symbol **Required**.  Symbol to buy/sell when the order triggers. (eg. EUR/USD).
 @param units **Required**.  Number of units to buy/sell when the order triggers.
 @param type **Required**.  Should be either "buy" or "sell".
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
 dir = L;
 id = 1913101947;
 "oca_group_id" = 0;
 price = "0.80443";
 symbol = "EUR/GBP";
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
 @param type **Required**.  Should be either "buy" or "sell".
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
 @param maxOrderId **Required**.  This is the maximum order id the client was given in the previous pollOrder call (pass in 0 if calling for very first time). This also means the client should save and latest max_order_id returned by the server for subsequent pollOrder requests.
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain multiple lists, one for each of the created, deleted and updated orders.  As mentioned, a "max_order_id" value is also returned for use in the next poll.  In essence, if the server's max_id is greater than the client's max_id, the server has changes the client is interested in.
 
 @return Example of a returned NSDictionary:
 {
 created =
 (
 1913101947
 );
 deleted =
 (
 1910348044
 );
 updated =
 (
 1910348044,
 1913101947
 );
 "max_order_id" = 1913103961;
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
 dir = L;
 id = 1913319751;
 "oca_group_id" = 0;
 price = "0.80443";
 symbol = "EUR/GBP";
 units = 123;
 }
 @return Similarly, any problem with the network would trigger the failureBlock, passing back an NSError.
 @see createOrderForAccount:symbol:units:type:price:expiry:minExecutionPrice:maxExecutionPrice:stopLoss:takeProfit:trailingStop:success:failure:
 */
- (void)deleteOrderForAccount:(NSNumber *)accountId
                      orderId:(NSNumber *)orderId
                      success:(NetworkSuccessBlock)successBlock
                      failure:(NetworkFailBlock)failureBlock;


#pragma mark Trades
/** @name Trades */

/** To create a MarketOrder trade for the given account
 
 @param accountId **Required**. Account Id to execute the trade as (must be owned by the user).
 @param symbol **Required**.  Symbol to buy/sell when the order triggers. (eg. EUR/USD).
 @param units **Required**.  Number of units to buy/sell when the trade triggers.
 @param type **Optional**.  Should be either "buy" or "sell".  Default is "buy"
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
 dir = L;
 ids =
 (
 177809322
 );
 "margin_used" = "28.5113";
 price = "85.875";
 symbol = "AUD/JPY";
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
 @param maxTradeId **Required**.  This is the maximum trade id the client was given in the previous pollTrade call (pass in 0 if calling for very first time). This also means the client should save and latest max_trade_id returned by the server for subsequent pollTrade requests.
 @param successBlock **Required**.  An Objective-C block passed in, to be triggered upon a successful network call.  The block has an
 argument of type **NSDictionary***.
 @param failureBlock **Required**.  An Objective-C block passed in, to be triggered upon a failed network call.  The block has an
 argument of type **NSError***.
 @return The function itself returns nothing.  A successful operation would trigger instead the successBlock, passing back an NSDictionary* as argument.  The NSDictionary should contain multiple lists, one for each of the opened, closed and updated orders.  As mentioned, a "max_trade_id" value is also returned for use in the next poll.  In essence, if the server's max_id is greater than the client's max_id, the server has changes the client is interested in.
 
 @return Example of a returned NSDictionary:
 {
 closed =
 (
 177809311,
 177809322
 );
 opened =
 (
 177809300,
 177809304,
 177809333
 );
 updated =
 (
 177809322,
 177809333
 );
 "max_trade_id" = 177809334;
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
 dir = L;
 id = 177809335;
 price = "85.92100000000001";
 profit = "0.0553";
 symbol = "AUD/JPY";
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
