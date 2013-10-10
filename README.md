![](https://raw.github.com/oanda/apidocs/master/images/oanda_header.png)
=========

**Disclaimer**: The OANDA API is currently available for use in our developer sandbox, where you are free to develop and test your apps.  To use the API with production accounts, please email us at api@oanda.com.

<table>
	<tr>
		<td>
            <b>OTNetwork</b> is essentially a wrapper of the OANDA API, intended to handle all the low level requests with <b>OANDA</b> to trade in the FOREX market.
			<br/><br/>
			This simple guide describes how to incorporate it in your iOS app, so you could start trading FOREX.
		</td>
		<td style="background-color:#e4e4e4"><img src="https://raw.github.com/oanda/apidocs/master/images/box.png" /></td>
	</tr>
</table>


Steps for Integration
---------------------

These are the simple steps to follow to incorporate this library into your iOS project:

1. Add the library itself.  There are two ways to do this:
    * Manually include the library folder <b>/OTNetwork/OTNetworkLayer</b> into your project.  This effectively includes <b>OTNetworkController.h</b> and <b>OTNertowkController.m</b>

        Please note you will need these third party libraries as well:

        <ul><li><b>AFNetworking</b></li>
        <li><b>JSONKit</b></li>
        </ul>
<br/>        
    * Or you could install the library via CocoaPods (assuming you have been following this workflow):
        * copy the included <b>iOSNetworkingWithOandaApi.podspec</b> to the location of your project's <b>Podfile</b>
        * edit your <b>Podfile</b>, and include this line:
        
                pod 'iOSNetworkingWithOandaApi', :podspec => 'iOSNetworkingWithOandaApi.podspec'
        
        * run <b>pod install</b> to update your .xcworkspace.  You should now find <b>iOSNetworkingWithOandaApi</b> as one of the pods installed.
<br/><br/>
2. Add these frameworks to your app:
    * <b>MobileCoreServices.framework</b>
    * <b>SystemConfiguration.framework</b>
<br/><br/>
3. Import <b>OTNetworkController.h</b> in relevant source files in your project
<br/><br/>
4. Create an instance of <b>OTNetworkController</b>.  Your app should continue using it to handle all network requests to OANDA services (it's probably a good idea to store a pointer to it in your app's common shell).
<br/><br/>

That's it.  Provided you have reliable network support (eg. Wifi, Ethernet, cell, etc.), you should be good to go.

NOTE: The library itself was developed using <b>Kiwi</b> as the testing framework, but it is not a mandatory requirement for your app.

For reference, please open the included <b>OTNetworkOandaApi.xcworkspace</b>.  It includes the Kiwi framework, the OTNetwork library, and the simple demo <b>OTNetworkDemo</b> that illustrates how one could make asynchronous calls to the <b>OANDA</b> trading services via this library.


Notes on the OTNetwork Library
------------------------------

* User login & authentication is currently disabled, so simply passing in an approved username for now (please speak with your <b>OANDA</b> partner for details).  Please see <b>accountListForUsername:success:failure:</b> for further info.
<br/><br/>
* It support the most commonly used feature set like polling for rates, making and closing orders & trades, generating reports, etc.  Other features (eg. price alerts, news, etc.) are being worked on and not ready for this release.
<br/><br/>
* We discourage creating more than once instance of the <b>OTNetworkController</b>, since having multiple ones simultaneously competing for network access from your app may result in poor and/or unpredictable performance.
<br/><br/>
* Methods are asynchrounous, so do not expect to obtain results back immediately, or wait for them.
<br/><br/>
* All methods require a <b>Success</b> block and a <b>Failure</b> block to be passed, and either would be triggered asynchronously depending on the outcome of network request.  And these blocks all follow the same convention:

            typedef void (^NetworkSuccessBlock)(NSDictionary *result);
            typedef void (^NetworkFailBlock)(NSDictionary *error);

* If an error occurs, the FailBlock above would return an NSDictionary with the following items to help you debug:

            "code" : OANDA error code, may or may not be the same as the HTTP status code
            "http status code" : response of the HTTP request to the network
            "message" : a description of the error which occurred, intended for developers
            "net error" : full error string received, intended for developers

Notes on the OTNetworkDemo App
------------------------------
* It is a simple app created mainly to illustrate how to fetch updated rates, for all tradable symbol pairs, from the OANDA services.  The most relevant sample code could be found in <b>OTTableViewController.m</b>
<br/><br/>
* <b>Kiwi</b> is not needed for OTNetworkDemo.


Example 1: Getting Rates
-------------------------

Getting updated rates (ie. prices) for tradable currency pairs from <b>OANDA</b> is one of the most common operations.  The following is a simple walkthrough for doing this using the <b>OTNetwork</b> library (for more info, please refer to the included <b>OTNetworkDemo</b> app):

* Call <b>rateListSymbolsSuccess:failure:</b> to obtain a list of tradable currency pairs.  In the <b>Success</b> block, extract the actual <b>instruments</b> list from the returned NSDictionary.  Further extract a list of just symbol pairs, to be used later to quote for rates.  You may want to save the original <b>instruments</b> list as well since it has more detailed info on each tradable pair (like proper display name).

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
             }];

* Call <b>rateQuote:success:failure:</b> to get actual quotes for a list of symbol pairs.  In the <b>Success</b> block, extract the <b>prices</b> list from the returned NSDictionary.

            [self.networkDelegate rateQuote:self.symbolsArray
                             success:^(NSDictionary *responseObject)
             {
                 NSLog(@"Success!  %d", callCount++);
                 self.listRates = [responseObject objectForKey:@"prices"];
                 [self.tableView reloadData];
                 //NSLog(@"Rates: %@", responseObject);
                 
             } failure:^(NSDictionary *error) {
                 NSLog(@"Failure");
             }];

* You now have a list of symbols and a list of prices ready for display.


Example 2: Making a "BUY" (ie. long) Order
-------------------------------------------

You need a valid username with an active account to do this.  In fact, at this point almost all requests to the <b>OANDA</b> services would need an account number:

* <b>(If you only have a username but no account number, follow this step, otherwise skip ahead)</b> Call <b>accountListForUsername:success:failure:</b> to get a list of all active accounts belonging to your username.  Then extract and save the account number of your choice from the list.

            [self.networkDelegate accountListForUsername:@"kyley"
                                                 success:^(NSDictionary *responseObject)
             {
                 NSLog(@"Success!  %@", responseObject);
                 
                 // say this is the account I am interested in
                 NSDictionary *anAccount = [[responseObject objectForKey:@"array"] lastObject];
                 gAccountId = [anAccount valueForKey:@"id"];
                                  
             } failure:^(NSDictionary *error) {
                 NSLog(@"Failure");
             }];

* Using the obtained account ID, <b>call createOrderForAccount:...success:failure:</b> to make a order request.  A successful network request would return an NSDictionary, and you could extract the order's <b>id</b> key value for later use (eg. quote the status of the order, make changes, cancel the order, etc.)

            [self.networkDelegate createOrderForAccount:gAccountId
                                          symbol:@"EUR/GBP"
                                           units:[NSNumber numberWithInt:123]
                                           side:@"buy"
                                            type:@"long"  //a buy request
                                           price:[[NSDecimalNumber alloc] initWithFloat:0.80443]
                                          expiry:[NSNumber numberWithInt:600]  // set to expire in 10 minutes
                               minExecutionPrice:nil  //optional
                               maxExecutionPrice:nil  //optional
                                        stopLoss:nil  //optional
                                      takeProfit:nil  //optional
                                    trailingStop:nil  //optional
                                         success:^(NSDictionary *responseObject)
             {
                 NSLog(@"Success!  Order Created: %@", responseObject);
                 gOrderId = [responseObject valueForKey:@"id"];
                 // do something with gOrderId somewhere else
                                  
             } failure:^(NSDictionary *error) {
                 NSLog(@"Failure");
             }];


Further Details
---------------

For more information on the methods provided, please refer to the documentation in <b>OTNetworkController.h</b>

For in-depth info on the OANDA API used under the hood, please check out <a href="https://github.com/oanda/apidocs/blob/master/README.md">The OANDA API</a>

