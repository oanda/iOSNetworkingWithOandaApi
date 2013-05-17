//
//  OTTableViewController.m
//  OTNetworkLayer
//
//  Created by Johnny Li, Adam Chan on 12-11-13.
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

#import "OTTableViewController.h"
#import "OTAppDelegate.h"
#import "OTNetwork/OTNetworkLayer/OTNetworkController.h"


@interface OTTableViewController () {
    BOOL allowRatesFetching;
}

@property (weak, nonatomic) OTNetworkController *networkDelegate;
@property (strong, nonatomic) NSArray *listSymbols;         // detailed list of symbols (for table cells)
@property (strong, nonatomic) NSMutableArray *symbolsArray; // simplified list of symbols (for network quoting)
@property (strong, nonatomic) NSArray *listRates;           // list of prices, updated periodically
@property (strong, nonatomic) NSTimer *syncTimer;

@end

@implementation OTTableViewController

@synthesize listSymbols = _listSymbols;
@synthesize symbolsArray = _symbolsArray;
@synthesize listRates = _listRates;
@synthesize syncTimer = _syncTimer;

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.title = @"REST API";
    
    self.networkDelegate = [((OTAppDelegate *)[[UIApplication sharedApplication] delegate]) networkController];
    
    // TODO: add Login once it's supported by the OANDA API.  For now just dive in and use it directly
    [self doGetRateList];

    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
 
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

- (void)viewDidAppear:(BOOL)animated {
    self.syncTimer = [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(doGetRatePrices) userInfo:nil repeats:YES];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return self.listRates.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    
    if(!cell) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellIdentifier];
    }
    
    cell.textLabel.text = [[self.listSymbols objectAtIndex:indexPath.row] valueForKey:@"displayName"];
    cell.detailTextLabel.text = [NSString stringWithFormat:@"Buy: %@  Sell: %@", [[self.listRates objectAtIndex:indexPath.row] valueForKey:@"bid"], [[self.listRates objectAtIndex:indexPath.row] valueForKey:@"ask"]];
    cell.detailTextLabel.textColor = [UIColor redColor];
    
    return cell;
}

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath
{
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Navigation logic may go here. Create and push another view controller.
    /*
     <#DetailViewController#> *detailViewController = [[<#DetailViewController#> alloc] initWithNibName:@"<#Nib name#>" bundle:nil];
     // ...
     // Pass the selected object to the new view controller.
     [self.navigationController pushViewController:detailViewController animated:YES];
     */
}

-(void) doGetRateList
{
    // Test get All Symbols List
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
         NSLog(@"soGetRateList Failure");
     }];
}

-(void) doGetRatePrices
{
    static int callCount = 0;
    if(allowRatesFetching) {
        
        // TODO: delete this small test when ready
        // A quick little hack to trigger a chain of network calls to test
        // the OANDA API.  The main purpose of this app is still just to show
        // rates for tradable instruments, refreshed periodically.  Please feel
        // free to delete the IF case
        static int numReps = 2;
        if (numReps == 0)
        {
            // Start running internal tests for network calls
            //[self doAccountList];
            [self doAccountStatus:@3973087];
        }
        else
        {
            // Test get Actual Rates
            
            // Invoke the actual network call
            [self.networkDelegate rateQuote:self.symbolsArray
                             success:^(NSDictionary *responseObject)
             {
                 NSLog(@"Success!  %d", callCount++);
                 self.listRates = [responseObject objectForKey:@"prices"];
                 [self.tableView reloadData];
                 //NSLog(@"Rates: %@", responseObject);
                 
             } failure:^(NSDictionary *error) {
                 NSLog(@"doGetRatePrices Failure");
             }];
        }
        
        // TODO: delete this from the hack as well
        --numReps;
    }
}

// TODO: delete this small test when ready
/////////////////////////////////////////////////////////////////////
//
// The following are made just to illustrate the different network
// calls, and have nothing to do with the table being displayed in
// this sample app.  Please feel free to delete all code below.
//
/////////////////////////////////////////////////////////////////////
static NSNumber *gAccountId;
static NSNumber *gOrderId;
static NSNumber *gMaxOrderIdForOrderPoll;
static NSNumber *gTradeId;
static NSNumber *gMaxTradeIdForTradePoll;

-(void) doAccountList
{
    // Test getting the list of accounts for this user
    [self.networkDelegate accountListForUsername:@"kyley"
                                         success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  %@", responseObject);
         NSDictionary *anAccount = [[responseObject objectForKey:@"array"] lastObject];
         gAccountId = [anAccount valueForKey:@"id"];
         [self doAccountStatus:gAccountId];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"doAccountList Failure");
     }];
}

-(void) doAccountStatus:(NSNumber *)accountId
{
    // Test getting the state of the current account
    [self.networkDelegate accountStatusForAccountId:accountId
                                     success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  %@", responseObject);
         gAccountId = accountId;
         [self doTransactionsList:gAccountId];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"doAccountStatus Failure");
     }];
}

-(void) doTransactionsList:(NSNumber *)accountId
{
    // Test getting the list of transactions
    [self.networkDelegate transactionListForAccountId:accountId
                                       success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  %@", responseObject);
         [self doPriceAlertsList:gAccountId];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"doTransactionsList Failure");
     }];
}

-(void) doPriceAlertsList:(NSNumber *)accountId
{
    
    // Test getting the list of price alerts
//    [self.networkDelegate priceAlertsListForAccountId:accountId
//                                       success:^(NSDictionary *responseObject)
//     {
//         NSLog(@"Success!  %@", responseObject);
//         [self doPositionsList:gAccountId];
//         
//     } failure:^(NSDictionary *error) {
//         NSLog(@"Failure");
//     }];
    [self doPositionsList:gAccountId];
}

-(void) doPositionsList:(NSNumber *)accountId
{
    // Test getting the list of positions
    [self.networkDelegate positionsListForAccountId:accountId
                                     success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  %@", responseObject);
         [self doCandlesList];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"doPositionsList Failure");
     }];
}

-(void) doCandlesList
{
    // Test getting a list of candles
    [self.networkDelegate rateCandlesForSymbol:@"EUR_USD"
                                   granularity:@"S30"
                                numberOfPoints:[NSNumber numberWithInt:5]
                                       success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  %@", responseObject);
         [self doCreateOrder:gAccountId];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"doCandlesList Failure");
     }];
}

-(void) doCreateOrder:(NSNumber *)accountId
{
    // Test creating a new purchase order
    
    [self.networkDelegate createOrderForAccount:accountId
                                  symbol:@"EUR_GBP"
                                   units:[NSNumber numberWithInt:123]
                                    type:@"buy"
                                   price:[[NSDecimalNumber alloc] initWithFloat:0.80443]
                                  expiry:[NSNumber numberWithInt:600]  // set to expire in 10 minutes
                       minExecutionPrice:nil
                       maxExecutionPrice:nil
                                stopLoss:nil
                              takeProfit:nil
                            trailingStop:nil
                                 success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  Order Created: %@", responseObject);
         gOrderId = [responseObject valueForKey:@"id"];
         [self doOrdersList:gAccountId];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"doCreateOrder Failure");
     }];
}

-(void) doOrdersList:(NSNumber *)accountId
{
    // Test getting the list of existing orders
    [self.networkDelegate ordersListForAccountId:accountId
                                         success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  %@", responseObject);
         [self doChangeOrder:gAccountId withOrderId:gOrderId];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"doOrdersList Failure");
     }];
}

-(void) doChangeOrder:(NSNumber *)accountId
          withOrderId:(NSNumber *)orderId
{
    // Test changing an existing order, turning it into a "sell" order
    
    [self.networkDelegate changeOrderForAccount:accountId
                                        orderId:orderId
                                         symbol:@"EUR/GBP"                      // TODO: investigate, no visible effect
                                          units:[NSNumber numberWithInt:456]
                                           type:@"buy"                         // TODO: investigate, no visible effect
                                          price:[[NSDecimalNumber alloc] initWithFloat:0.80443]
                                         expiry:[NSNumber numberWithInt:700]  // set to expire in 10 minutes
                              minExecutionPrice:[[NSDecimalNumber alloc] initWithFloat:0.80343]     // TODO: investigate, no visible effect
                              maxExecutionPrice:[[NSDecimalNumber alloc] initWithFloat:0.80543]     // TODO: investigate, no visible effect
                                       stopLoss:[[NSDecimalNumber alloc] initWithFloat:0.78443]
                                     takeProfit:[[NSDecimalNumber alloc] initWithFloat:0.88443]
                                   trailingStop:nil
                                        success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  Order Changed");
         gMaxOrderIdForOrderPoll = [NSNumber numberWithInt:0];
         [self doPollOrder:gAccountId withMaxOrderId:gMaxOrderIdForOrderPoll];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"doChangeOrder Failure");
     }];
}

-(void) doPollOrder:(NSNumber *)accountId
     withMaxOrderId:(NSNumber *)maxOrderId
{
    // Test polling orders
    [self.networkDelegate pollOrderForAccount:accountId
                            maxOrderId:maxOrderId
                               success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  Order Polled: %@", responseObject);
         gMaxOrderIdForOrderPoll = [responseObject valueForKey:@"max_order_id"];
         [self doDeleteOrder:gAccountId withOrderId:gOrderId];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"Failure");
     }];
}

-(void) doDeleteOrder:(NSNumber *)accountId
          withOrderId:(NSNumber *)orderId
{
    // Test deleting an order
    [self.networkDelegate deleteOrderForAccount:accountId
                                 orderId:orderId
                                 success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  Order Deleted: %@", responseObject);
         [self doOpenTrade:gAccountId];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"Failure");
     }];
}

-(void) doOpenTrade:(NSNumber *)accountId
{
    // Test creating a new trade order
    
    [self.networkDelegate openTradeForAccount:accountId
                                symbol:@"AUD/JPY"
                                 units:[NSNumber numberWithInt:456]
                                  type:@"buy"
                                 price:nil
                     minExecutionPrice:nil
                     maxExecutionPrice:nil
                              stopLoss:nil
                            takeProfit:nil
                          trailingStop:nil
                               success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  Trade Opened: %@", responseObject);
         gTradeId = [[responseObject valueForKey:@"ids"] objectAtIndex:0];
         [self doTradesList:gAccountId];         
         
     } failure:^(NSDictionary *error) {
         NSLog(@"Failure");
     }];
}


-(void) doTradesList:(NSNumber *)accountId
{
    // Test getting the list of existing trades
    [self.networkDelegate tradesListForAccountId:accountId
                                         success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  %@", responseObject);
         [self doChangeTrade:gAccountId withTradeId:gTradeId];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"Failure");
     }];
}

-(void) doChangeTrade:(NSNumber *)accountId
          withTradeId:(NSNumber *)tradeId
{
    // Test changing an existing trade, setting the "take profit"
    
    [self.networkDelegate changeTradeForAccount:accountId
                                 tradeId:tradeId
                                stopLoss:nil
                              takeProfit:[[NSDecimalNumber alloc] initWithFloat:2000.0]
                            trailingStop:nil
                                 success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  Trade Changed");
         gMaxTradeIdForTradePoll = [NSNumber numberWithInt:0];
         [self doPollTrade:gAccountId withMaxTradeId:gMaxTradeIdForTradePoll];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"Failure");
     }];
}

-(void) doPollTrade:(NSNumber *)accountId
     withMaxTradeId:(NSNumber *)maxTradeId
{
    // Test polling trades
    [self.networkDelegate pollTradeForAccount:accountId
                            maxTradeId:maxTradeId
                               success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  Trades Polled: %@", responseObject);
         gMaxTradeIdForTradePoll = [responseObject valueForKey:@"max_trade_id"];
         [self doDeleteTrade:gAccountId withTradeId:gTradeId];
         
     } failure:^(NSDictionary *error) {
         NSLog(@"Failure");
     }];
}

-(void) doDeleteTrade:(NSNumber *)accountId
          withTradeId:(NSNumber *)tradeId
{
    // Test deleting an order
    [self.networkDelegate closeTradeForAccount:accountId
                                tradeId:tradeId
                                  price:nil
                                success:^(NSDictionary *responseObject)
     {
         NSLog(@"Success!  Trade Closed: %@", responseObject);
         
     } failure:^(NSDictionary *error) {
         NSLog(@"Failure");
     }];
}

@end
