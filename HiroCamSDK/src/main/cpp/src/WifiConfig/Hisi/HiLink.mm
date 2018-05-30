
#import "HisiConfigureNetworkSDK.h"

extern "C" void Hisi_StartConfigWifi(char* ssid, char* wifiPwd, char* devPwd)
{
    [HisiConfigureNetworkSDK StartConfigureNetworkWithSsid:[NSString stringWithUTF8String:ssid]
                                                  password:[NSString stringWithUTF8String:wifiPwd]
                                               devPassword:[NSString stringWithUTF8String:devPwd]];
}


extern "C" void Hisi_StopConfigWifi()
{
    [HisiConfigureNetworkSDK StopConfigureNetwork];
}
