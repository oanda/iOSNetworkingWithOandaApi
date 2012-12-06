Pod::Spec.new do |s|
  s.name = "iOSNetworkingWithOandaApi"
  s.version = "0.0.1"
  s.summary = "A wrapper of the OANDA API, intended to handle all the low level networking requests to trade FOREX."
  s.homepage = "https://github.com/oanda/iOSNetworkingWithOandaApi"
  s.license = { :type => 'MIT', :file => 'LICENSE' }
  s.author = { "OANDA Support - Mobile" => "helpme-mobile@oanda.com" }
  s.source = { :git => "https://github.com/oanda/iOSNetworkingWithOandaApi.git", :commit => "727ff3c20448883e5c247b8592d992231acb5aed" }
  s.platform     = :ios, '5.0'
  s.source_files = 'OTNetwork/OTNetworkLayer/*.{h,m}'
  s.requires_arc = true
  s.dependency 'JSONKit'
  s.dependency 'AFNetworking'
  s.ios.frameworks = 'MobileCoreServices', 'SystemConfiguration'
end