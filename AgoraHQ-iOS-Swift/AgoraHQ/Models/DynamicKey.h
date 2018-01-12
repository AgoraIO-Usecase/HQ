//
//  DynamicKey.h
//  AgoraDynamicKeyBasic
//
//  Created by ZhangJi on 01/08/2017.
//  Copyright © 2017 Agora. All rights reserved.
//

#ifndef DynamicKey_h
#define DynamicKey_h


#endif /* DynamicKey_h */

#import <Foundation/Foundation.h>


@interface DynamicKey : NSObject
+ (NSString *) generateMediaChannelKeyby: (NSString *)appId
                             Certificate: (NSString *)appCertificate
                             ChannelName: (NSString *)channelName
                                     uid: (uint32_t)uid;

@end
