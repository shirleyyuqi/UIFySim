/*
* Copyright (c) 2018, hiro
* All rights reserved.
*
* File Name: HiroCamDef.h
* Abstract: HiroCamSDK Definition File
*
* Current Version: 1.0
* Author: luoc
*
*/

#ifndef __HIROCAMSDK_SRC_INCLUDE_HIROCAMDEF__
#define __HIROCAMSDK_SRC_INCLUDE_HIROCAMDEF__
#include <string.h>

#ifdef DISABLE_4200
#pragma warning (disable : 4200)	//disable zero byte data[0] warning.
#endif


typedef enum {
    HRKLSDKRST_SUCCESS  =   0,
    HRKLSDKRST_QUERIING =   -1,
    HRKLSDKRST_SERVER_NOTRESPONSE   = -2,
    HRKLSDKRST_FAIL_UNKOWN          = -100,
}HRKEEPALIVESDK_RESULT;

typedef enum{
    HRSDKCMDRST_SUCCESS,
    HRSDKCMDRST_HNDLE_ERR,
    HRSDKCMDRST_TIMEOUT,
    HRSDKCMDRST_TIMESYNC_ERR,
    HRSDKCMDRST_USRORPWD_ERR,
    HRSDKCMDRST_ARG_ERR,
    HRSDKCMDRST_ACCESSDENY,
    HRSDKCMDRST_SYSBUSY,
    HRSDKCMDRST_UNSUPPORT,
    HRSDKCMDRST_ACHIEVE_FILE_MAXSIZE,
    HRSDKCMDRST_ACHIEVE_FILE_CHANGE_RES,
    HRSDKCMDRST_ERR_UNKNOW,
}HRSDK_CMDRESULT;

typedef enum  
{
	HRDECTYPE_VIDEORAW,

	HRDECTYPE_ARGB32,	  //packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
	HRDECTYPE_RGBA32,    //packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
	HRDECTYPE_ABGR32,    //packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
	HRDECTYPE_BGRA32,    //packed BGRA 8:8:8:8, 32bpp, BGRABGRA...
	HRDECTYPE_RGB24,	  //packed RGB 8:8:8, 24bpp, RGBRGB...
	HRDECTYPE_BGR24,     //packed RGB 8:8:8, 24bpp, BGRBGR...
	HRDECTYPE_RGB565BE,  //packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
	HRDECTYPE_RGB565LE,  //packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
	HRDECTYPE_BGR565BE,  //packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
	HRDECTYPE_BGR565LE,  //packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
	
	HRDECTYPE_YUV420,
	HRDECTYPE_YUYV422,
	HRDECTYPE_UYVY422,
	HRDECTYPE_H264,
	HRDECTYPE_MJPEG,
	HRDECTYPE_MJPEG_BASE64,
    HRDECTYPE_H264_BASE64,
	
	HRDECTYPE_AUDIORAW,
	HRDECTYPE_G726,
	HRDECTYPE_G711U,
	HRDECTYPE_PCM,
	HRDECTYPE_ADPCM,
	HRDECTYPE_G711A,
	HRDECTYPE_AAC,
}HRDECFMT;
typedef enum{
    HRRCTYPE_MP4,
    HRRCTYPE_PS,
    HRRCTYPE_AVI,
}HRRECORDTYPE;
typedef enum {
    HRSDKMT_VIDEO,
    HRSDKMT_AUDIO,
}HRSDK_MEDIA;
typedef struct {
    int type;
    int w;
    int h;
    int len;
    int isKey;
    unsigned long long pts;
    char data[0];
}HRSDK_DecFrame;

typedef struct{
    int    devVer;
    int    eventChannel;
    int    iBat;
    int    bInCharge;
    int    iUsrRight;
    int    bFlip;
    int    bMirror;
}HRSDK_Login;

typedef struct{
    char    productName[16];
    int     verCode;
    int     powerFreq;
    int     iBat;
    int     bInCharge;
    int     timeZone;
    int     langCode;
    int     sdTotalMB;
    int     sdFreeMB;
    int     sdCanRecordDays;
    int     recordMode;
    int     talkVol;
}HRSDK_DevInfo;

typedef struct{
    int    bEnable;
    int    sensitivity;
    int    weekMap;
    int    detectTimeSun;
    int    detectTimeMon;
    int    detectTimeTue;
    int    detectTimeWed;
    int    detectTimeThu;
    int    detectTimeFri;
    int    detectTimeSat;
    int    recordDuration;
}HRSDK_PirInfo;

typedef struct{
    int    sdTotalMB;
    int    sdFreeMB;
    int    sdCanRecordDays;
}HRSDK_SDCardInfo;

typedef struct{
    int    utcTime;
    int    timeZone;
    int    powerFreq;
    int    recordMode;
    int    langCode;
    int    talkVol;
}HRSDK_SetDevInfo;

typedef struct{
    int    result;
    int    count;
    char    data[0];
}HRSDK_RecordListHead;
typedef struct{
    char    file[24];
}HRSDK_RecordListData;

typedef struct{
    int    iWidth;
    int    iHeight;
    int    iTotalFrame;
    int    iTotalTime;
}HRSDK_RecordInfo;
#endif
