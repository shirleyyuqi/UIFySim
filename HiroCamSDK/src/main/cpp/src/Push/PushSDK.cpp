#include <stdio.h>
#include "BaseDataType.h"
#include "HiroCamSDK.h"
#include "NDT_API.h"
#include "NDT_Error.h"
extern "C"{
#include "WiPN_API.h"
}
using namespace HiroCamSDK;

typedef struct tagWiPNServerInfo
{
    HR_S32  bInit;
    HR_CS8* key;
    HR_CS8* initString;
    HR_CS8* aesKey;
    HR_CS8* encKey;
    HR_CS8* queryServer[6];
}WiPNServerInfo;

static WiPNServerInfo g_WiPNServer[] = {
    {
        0,
        "AACS",
        "EFGHFDBLKEIAGDJIEKHAFFENGFNAHHMBHOFABEDLABJFLIKMDMAKCDPLGELGIGLHADNJKIDDPPNDBHCHIP",
        "VG3EC75ncHZlqvaf",
        "PvqVDvFp7ZYET6VL",
        {"AACS-000003-PYGNU", "AACS-000004-CTPNC", "AACS-000005-PHMDM", "AACS-000006-UHDBW", "AACS-000007-KYWWN", "AACS-000008-XXJFL"}
    },
    {
        0,
        "ABCS",
        "EEGDFHBMKGICGCJMEOHEFLEFGNNKHKNDHDFPBBDOAHJJLGKCDBADDFPAGLLCICLCAHNNKODAPMNDBOCOIG",
        "Ho1c7MFutNi8deRj",
        "BKHlSkqZqQglW3Dv",
        {"ABCS-000003-WBHTY", "ABCS-000004-KUBKJ", "ABCS-000005-CYFWH", "ABCS-000006-HJXBT", "ABCS-000007-GZMXL", "ABCS-000008-KZBWL"}
    },
    {
        0,
        "PPCS",
        "EBGAEIBIKHJJGFJKEOGCFAEPHPMAHONDGJFPBKCPAJJMLFKBDBAGCJPBGOLKIKLKAJMJKFDOOFMOBECEJIMM",
        "0123456789ABCDEF",
        "WiPN@CS2-Network",
        {"PPCS-014143-SBKHR", "PPCS-014144-RVDKK", "PPCS-014143-SBKHR", "PPCS-014144-RVDKK", "PPCS-014143-SBKHR", "PPCS-014144-RVDKK"}
    },
};

static int g_bInit = 0;
char g_SubscribeServerString[SIZE_SubscribeServerString] = {0};
char g_UTCTString[SIZE_UTCTString] = {0};

extern "C"{
    
    int setCmd(    char *pCmd,
               unsigned short SizeOfSubCmd,
               const char *pDeviceDID,
               unsigned long EventCH,
               const char *pAG_Name,
               const char *pAPP_Name,
               const char *pDeviceToken,
               const char *pACT)
    {
        if (   !pCmd
            || !pDeviceDID
            || !pAG_Name
            || !pAPP_Name
            || !pACT
            || 0 == SizeOfSubCmd
            || 0 == strlen(pDeviceDID)
            || 0 == strlen(pAG_Name)
            || 0 == strlen(pAPP_Name)
            || 0 == strlen(pACT)
            || 0xFFFFFFFF < EventCH)
        {
            return WiPN_ERROR_InvalidParameter;
        }
        if (!pDeviceToken || 0 == strlen(pDeviceToken))
        {
            printf("\n****Error: DeviceToken is empty!!\n");
            return WiPN_ERROR_DeviceTokenIsEmpty;
        }
        unsigned short LengthOfParameter = strlen(pDeviceDID)
        +sizeof(EventCH)
        +strlen(pAG_Name)
        +strlen(pAPP_Name)
        +strlen(pDeviceToken)
        +strlen(pACT);
        if (LengthOfParameter > SizeOfSubCmd - 1)
        {
            return WiPN_ERROR_ExceedMaxSize;
        }
        memset(pCmd, '\0', SizeOfSubCmd);
        
        // Cmd
        SNPRINTF(pCmd, SizeOfSubCmd, "DID=%s&CH=%lu&AG=%s&APP=%s&INFO=%s&ACT=%s&", pDeviceDID, EventCH, pAG_Name, pAPP_Name, pDeviceToken, pACT);
        
        return 0;
    }
    
    int SubScribe_Init(char* uid, int* pIdxSvr)
    {
        char tmp[256];
        strcpy(tmp, uid);
        char* pStr = strchr(tmp, '-');
        pStr[0] = 0;
        
        int idx = 0;
        for (; idx<sizeof(g_WiPNServer)/sizeof(g_WiPNServer[0]) && !g_bInit; idx++) {
            if (strcmp(g_WiPNServer[idx].key, tmp) == 0) {
                if (!g_bInit){
                    //if(!g_WiPNServer[i].bInit) {
                    int ret = NDT_PPCS_Initialize(g_WiPNServer[idx].initString, 0, NULL, g_WiPNServer[idx].aesKey);
                    if (0 > ret)
                    {
                        printf("Initialize ret = %d [%s]\n", ret, getErrorCodeInfo(ret));
                        return -1;
                    }
                    else
                    {
                        g_bInit = 1;
                        g_WiPNServer[idx].bInit = 1;
                        printf("Initialize Success!! ret = %d\n", ret);
                    }
                    break;
                }
            }
        }
        
        for (idx = 0; idx<sizeof(g_WiPNServer)/sizeof(g_WiPNServer[0]); idx ++) {
            if (strcmp(g_WiPNServer[idx].key, tmp) == 0) {
                break;
            }
        }
        
        if (idx >= sizeof(g_WiPNServer)/sizeof(g_WiPNServer[0])) {
            return -2;
        }
        
        st_NDT_NetInfo NetInfo;
        NDT_PPCS_NetworkDetect(&NetInfo, 3000); // wait for 3 sec
        printf("My Lan IP:Port=%s:%d\n", NetInfo.LanIP, NetInfo.LanPort);
        printf("My Wan IP:Port=%s:%d\n", NetInfo.WanIP, NetInfo.WanPort);
        printf("Server Hello Ack: %s\n", 1 == NetInfo.bServerHelloAck ? "Yes":"No");
        
        if (0 == NetInfo.bServerHelloAck){
            printf("*** Warning!! CS didn't response!! Client from Internet won't be able to send command to Device.\n");
            NDT_PPCS_DeInitialize();
            g_bInit = 0;
            return -3;
        }
        
        int ret = WiPN_QueryByServer((char*)g_WiPNServer[idx].encKey,
                                     g_WiPNServer[idx].initString,
                                     g_WiPNServer[idx].aesKey,
                                     uid,
                                     g_WiPNServer[idx].queryServer,
                                     NULL,   // not need PostServerString
                                     0,
                                     g_SubscribeServerString,
                                     sizeof(g_SubscribeServerString),
                                     g_UTCTString,
                                     sizeof(g_UTCTString));
        if (0 > ret){
            printf("Get SubscribeServerString failed! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
            return -4;
        }else{
            printf("WiPN_Query: SubscribeServerString= %s\nWiPN_Query: UTCTString= %s\n\n", g_SubscribeServerString, g_UTCTString);
        }
        
        *pIdxSvr = idx;
        return 0;
    }
    
    
    int SubScribe_Proc(char* uid, int idx, char* agName, char* appName, char* devToken, int eventChn, const char* act)
    {
        char RETString[SIZE_RETString];
        memset(RETString, 0, sizeof(RETString));
        
        // set Cmd
        //unsigned short cmdSize = SIZE_DID + (int)strlen(agName)+1 + (int)strlen(appName)+1 + SIZE_DeviceToken + (int)strlen(act)+1;
        //(char *)malloc(cmdSize);
        char Cmd[1024] = {0};
        
        
        int ret = setCmd((char*)Cmd, sizeof(Cmd), // SubCmd Buf Size
                         (const char*)uid,     // Device DID
                         (unsigned long)eventChn,         // Event Channel: 0~0xFFFFFFFF
                         (const char*)agName,         // Agent Name,Such as: APNS, XinGe...
                         (const char*)appName,     // App Name
                         (const char*)devToken,   // DeviceToken
                         act);    // ACT: Subscribe/UnSubscribe
        if (0 > ret){
            printf("set Cmd failed!! ret= %d. [%s]\n", ret, getErrorCodeInfo(ret));
            return -1;
        }
        
        if (0 == strcmp(act, "Subscribe")){
            static unsigned long SubscribeCount = 0;
            printf("\nBeing Subscribe...%03lu\n", ++SubscribeCount);
            
            ret = WiPN_SubscribeByServer(g_WiPNServer[idx].initString,
                                         g_WiPNServer[idx].aesKey,
                                         (const char*)g_SubscribeServerString,
                                         (const char*)Cmd,
                                         RETString,
                                         sizeof(RETString),
                                         g_UTCTString,
                                         sizeof(g_UTCTString));
            if (0 > ret)
            {
                printf("thread_WiPN_Subscribe - Subscribe failed! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
            }
            else
            {
                time_t ServerTime = strtol(g_UTCTString, NULL, 16);
                struct tm *ptm = localtime((const time_t *)&ServerTime);
                if (!ptm){
                    printf("thread_WiPN_Subscribe - RET= %s\nthread_WiPN_Subscribe - UTCT= %s\n", RETString, g_UTCTString);
                }else{
                    printf("thread_WiPN_Subscribe - RET= %s\nthread_WiPN_Subscribe - ServerTime: %d-%02d-%02d %02d:%02d:%02d\n", RETString, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
                }
                
                if (0 == strcmp(RETString, "OK")){
                    printf("thread_WiPN_Subscribe - Subscribe success!\n");
                }else{
                    printf("thread_WiPN_Subscribe - Subscribe failed!\n");
                }
                printf("-------------------------------------------------------\n\n");
            }
        }else if (0 == strcmp(act, "UnSubscribe")){
            printf("\nBeing UnSubscribe...\n");
            ret = WiPN_UnSubscribeByServer(g_WiPNServer[idx].initString,
                                           g_WiPNServer[idx].aesKey,
                                           g_SubscribeServerString,
                                           Cmd,
                                           RETString,
                                           sizeof(RETString),
                                           g_UTCTString,
                                           sizeof(g_UTCTString));
            if (0 > ret){
                printf("WiPN_UnSubscribe: UnSubscribe failed! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
                return -2;
            }else{
                printf("WiPN_UnSubscribe: RET= %s\nWiPN_UnSubscribe: UTCT= %s\n", RETString, g_UTCTString);
                if (0 == strcmp(RETString, "OK")){
                    printf("WiPN_UnSubscribe: UnSubscribe success!\n");
                    
                }else{
                    printf("WiPN_UnSubscribe: UnSubscribe failed!\n");
                    return -3;
                }
            }
            
            printf("DeInitialize done!\n");
            
        }else if(0 == strcmp(act, "ResetBadge")){
            ret = WiPN_ResetBadgeByServer(g_WiPNServer[idx].initString,
                                          g_WiPNServer[idx].aesKey,
                                          g_SubscribeServerString,
                                          Cmd,
                                          RETString,
                                          sizeof(RETString),
                                          g_UTCTString,
                                          sizeof(g_UTCTString));
            
            
            if (0 > ret){
                printf("WiPN_ResetBadge failed! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
                return -4;
            }else{
                time_t ServerTime = strtol(g_UTCTString, NULL, 16);
                struct tm *ptm = localtime((const time_t *)&ServerTime);
                if (!ptm){
                    printf("WiPN_ResetBadge - RETString= %s\nWiPN_ResetBadge - UTCT= %s", RETString, g_UTCTString);
                }else{
                    printf("WiPN_ResetBadge - RETString= %s\nWiPN_ResetBadge - ServerTime: %d-%02d-%02d %02d:%02d:%02d\n", RETString, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
                }
            }
        }
        return 0;
    }
    
    int HiroCamPushSDK_SubScribe(char* uid, char* agName, char* appName, char* devToken, int eventChn)
    {
        int idx = 0;
        int bInit = SubScribe_Init(uid, &idx);
        if (0 != bInit) {
            return -1;
        }
        
        int ret = SubScribe_Proc(uid, idx, agName, appName, devToken, eventChn, "Subscribe");
        if (0 != ret) {
            return -2;
        }
        return 0;
    }
    
    int HiroCamPushSDK_UnSubScribe(char* uid, char* agName, char* appName, char* devToken, int eventChn)
    {
        int idx = 0;
        int bInit = SubScribe_Init(uid, &idx);
        if (0 != bInit) {
            return -1;
        }
        
        int ret = SubScribe_Proc(uid, idx, agName, appName, devToken, eventChn, "UnSubscribe");
        if (0 != ret) {
            return -2;
        }
        return 0;
    }
    
    int HiroCamPushSDK_ResetBadge(char* uid, char* agName, char* appName, char* devToken, int eventChn)
    {
        int idx = 0;
        int bInit = SubScribe_Init(uid, &idx);
        if (0 != bInit) {
            return -1;
        }
        
        int ret = SubScribe_Proc(uid, idx, agName, appName, devToken, eventChn, "ResetBadge");
        if (0 != ret) {
            return -2;
        }
        return 0;
    }
}
