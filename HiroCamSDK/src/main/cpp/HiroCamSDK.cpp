#include <stdio.h>
#include "HiroCamSDK.h"
#include "HandleManager.h"
#include "DecoderManager.h"
#include "PPCS_API.h"
#include <android/log.h>

#define WIFICONFIG  0
#if WIFICONFIG
extern "C" void Hisi_StartConfigWifi(char* ssid, char* wifiPwd, char* devPwd);
extern "C" void Hisi_StopConfigWifi();
extern "C" void InitSinVoice();
extern "C" void SinVoice_StartConfigWifi(char* ssid, char* wifiPwd, char* devPwd);
extern "C" void SinVoice_StopConfigWifi();
#endif

using namespace HiroCamSDK;
extern "C"{
    
    unsigned int HiroCamSDK_Init()
    {
#if WIFICONFIG
        InitSinVoice();
#endif
        CStreamBuff::InitalMutex();
        DecoderManager::InitDecordManager();

        PPCS_Initialize((CHAR*)"EFGHFDBLKEIAGDJIEKHAFFENGFNAHHMBHOFABEDLABJFLIKMDMAKCDPLGELGIGLHADNJKIDDPPNDBHCHIP");
        UINT32 apiVer = PPCS_GetAPIVersion();
        printf("PPCS_API Version: %d.%d.%d.%d\n",
             (apiVer & 0xFF000000)>>24, (apiVer & 0x00FF0000)>>16,
             (apiVer & 0x0000FF00)>>8, (apiVer & 0x000000FF) >> 0 );
        return 0;
    }

    unsigned int HiroCamSDK_UnInit()
    {
        return 0;
    }
    
    
    unsigned int HiroCamSDK_CreateHandle(char* uid)
    {
        unsigned int handle;
        CApiImpl* pApi = CHandleManager::CreateApiManager(handle);
        if (pApi) {
            pApi->SetUsrInfo(uid);
        }
        return handle;
    }
    
    unsigned int HiroCamSDK_ReleaseHandle(unsigned int handle)
    {
        CHandleManager::DeleteApiManager(handle);
        return 0;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_Login(unsigned int handle, char* usr, char* pwd, int bLive, HRSDK_Login* loginRst, int* timeout)
    {
        if (!timeout) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->Login(usr, pwd, bLive, loginRst, *timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_LogOut(unsigned int handle)
    {
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->LogOut();
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_ChangePwd(unsigned int handle, char* oldPwd, char* newPwd, int* timeout)
    {
        if (!timeout) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->ChangePwd(oldPwd, newPwd, *timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_StartTalk(unsigned int handle, int* timeout)
    {
        if (!timeout) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->StartTalk(*timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_StopTalk(unsigned int handle, int* timeout)
    {
        if (!timeout) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->StopTalk(*timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_GetDevInfo(unsigned int handle, HRSDK_DevInfo* devInfo, int* timeout)
    {
        if (!timeout) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->GetDevInfo(devInfo, *timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_SetDevInfo(unsigned int handle, HRSDK_SetDevInfo devInfo, int* timeout)
    {
        if (!timeout) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->SetDevInfo(devInfo, *timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_GetPirInfo(unsigned int handle, HRSDK_PirInfo* pirInfo, int* timeout)
    {
        if (!timeout) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->GetPirInfo( pirInfo, *timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_SetPirInfo(unsigned int handle, HRSDK_PirInfo pirInfo, int* timeout)
    {
        if (!timeout) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->SetPirInfo( pirInfo, *timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_VideoFlip(unsigned int handle, int* timeout)
    {
        if (!timeout) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->VideoFlip(*timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_VideoMirror(unsigned int handle, int* timeout)
    {
        if (!timeout) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->VideoMirror(*timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_FormatDCardInfo(unsigned int handle, HRSDK_SDCardInfo* sdCardInfo, int* timeout)
    {
        if (!timeout) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->FormatSDCardInfo( sdCardInfo, *timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_GetRecordMap(unsigned int handle, int year, int month, int* recordMap, int* timeout)
    {
        if (!recordMap || !timeout) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->GetRecordMap( year, month, recordMap, *timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_GetRecordList(unsigned int handle, int year, int month, int day, HRSDK_RecordListHead* list, int listBufSize, int* timeout)
    {
        if (!list || !timeout || listBufSize < sizeof(HRSDK_RecordListHead)) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->GetRecordList( year, month, day, list, listBufSize, *timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_GetVideoData(unsigned int handle, HRSDK_DecFrame **data, int *outLen, HRDECFMT videoFmt)
    {
        if (!data || !outLen || !videoFmt) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->GetVideoData( data, outLen, videoFmt);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_GetAudioData(unsigned int handle, HRSDK_DecFrame **data, int *outLen)
    {
        if (!data || !outLen) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->GetAudioData( data, outLen);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_SendTalkData(unsigned int handle, char* data, int len)
    {
        if (!data || len <= 0) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->SendTalkData( data, len);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_StartPlayback(unsigned int handle, char* filename, HRSDK_RecordInfo* rcdInfo, int *timeout)
    {
        if (!filename) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->StartPlayback(filename, rcdInfo, timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_StopPlayback(unsigned int handle, int* timeout)
    {
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->StopPlayback(timeout);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_StartRecord(unsigned int handle, HRRECORDTYPE type,const char *filename)
    {
        if (!filename) {
            return HRSDKCMDRST_ARG_ERR;
        }
        
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->StartRecord(type, filename);
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    HRSDK_CMDRESULT HiroCamSDK_StopRecord(unsigned int handle)
    {
        CApiImpl* pApi = CHandleManager::GetApiManager(handle);
        if (pApi) {
            HRSDK_CMDRESULT rst = pApi->StopRecord();
            CHandleManager::ReleaseApiManager(handle);
            return rst;
        }
        return HRSDKCMDRST_HNDLE_ERR;
    }
    
    void HiroCamSDK_StartConfigWifi(char* ssid, char* wifiPwd, char* devPwd)
    {
#if WIFICONFIG
        Hisi_StartConfigWifi(ssid, wifiPwd, devPwd);
        SinVoice_StartConfigWifi(ssid, wifiPwd, devPwd);
#endif
    }
    
    void HiroCamSDK_StopConfigWifi()
    {
#if WIFICONFIG
        Hisi_StopConfigWifi();
        SinVoice_StopConfigWifi();
#endif
    }
}
