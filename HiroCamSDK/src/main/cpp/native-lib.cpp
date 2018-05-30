#include <jni.h>
#include <string>
#include <cstdlib>
#include <android/bitmap.h>
#include "HiroCamDef.h"
#include "HiroCamSDK.h"

extern "C" {
    JNIEXPORT jstring JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_stringFromJNI(JNIEnv *env,  jobject /* this */) {
        std::string hello = "Hello from C++1113";
        return env->NewStringUTF(hello.c_str());
    }

    JNIEXPORT jlong JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_GetTimeOutPrt(JNIEnv *env,  jobject /* this */, jint value) {
        int* pTm = (int*)malloc(sizeof(int));
        *pTm = value;
        return (jlong)pTm;
    }

    JNIEXPORT void JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_ReleaseTimeOutPrt(JNIEnv *env,  jobject /* this */, jlong ptr) {
        int* pTm = (int*)ptr;
        free(pTm);
    }

    JNIEXPORT void JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_SetTimeOutValue(JNIEnv *env,  jobject /* this */, jlong ptr, jint value) {
        int* pTm = (int*)ptr;
        *pTm = value;
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_GetTimeOutValue(JNIEnv *env,  jobject /* this */, jlong ptr) {
        int* pTm = (int*)ptr;
        return (jint)*pTm;
    }

    JNIEXPORT void JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_Init(JNIEnv *env,  jobject /* this */) {
        HiroCamSDK_Init();
    }

    JNIEXPORT void JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_UnInit(JNIEnv *env,  jobject /* this */) {
        HiroCamSDK_UnInit();
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_CreateHandle(JNIEnv *env,  jobject /* this */, jstring uid) {
        return HiroCamSDK_CreateHandle((char*)env->GetStringUTFChars(uid, 0));
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_ReleaseHandle(JNIEnv *env,  jobject /* this */, jint handle) {
        return HiroCamSDK_ReleaseHandle(handle);
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_Login
            (JNIEnv *env, jobject obj, jint handle, jstring usr, jstring pwd, jint bLive, jobject loginRst, jlong timeOutPtr)
    {
        char* pUsr = (char*)env->GetStringUTFChars(usr, 0);
        char* pPwd = (char*)env->GetStringUTFChars(pwd, 0);
        int *timeOut = (int*)timeOutPtr;
        HRSDK_Login tLoginRst = {0};
        HRSDK_CMDRESULT ret = HiroCamSDK_Login(handle, pUsr, pPwd, bLive, &tLoginRst, timeOut);
        if (HRSDKCMDRST_SUCCESS != ret){
            return ret;
        }

        jclass objClass = env->FindClass("hiro/com/hirocamsdk/HRSDKLogin");
        jfieldID fieldDevVer = env->GetFieldID( objClass, "devVer", "I" );
        jfieldID fieldEventChannel = env->GetFieldID( objClass, "eventChannel", "I" );
        jfieldID fieldIBat = env->GetFieldID( objClass, "iBat", "I" );
        jfieldID fieldBInCharge = env->GetFieldID( objClass, "bInCharge", "I" );
        jfieldID fieldIUsrRight = env->GetFieldID( objClass, "iUsrRight", "I" );
        jfieldID fieldBFlip = env->GetFieldID( objClass, "bFlip", "I" );
        jfieldID fieldBMirror = env->GetFieldID( objClass, "bMirror", "I" );

        env->SetIntField(loginRst, fieldDevVer, tLoginRst.devVer);
        env->SetIntField(loginRst, fieldEventChannel, tLoginRst.eventChannel);
        env->SetIntField(loginRst, fieldIBat, tLoginRst.iBat);
        env->SetIntField(loginRst, fieldBInCharge, tLoginRst.bInCharge);
        env->SetIntField(loginRst, fieldIUsrRight, tLoginRst.iUsrRight);
        env->SetIntField(loginRst, fieldBFlip, tLoginRst.bFlip);
        env->SetIntField(loginRst, fieldBMirror, tLoginRst.bMirror);
        return ret;
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_Logout(JNIEnv *env,  jobject /* this */, jint handle) {
        return HiroCamSDK_LogOut(handle);
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_StartTalk(JNIEnv *env,  jobject /* this */, jint handle, jlong timeOutPrt) {
        int *timeOut = (int*)timeOutPrt;
        return HiroCamSDK_StartTalk(handle, timeOut);
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_StopTalk(JNIEnv *env,  jobject /* this */, jint handle, jlong timeOutPrt) {
        int *timeOut = (int*)timeOutPrt;
        return HiroCamSDK_StopTalk(handle, timeOut);
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_ChangePwd(JNIEnv *env,  jobject /* this */, jint handle,jstring oldPwd, jstring newPwd, jlong timeOutPrt) {
        int *timeOut = (int*)timeOutPrt;
        char* pOld = (char*)env->GetStringUTFChars(oldPwd, 0);
        char* pNew = (char*)env->GetStringUTFChars(newPwd, 0);
        return HiroCamSDK_ChangePwd(handle, pOld, pNew, timeOut);
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_GetDevInfo(JNIEnv *env,  jobject /* this */, jint handle,jobject getDevInfo, jlong timeOutPrt) {
        HRSDK_DevInfo devInfo;
        int *timeOut = (int*)timeOutPrt;
        HRSDK_CMDRESULT ret = HiroCamSDK_GetDevInfo(handle, &devInfo, timeOut);
        if (HRSDKCMDRST_SUCCESS != ret){
            return ret;
        }

        jclass objClass = env->FindClass("hiro/com/hirocamsdk/GetDevInfo");
        jfieldID fieldproductName = env->GetFieldID( objClass, "productName", "Ljava/lang/String;" );
        jfieldID fieldverCode = env->GetFieldID( objClass, "verCode", "I" );
        jfieldID fieldpowerFreq = env->GetFieldID( objClass, "powerFreq", "I" );
        jfieldID fieldiBat = env->GetFieldID( objClass, "iBat", "I" );
        jfieldID fieldbInCharge = env->GetFieldID( objClass, "bInCharge", "I" );
        jfieldID fieldtimeZone = env->GetFieldID( objClass, "timeZone", "I" );
        jfieldID fieldlangCode = env->GetFieldID( objClass, "langCode", "I" );
        jfieldID fieldsdTotalMB = env->GetFieldID( objClass, "sdTotalMB", "I" );
        jfieldID fieldsdFreeMB = env->GetFieldID( objClass, "sdFreeMB", "I" );
        jfieldID fieldsdCanRecordDays = env->GetFieldID( objClass, "sdCanRecordDays", "I" );
        jfieldID fieldrecordMode = env->GetFieldID( objClass, "recordMode", "I" );
        jfieldID fieldtalkVol = env->GetFieldID( objClass, "talkVol", "I" );

        env->SetObjectField(getDevInfo, fieldproductName,env->NewStringUTF(devInfo.productName) );
        env->SetIntField(getDevInfo, fieldverCode, devInfo.verCode);
        env->SetIntField(getDevInfo, fieldpowerFreq, devInfo.powerFreq);
        env->SetIntField(getDevInfo, fieldiBat, devInfo.iBat);
        env->SetIntField(getDevInfo, fieldbInCharge, devInfo.bInCharge);
        env->SetIntField(getDevInfo, fieldtimeZone, devInfo.timeZone);
        env->SetIntField(getDevInfo, fieldlangCode, devInfo.langCode);
        env->SetIntField(getDevInfo, fieldsdTotalMB, devInfo.sdTotalMB);
        env->SetIntField(getDevInfo, fieldsdFreeMB, devInfo.sdFreeMB);
        env->SetIntField(getDevInfo, fieldsdCanRecordDays, devInfo.sdCanRecordDays);
        env->SetIntField(getDevInfo, fieldrecordMode, devInfo.recordMode);
        env->SetIntField(getDevInfo, fieldtalkVol, devInfo.talkVol);
        return ret;
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_SetDevInfo(JNIEnv *env,  jobject /* this */, jint handle,jobject setDevInfo, jlong timeOutPrt) {
        int *timeOut = (int*)timeOutPrt;
        HRSDK_SetDevInfo tSetDevInfo;
        memset( &setDevInfo, 0x00, sizeof(tSetDevInfo) );

        jclass objClass = env->FindClass("hiro/com/hirocamsdk/SetDevInfo");
        jfieldID fieldutcTime = env->GetFieldID( objClass, "utcTime", "I" );
        jfieldID fieldtimeZone = env->GetFieldID( objClass, "timeZone", "I" );
        jfieldID fieldpowerFreq = env->GetFieldID( objClass, "powerFreq", "I" );
        jfieldID fieldrecordMode = env->GetFieldID( objClass, "recordMode", "I" );
        jfieldID fieldlangCode = env->GetFieldID( objClass, "langCode", "I" );
        jfieldID fieldtalkVol = env->GetFieldID( objClass, "talkVol", "I" );

        tSetDevInfo.utcTime = env->GetIntField(setDevInfo , fieldutcTime );
        tSetDevInfo.timeZone = env->GetIntField(setDevInfo , fieldtimeZone );
        tSetDevInfo.powerFreq = env->GetIntField(setDevInfo , fieldpowerFreq );
        tSetDevInfo.recordMode = env->GetIntField(setDevInfo , fieldrecordMode );
        tSetDevInfo.langCode = env->GetIntField(setDevInfo , fieldlangCode );
        tSetDevInfo.talkVol = env->GetIntField(setDevInfo , fieldtalkVol );

        return HiroCamSDK_SetDevInfo(handle, tSetDevInfo, timeOut);
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_GetPirInfo(JNIEnv *env,  jobject /* this */, jint handle,jobject getPirInfo, jlong timeOutPrt) {
        int *timeOut = (int*)timeOutPrt;
        HRSDK_PirInfo pirInfo;
        HRSDK_CMDRESULT ret = HiroCamSDK_GetPirInfo(handle, &pirInfo, timeOut);
        if (HRSDKCMDRST_SUCCESS != ret){
            return ret;
        }

        jclass objClass = env->FindClass("hiro/com/hirocamsdk/GetPirInfo");
        jfieldID fieldbEnable = env->GetFieldID( objClass, "bEnable", "I" );
        jfieldID fieldsensitivity = env->GetFieldID( objClass, "sensitivity", "I" );
        jfieldID fieldweekMap = env->GetFieldID( objClass, "weekMap", "I" );
        jfieldID fielddetectTimeSun = env->GetFieldID( objClass, "detectTimeSun", "I" );
        jfieldID fielddetectTimeMon = env->GetFieldID( objClass, "detectTimeMon", "I" );
        jfieldID fielddetectTimeTue = env->GetFieldID( objClass, "detectTimeTue", "I" );
        jfieldID fielddetectTimeWed = env->GetFieldID( objClass, "detectTimeWed", "I" );
        jfieldID fielddetectTimeThu = env->GetFieldID( objClass, "detectTimeThu", "I" );
        jfieldID fielddetectTimeFri = env->GetFieldID( objClass, "detectTimeFri", "I" );
        jfieldID fielddetectTimeSat = env->GetFieldID( objClass, "detectTimeSat", "I" );
        jfieldID fieldrecordDuration = env->GetFieldID( objClass, "recordDuration", "I" );

        env->SetIntField(getPirInfo, fieldbEnable, pirInfo.bEnable );
        env->SetIntField(getPirInfo, fieldsensitivity, pirInfo.sensitivity);
        env->SetIntField(getPirInfo, fieldweekMap, pirInfo.weekMap);
        env->SetIntField(getPirInfo, fielddetectTimeSun, pirInfo.detectTimeSun);
        env->SetIntField(getPirInfo, fielddetectTimeMon, pirInfo.detectTimeMon);
        env->SetIntField(getPirInfo, fielddetectTimeTue, pirInfo.detectTimeTue);
        env->SetIntField(getPirInfo, fielddetectTimeWed, pirInfo.detectTimeWed);
        env->SetIntField(getPirInfo, fielddetectTimeThu, pirInfo.detectTimeThu);
        env->SetIntField(getPirInfo, fielddetectTimeFri, pirInfo.detectTimeFri);
        env->SetIntField(getPirInfo, fielddetectTimeSat, pirInfo.detectTimeSat);
        env->SetIntField(getPirInfo, fieldrecordDuration, pirInfo.recordDuration);
        return ret;
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_SetPirInfo(JNIEnv *env,  jobject /* this */, jint handle,jobject setPirInfo, jlong timeOutPrt) {
        int *timeOut = (int *) timeOutPrt;
        HRSDK_PirInfo pirInfo;

        jclass objClass = env->FindClass("hiro/com/hirocamsdk/SetPirInfo");
        jfieldID fieldbEnable = env->GetFieldID( objClass, "bEnable", "I" );
        jfieldID fieldsensitivity = env->GetFieldID( objClass, "sensitivity", "I" );
        jfieldID fieldweekMap = env->GetFieldID( objClass, "weekMap", "I" );
        jfieldID fielddetectTimeSun = env->GetFieldID( objClass, "detectTimeSun", "I" );
        jfieldID fielddetectTimeMon = env->GetFieldID( objClass, "detectTimeMon", "I" );
        jfieldID fielddetectTimeTue = env->GetFieldID( objClass, "detectTimeTue", "I" );
        jfieldID fielddetectTimeWed = env->GetFieldID( objClass, "detectTimeWed", "I" );
        jfieldID fielddetectTimeThu = env->GetFieldID( objClass, "detectTimeThu", "I" );
        jfieldID fielddetectTimeFri = env->GetFieldID( objClass, "detectTimeFri", "I" );
        jfieldID fielddetectTimeSat = env->GetFieldID( objClass, "detectTimeSat", "I" );
        jfieldID fieldrecordDuration = env->GetFieldID( objClass, "recordDuration", "I" );

        pirInfo.bEnable = env->GetIntField(setPirInfo , fieldbEnable );
        pirInfo.sensitivity = env->GetIntField(setPirInfo , fieldsensitivity );
        pirInfo.weekMap = env->GetIntField(setPirInfo , fieldweekMap );
        pirInfo.detectTimeSun = env->GetIntField(setPirInfo , fielddetectTimeSun );
        pirInfo.detectTimeMon = env->GetIntField(setPirInfo , fielddetectTimeMon );
        pirInfo.detectTimeTue = env->GetIntField(setPirInfo , fielddetectTimeTue );
        pirInfo.detectTimeWed = env->GetIntField(setPirInfo , fielddetectTimeWed );
        pirInfo.detectTimeThu = env->GetIntField(setPirInfo , fielddetectTimeThu );
        pirInfo.detectTimeFri = env->GetIntField(setPirInfo , fielddetectTimeFri );
        pirInfo.detectTimeSat = env->GetIntField(setPirInfo , fielddetectTimeSat );
        pirInfo.recordDuration = env->GetIntField(setPirInfo , fieldrecordDuration );

        return HiroCamSDK_SetPirInfo(handle, pirInfo, timeOut);
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_VideoFlip(JNIEnv *env,  jobject /* this */, jint handle, jlong timeOutPrt) {
        int *timeOut = (int*)timeOutPrt;
        return HiroCamSDK_VideoFlip(handle, timeOut);
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_VideoMirror(JNIEnv *env,  jobject /* this */, jint handle, jlong timeOutPrt) {
        int *timeOut = (int*)timeOutPrt;
        return HiroCamSDK_VideoMirror(handle, timeOut);
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_FormatDCardInfo(JNIEnv *env,  jobject /* this */, jint handle,jobject sdCardInfo, jlong timeOutPrt) {
        int *timeOut = (int *) timeOutPrt;
        HRSDK_SDCardInfo sdInfo;
        HRSDK_CMDRESULT ret = HiroCamSDK_FormatDCardInfo(handle, &sdInfo, timeOut);
        if (HRSDKCMDRST_SUCCESS != ret){
            return ret;
        }

        jclass objClass = env->FindClass("hiro/com/hirocamsdk/SDCardInfo");
        jfieldID fieldsdTotalMB = env->GetFieldID( objClass, "sdTotalMB", "I" );
        jfieldID fieldsdFreeMB = env->GetFieldID( objClass, "sdFreeMB", "I" );
        jfieldID fieldsdCanRecordDays = env->GetFieldID( objClass, "sdCanRecordDays", "I" );

        env->SetIntField(sdCardInfo, fieldsdTotalMB, sdInfo.sdTotalMB );
        env->SetIntField(sdCardInfo, fieldsdFreeMB, sdInfo.sdFreeMB);
        env->SetIntField(sdCardInfo, fieldsdCanRecordDays, sdInfo.sdCanRecordDays);

        return ret;
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_GetRecordMap(JNIEnv *env,  jobject /* this */, jint handle, jint year, jint month,jobject rcdMap, jlong timeOutPrt) {
        int *timeOut = (int *) timeOutPrt;
        int map;
        HRSDK_CMDRESULT ret = HiroCamSDK_GetRecordMap(handle,year, month, &map, timeOut);
        if (HRSDKCMDRST_SUCCESS != ret){
            return ret;
        }

        jclass objClass = env->FindClass("hiro/com/hirocamsdk/RecordMap");
        jfieldID fieldMap = env->GetFieldID( objClass, "map", "I" );
        env->SetIntField(rcdMap, fieldMap, map );
        return ret;
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_GetRecordList(JNIEnv *env, jobject obj, jint handle, jint year, jint month, jint day, jobject list, jlong timeOutPrt){
        int *timeOut = (int *) timeOutPrt;
        char buf[sizeof(HRSDK_RecordListData)*100 + sizeof(HRSDK_RecordListHead)];
        HRSDK_RecordListHead* pList = (HRSDK_RecordListHead*)buf;
        HRSDK_CMDRESULT ret = HiroCamSDK_GetRecordList(handle, year, month, day, pList, 100, timeOut);
        if (HRSDKCMDRST_SUCCESS != ret){
            return ret;
        }

        jclass objClass = env->FindClass("com/fos/sdk/RecordList");
        jfieldID fieldIDtotalCnt = env->GetFieldID(objClass, "totalCnt", "I");
        jfieldID fieldIDrecordInfo = env->GetFieldID(objClass, "recordInfo", "[Ljava/lang/String;");
        env->SetIntField( list , fieldIDtotalCnt , pList->count);

        if (pList->count == 0){
            return ret;
        }

        jclass strclass = env->FindClass("java/lang/String");
        jobjectArray objarry = env->NewObjectArray(pList->count, strclass, NULL);
        HRSDK_RecordListData* pData = (HRSDK_RecordListData*)pList->data;
        for(int i = 0;i < pList->count; i++)
        {
            env->SetObjectArrayElement(objarry, i, env->NewStringUTF(pData[i].file));
        }
        env->SetObjectField(list, fieldIDrecordInfo, objarry);
        return ret;
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_SendTalkData(JNIEnv *env, jobject obj, jint handle, jbyteArray data, jint len) {
        char* arrayBody = (char*)env->GetByteArrayElements( data, 0);
        int ret = HiroCamSDK_SendTalkData(handle, arrayBody, len);
        env->ReleaseByteArrayElements( data, (jbyte*)arrayBody, 0 );
        return ret;
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_StartPlayback(JNIEnv *env, jobject obj, jint handle, jstring filename, jobject recordInfo, jlong timeOutPtr) {
        int *timeOut = (int *) timeOutPtr;
        char* fileName = (char*)env->GetStringUTFChars(filename, 0);

        HRSDK_RecordInfo rcdInfo;
        HRSDK_CMDRESULT ret = HiroCamSDK_StartPlayback(handle, fileName, &rcdInfo, timeOut);
        if (HRSDKCMDRST_SUCCESS != ret){
            return ret;
        }

        jclass objClass = env->FindClass("hiro/com/hirocamsdk/RecordInfo");
        jfieldID fieldiWidth = env->GetFieldID( objClass, "iWidth", "I" );
        jfieldID fieldiHeight = env->GetFieldID( objClass, "iHeight", "I" );
        jfieldID fieldiTotalFrame = env->GetFieldID( objClass, "iTotalFrame", "I" );
        jfieldID fieldiTotalTime = env->GetFieldID( objClass, "iTotalTime", "I" );

        env->SetIntField(recordInfo, fieldiWidth, rcdInfo.iWidth );
        env->SetIntField(recordInfo, fieldiHeight, rcdInfo.iHeight);
        env->SetIntField(recordInfo, fieldiTotalFrame, rcdInfo.iTotalFrame);
        env->SetIntField(recordInfo, fieldiTotalTime, rcdInfo.iTotalTime);
        return ret;
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_StopPlayback(JNIEnv *env, jobject obj, jint handle, jlong timeOutPtr) {
        int *timeOut = (int *) timeOutPtr;
        return HiroCamSDK_StopPlayback(handle, timeOut);
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_StartRecord(JNIEnv *env, jobject obj, jint handle, jint type, jstring filename) {
        return HiroCamSDK_StartRecord(handle, (HRRECORDTYPE)type, (char*)env->GetStringUTFChars(filename, 0));
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_StopRecord(JNIEnv *env, jobject obj, jint handle) {
        return HiroCamSDK_StopRecord(handle);
    }

    /****************************************** Subscirbe/Push ************************************************************************************/
    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_SubScribe(JNIEnv *env, jobject obj, jstring uid, jstring agName, jstring appName, jstring devToken, jint eventChn) {
        char* pUid = (char*)env->GetStringUTFChars(uid, 0);
        char* pAgName = (char*)env->GetStringUTFChars(agName, 0);
        char* pAppName = (char*)env->GetStringUTFChars(appName, 0);
        char* pDevToken = (char*)env->GetStringUTFChars(devToken, 0);
#if 0
        return HiroCamPushSDK_SubScribe(pUid, pAgName, pAppName, pDevToken, eventChn);
#else
        return 0;
#endif
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_UnSubScribe(JNIEnv *env, jobject obj, jstring uid, jstring agName, jstring appName, jstring devToken, jint eventChn) {
        char* pUid = (char*)env->GetStringUTFChars(uid, 0);
        char* pAgName = (char*)env->GetStringUTFChars(agName, 0);
        char* pAppName = (char*)env->GetStringUTFChars(appName, 0);
        char* pDevToken = (char*)env->GetStringUTFChars(devToken, 0);
#if 0
        return HiroCamPushSDK_UnSubScribe(pUid, pAgName, pAppName, pDevToken, eventChn);
#else
        return 0;
#endif
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_ResetBadge(JNIEnv *env, jobject obj, jstring uid, jstring agName, jstring appName, jstring devToken, jint eventChn) {
        char* pUid = (char*)env->GetStringUTFChars(uid, 0);
        char* pAgName = (char*)env->GetStringUTFChars(agName, 0);
        char* pAppName = (char*)env->GetStringUTFChars(appName, 0);
        char* pDevToken = (char*)env->GetStringUTFChars(devToken, 0);
#if 0
        return HiroCamPushSDK_ResetBadge(pUid, pAgName, pAppName, pDevToken, eventChn);
#else
        return 0;
#endif
    }

    /****************************************** keep alive ************************************************************************************/
    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_GetKlResult(JNIEnv *env, jobject obj, jstring uid) {
        char* pUid = (char*)env->GetStringUTFChars(uid, 0);
        return HiroCamKlSDK_GetResult(pUid);
    }

    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_QueryKlSt(JNIEnv *env, jobject obj, jstring uid) {
        char* pUid = (char*)env->GetStringUTFChars(uid, 0);
        return HiroCamKlSDK_Query(pUid);
    }

    /****************************************** wifi config ************************************************************************************/
    JNIEXPORT void JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_StartWifiConfig(JNIEnv *env, jobject obj, jstring ssid, jstring wifiPwd, jstring devPwd) {
        char* pSSid = (char*)env->GetStringUTFChars(ssid, 0);
        char* pWifiPwd = (char*)env->GetStringUTFChars(wifiPwd, 0);
        char* pDevPwd = (char*)env->GetStringUTFChars(devPwd, 0);
        HiroCamSDK_StartConfigWifi(pSSid, pWifiPwd, pDevPwd);
    }

    JNIEXPORT void JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_StopWifiConfig(JNIEnv *env, jobject obj) {
        HiroCamSDK_StopConfigWifi();
    }



    JNIEXPORT jint JNICALL Java_hiro_com_hirocamsdk_HiroCamJNI_UpdateBitmap(JNIEnv *env, jobject obj, jint handle, jobject zBitmap) {
        if (zBitmap == NULL) {
            return -1;
        }

        // Get bitmap info
        AndroidBitmapInfo info;
        memset(&info, 0, sizeof(info));
        AndroidBitmap_getInfo(env, zBitmap, &info);
        // Check format, only RGB565 & RGBA are supported
        if (info.width <= 0 || info.height <= 0 ||
            (info.format != ANDROID_BITMAP_FORMAT_RGB_565 && info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)) {
            env->ThrowNew(env->FindClass("java/io/IOException"), "invalid bitmap");
            return -2;
        }

        HRSDK_DecFrame* pDecFrame = NULL;
        int outLen = 0;
        HRDECFMT videoFmt = HRDECTYPE_RGB24;
        if (info.format == ANDROID_BITMAP_FORMAT_RGB_565){
            videoFmt = HRDECTYPE_RGB565LE;
        }else if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888){
            videoFmt = HRDECTYPE_RGBA32;
        }
        if (HRSDKCMDRST_SUCCESS == HiroCamSDK_GetVideoData(handle, &pDecFrame, &outLen, videoFmt)){
            void * pixels = NULL;
            int res = AndroidBitmap_lockPixels(env, zBitmap, &pixels);
            if (pixels == NULL) {
                env->ThrowNew(env->FindClass("java/io/IOException"), "fail to open bitmap");
                return -3;
            }

            memcpy(pixels, pDecFrame->data, pDecFrame->len);

            AndroidBitmap_unlockPixels(env, zBitmap);
            return 0;
        }
        return -4;
    }
}
