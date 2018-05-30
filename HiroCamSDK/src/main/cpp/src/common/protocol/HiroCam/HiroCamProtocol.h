/*
* Copyright (c) 2018,hiro �з���ϵ
* All rights reserved.
*
* �ļ����ƣ�HiroCamProtocol.h
* �ļ���ʶ��
* ժ Ҫ��
*
* ��ǰ�汾��1.0
* �� �ߣ�luoc
* ������ڣ�
*
* ȡ���汾��
* ԭ���� ��
* ������ڣ�
*/

#ifndef __SRC_NETSERVER_PRPTOCOL_HIROCAM_HIROCAMPROTOCOL_H__
#define __SRC_NETSERVER_PRPTOCOL_HIROCAM_HIROCAMPROTOCOL_H__
namespace HiroCamSDK {
    namespace Protocol {
        namespace HiroCam {
            
typedef unsigned char           		HP_U8;
typedef unsigned short          		HP_U16;
typedef unsigned int            		HP_U32;
typedef unsigned int            		HP_BOOL;
		
typedef char             				HP_S8;
typedef short							HP_S16;
typedef int								HP_S32;
		
typedef unsigned long long				HP_U64;
typedef long long						HP_S64;
		
typedef const unsigned char     		HP_CU8;
typedef const unsigned short   			HP_CU16;
typedef const unsigned int    			HP_CU32;
		
typedef const char						HP_CS8;
typedef const short						HP_CS16;
typedef const int						HP_SC32;

typedef const unsigned long long		HP_CU64;
typedef const long long					HP_CS64;

#define HIROCAMPROT_MAGIC				*(HP_S64*)"*(HIRO)*"
#define HIROCAMPROT_VER					0
/*
ps:
	1. ��ʱ���ù�������Ϣ��login�д����Ƿ����Ƶ
		HRCMD_START_VIDEO
		HRCMD_START_VIDEO_RESP,
		HRCMD_STOP_VIDEO,
		HRCMD_STOP_VIDEO_RESP,	
		HRCMD_START_AUDIO,
		HRCMD_START_AUDIO_RESP,	
		HRCMD_STOP_AUDIO,
		HRCMD_STOP_AUDIO_RESP,
	
	2. ������Ϣ����ֻ����HiroCamMsgHead�м�����ϢID,�޸��ӽṹ��
		HRCMD_GETDEVINFO,
		HRCMD_GETPIRINFO,
		HRCMD_SDCART_FORMAT,
		HRCMD_RECORD_STOP,
		HRCMD_START_SLEEP,
	
	3.������Ϣͳһ��HiroCmdResp��Ӧ
		HRCMD_SETDEVINFO_RESP,
		HRCMD_SETPIRINFO_RESP,
		HRCMD_CHANGEPWD_RESP,
		HRCMD_RECORD_CAMMAND_RESP,
		HRCMD_RECORD_STOP_RESP,
		HRCMD_START_SLEEP_RESP,
	
	4.��������Ϣ�⣬������Ϣ���л�Ӧ
		HRCMD_HEART_BEAT,
		
	5.HiroCamMsgHead��seqԭ������
	
	6.����Ϣ�����б仯��ʱ�����޸�HiroCamMsgHead�еİ汾��Ϣ
		1)��Ϣ��չ��ѭ�̳й�ϵ
		2)��Ϣ��������Ӧ���жϱ���Э��汾��Զ��Э��汾,���Ծ���ʹ��
		��������Ϣ�塣
*/
enum HRCMD
{
	HRCMD_LOGIN					= 0x100,
	HRCMD_LOGIN_RESP,
	
	HRCMD_GETDEVINFO,
	HRCMD_GETDEVINFO_RESP,	
	HRCMD_SETDEVINFO,
	HRCMD_SETDEVINFO_RESP,
	
	HRCMD_GETPIRINFO,
	HRCMD_GETPIRINFO_RESP,	
	HRCMD_SETPIRINFO,
	HRCMD_SETPIRINFO_RESP,
	
	HRCMD_VIDEO_FLIP,
	HRCMD_VIDEO_FLIP_RESP,
	
	HRCMD_VIDEO_MIRROR,
	HRCMD_VIDEO_MIRROR_RESP,
	
	HRCMD_CHANGEPWD,
	HRCMD_CHANGEPWD_RESP,
	
	HRCMD_SDCART_FORMAT,
	HRCMD_SDCART_FORMAT_RESP,
	
	HRCMD_START_SLEEP,
	HRCMD_START_SLEEP_RESP,
	
	
	HRCMD_RECORD_GETMAP			= 0x200,
	HRCMD_RECORD_GETMAP_RESP,
	HRCMD_RECORD_GETLIST,
	HRCMD_RECORD_GETLIST_RESP,
	HRCMD_RECORD_START,
	HRCMD_RECORD_START_RESP,
	HRCMD_RECORD_CAMMAND,
	HRCMD_RECORD_CAMMAND_RESP,
	HRCMD_RECORD_STOP,
	HRCMD_RECORD_STOP_RESP,
	
	
	HRCMD_HEART_BEAT			= 0x300,
	
	
	HRCMD_START_VIDEO			= 0x400,
	HRCMD_START_VIDEO_RESP,
	HRCMD_STOP_VIDEO,
	HRCMD_STOP_VIDEO_RESP,	
	HRCMD_START_AUDIO,
	HRCMD_START_AUDIO_RESP,	
	HRCMD_STOP_AUDIO,
	HRCMD_STOP_AUDIO_RESP,
	
	HRCMD_START_TALK,
	HRCMD_START_TALK_RESP,	
	HRCMD_STOP_TALK,
	HRCMD_STOP_TALK_RESP,
	
	HRCMD_STREAM_LIVE			= 0x500,
	HRCMD_STREAM_PLAYBACK,
	HRCMD_STREAM_TALK,
};
            
typedef struct tagFrameHeadInfo
{
    unsigned long long  seqNo;
    long long           timeStamp;   // in us
    unsigned int        videoLen;
    unsigned int        audioLen;
    unsigned int        isKeyFrame;
    unsigned int        videoWidth;
    unsigned int        videoHeight;
    unsigned int        audio_format;
    unsigned int        frame_type;
    unsigned int        wifiQuality;
    unsigned int        pbSessionNo;
    unsigned char       reserved[52];
    char data[0];
}__attribute__((packed))  FrameHeadInfo;

enum HR_RESULT{
	HRRST_LOGIN_SUCCESS			= 0x00,
	HRRST_LOGIN_USRORPWD_ERR	= -0x01,
	HRRST_LOGIN_EXCEED_MAXUSR	= -0x01,
};

typedef struct
{
	HP_S8 	magic[8];
	HP_U16 	ver;
	HP_U16 	id;
	HP_U32 	seq;
	HP_U32	dataLen;
	HP_S8 	data[0];
}HiroCamMsgHead;

typedef struct{
	HP_S32	result;
}HiroCmdResp;

typedef struct{
	HP_S8	usr[32];
	HR_S8	pwd[32];
	HP_BOOL	bVideo;
	HP_BOOL	bAudio;
}HiroLoginCmd;
typedef struct{
	HP_S32	result;
	HR_S32	eventChannel;
	HP_S32	iBat;
	HP_BOOL	bInCharge;
	HP_S32	iUsrRight;
	HP_S16	bFlip;
	HP_S16	bMirror;
}HiroLoginCmdResp;

typedef struct{
	HP_S32	utcTime;
	HP_S32	timeZone;
	HP_S32	powerFreq;
	HP_S32	recordMode;
	HP_S32	langCode;
	HP_S32	talkVol;
}HiroSetDevInfo;

typedef struct{
	HP_S32	result;
	HP_S8	productName[16];
	HP_S32	verCode;
	HP_S32	powerFreq;
	HP_S32	iBat;
	HP_BOOL	bInCharge;
	HP_S32	timeZone;
	HP_S32	langCode;
	HP_S32	sdTotalMB;
	HP_S32	sdFreeMB;
	HP_S32	sdCanRecordDays;
	HP_S32	recordMode;
	HP_S32	talkVol;
}HiroGetDevInfoResp;

typedef struct{
	HP_S32	result;
	HP_BOOL	bEnable;
	HP_S32	sensitivity;
	HP_S32	weekMap;
	HP_S32	detectTimeSun;
	HP_S32	detectTimeMon;
	HP_S32	detectTimeTue;
	HP_S32	detectTimeWed;
	HP_S32	detectTimeThu;
	HP_S32	detectTimeFri;
	HP_S32	detectTimeSat;
	HP_S32	recordDuration;
}HiroGetPirInfoResp;

typedef struct{
	HP_BOOL	bEnable;
	HP_S32	sensitivity;
	HP_S32	weekMap;
	HP_S32	detectTimeSun;
	HP_S32	detectTimeMon;
	HP_S32	detectTimeTue;
	HP_S32	detectTimeWed;
	HP_S32	detectTimeThu;
	HP_S32	detectTimeFri;
	HP_S32	detectTimeSat;
	HP_S32	recordDuration;
}HiroSetPirInfo;

typedef struct{
	HP_S8	oldPwd[32];
	HP_S8	newPwd[32];
}HiroChangePwd;

typedef struct{
	HP_S32	result;
	HP_S32	sdTotalMB;
	HP_S32	sdFreeMB;
	HP_S32	sdCanRecordDays;
}HiroSDCardFormatResp;

typedef struct{
	HP_S32	beatValue;
}HiroHeadBeat;

typedef struct{
	HP_S32	year;
	HP_S32	month;
}HiroGetRecordMap;
typedef struct{
	HP_S32	result;
	HP_S32	monthMap;
}HiroGetRecordMapResp;

typedef struct{
	HP_S32	year;
	HP_S32	month;
	HP_S32	day;
}HiroGetRecordList;
typedef struct{
	HP_S32	result;
	HP_S32	count;
	HP_S8	data[0];
}HiroGetRecordListResp;
typedef struct{
	HP_S8	file[24];
}HiroGetRecordListData;

typedef struct{
	HP_S8	file[24];
}HiroRecordStart;
typedef struct{
	HP_S32	result;
	HP_S32	iWidth;
	HP_S32	iHeight;
	HP_S32	iTotalFrame;
	HP_S32	iTotalTime;
}HiroRecordStartResp;
typedef struct{
	HP_S32	result;
}HiroRecordCmdResp;

typedef struct{
	HP_S32	command;	//1:pause 2:resum
}HiroRecordCommand;

        }
    }
}
#endif
