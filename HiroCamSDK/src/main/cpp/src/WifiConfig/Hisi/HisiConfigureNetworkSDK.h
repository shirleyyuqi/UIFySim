//
//  HisiConfigureNetworkSDK.h
//  HisiConfigureNetworkSDK
//
//  Created by hwh on 2017/7/24.
//  Copyright © 2017年 xlink. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum TAG_CON_NETWORK_RETURN
{
    CON_NETWORK_RETURN_FALES = -1,
    CON_NETWORK_RETURN_OK = 0,
}CON_NETWORK_RETURN;

@interface HisiConfigureNetworkSDK : NSObject

+(int)StartConfigureNetworkWithSsid:(NSString *)wifiSsid
                           password:(NSString *)wifiPassword
                        devPassword:(NSString *)devPassword;

+(int)StopConfigureNetwork;

+(NSString *)getVersion;

@end
