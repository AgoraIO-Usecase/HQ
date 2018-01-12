//
//  DynamicKey5.m
//  AgoraDynamicKeyBasic
//
//  Created by ZhangJi on 01/08/2017.
//  Copyright © 2017 Agora. All rights reserved.
//
#import "DynamicKey.h"
#import "DynamicKey5.h"

@implementation DynamicKey
+ (NSString *) generateMediaChannelKeyby:(const NSString *)appId
                             Certificate:(const NSString *)appCertificate
                             ChannelName:(const NSString *)channelName
                                     uid:(uint32_t)uid
{
    uint32_t unixTs = (uint32_t)[[NSDate date] timeIntervalSince1970];
    
    uint32_t randomInt = (::rand()%256 << 24) + (::rand()%256 << 16) + (::rand()%256 << 8) + (::rand()%256);
    uint32_t expiredTs = 0;//unixTs + 20;
    
    printf("AppId: %s\n", [appId UTF8String]);
    printf("Certificate: %s\n", [appCertificate UTF8String]);
    printf("channelName: %s\n", [channelName UTF8String]);
    printf("unixTS: %d\n", unixTs);
    printf("random: %d\n", randomInt);
    printf("uid: %d\n", uid);
    printf("expriedTs: %d\n", expiredTs);
    
    agora::tools::DynamicKey5 dynamic = agora::tools::DynamicKey5();
    
    std::string key = dynamic.generateMediaChannelKey([appId UTF8String], [appCertificate UTF8String], [channelName UTF8String], unixTs, randomInt, uid, expiredTs);
    return [[NSString alloc] initWithCString:key.c_str() encoding:NSUTF8StringEncoding];
}

@end
