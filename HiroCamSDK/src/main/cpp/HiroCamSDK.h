
#ifndef __HIROCAMSDK_H__
#define __HIROCAMSDK_H__

#include "HiroCamDef.h"
#ifdef __cplusplus
extern "C"{
#endif

    /**********************************************Camare Process**********************************************************************/
    unsigned int HiroCamSDK_Init();
    unsigned int HiroCamSDK_UnInit();

    unsigned int HiroCamSDK_CreateHandle(char* uid);
    unsigned int HiroCamSDK_ReleaseHandle(unsigned int handle);
    
    HRSDK_CMDRESULT HiroCamSDK_Login(unsigned int handle, char* usr, char* pwd, int bLive, HRSDK_Login* loginRst, int* timeout);
    HRSDK_CMDRESULT HiroCamSDK_LogOut(unsigned int handle);
    
    HRSDK_CMDRESULT HiroCamSDK_StartTalk(unsigned int handle, int* timeout);
    HRSDK_CMDRESULT HiroCamSDK_StopTalk(unsigned int handle, int* timeout);
    
    HRSDK_CMDRESULT HiroCamSDK_ChangePwd(unsigned int handle, char* oldPwd, char* newPwd, int* timeout);
    
    HRSDK_CMDRESULT HiroCamSDK_GetDevInfo(unsigned int handle, HRSDK_DevInfo* devInfo, int* timeout);
    HRSDK_CMDRESULT HiroCamSDK_SetDevInfo(unsigned int handle, HRSDK_SetDevInfo devInfo, int* timeout);
    
    HRSDK_CMDRESULT HiroCamSDK_GetPirInfo(unsigned int handle, HRSDK_PirInfo* pirInfo, int* timeout);
    HRSDK_CMDRESULT HiroCamSDK_SetPirInfo(unsigned int handle, HRSDK_PirInfo pirInfo, int* timeout);
    
    HRSDK_CMDRESULT HiroCamSDK_VideoFlip(unsigned int handle, int* timeout);
    HRSDK_CMDRESULT HiroCamSDK_VideoMirror(unsigned int handle, int* timeout);
    
    HRSDK_CMDRESULT HiroCamSDK_FormatDCardInfo(unsigned int handle, HRSDK_SDCardInfo* sdCardInfo, int* timeout);
    HRSDK_CMDRESULT HiroCamSDK_GetRecordMap(unsigned int handle, int year, int month, int* recordMap, int* timeout);
    
    HRSDK_CMDRESULT HiroCamSDK_GetRecordList(unsigned int handle, int year, int month, int day, HRSDK_RecordListHead* list, int listBufSize, int* timeout);
    
    HRSDK_CMDRESULT HiroCamSDK_SendTalkData(unsigned int handle, char* data, int len);
    HRSDK_CMDRESULT HiroCamSDK_GetVideoData(unsigned int handle, HRSDK_DecFrame **data, int *outLen, HRDECFMT videoFmt);
    HRSDK_CMDRESULT HiroCamSDK_GetAudioData(unsigned int handle, HRSDK_DecFrame **data, int *outLen);
    
    HRSDK_CMDRESULT HiroCamSDK_StartPlayback(unsigned int handle, char* filename, HRSDK_RecordInfo* rcdInfo, int* timeout);
    HRSDK_CMDRESULT HiroCamSDK_StopPlayback(unsigned int handle, int* timeout);
    
    HRSDK_CMDRESULT HiroCamSDK_StartRecord(unsigned int handle, HRRECORDTYPE type,const char *filename);
    HRSDK_CMDRESULT HiroCamSDK_StopRecord(unsigned int handle);
    
    /****************************************** Subscirbe/Push ************************************************************************************/
    int HiroCamPushSDK_SubScribe(char* uid, char* agName, char* appName, char* devToken, int eventChn);
    int HiroCamPushSDK_UnSubScribe(char* uid, char* agName, char* appName, char* devToken, int eventChn);
    int HiroCamPushSDK_ResetBadge(char* uid, char* agName, char* appName, char* devToken, int eventChn);
    
    /****************************************** keep alive ************************************************************************************/
    HRKEEPALIVESDK_RESULT HiroCamKlSDK_GetResult(const char* uid);
    HRKEEPALIVESDK_RESULT HiroCamKlSDK_Query(const char* uid);
    
    /****************************************** wifi config ************************************************************************************/
    void HiroCamSDK_StartConfigWifi(char* ssid, char* wifiPwd, char* devPwd);
    void HiroCamSDK_StopConfigWifi();
#ifdef __cplusplus
}
#endif

#endif
