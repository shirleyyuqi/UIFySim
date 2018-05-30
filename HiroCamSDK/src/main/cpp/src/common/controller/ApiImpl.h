/*
* Copyright (c) 2014,foscam �з���ϵ
* All rights reserved
*
* �ļ����ƣ�ApiManager.h
* �ļ���ʶ��
* ժ Ҫ��
*
* ��ǰ�汾��1.0
* �� �ߣ�luoc
* ������ڣ�2014��7��23��
*
* ȡ���汾��
* ԭ���� ��
* ������ڣ�
*/
#ifndef __HIROCAMSDK_SRC_CONTROLLER_APIMANAGER__
#define __HIROCAMSDK_SRC_CONTROLLER_APIMANAGER__
#include "BaseDataType.h"
#include "BaseFunc.h"
#include "HiroCamDef.h"
#include "CStreamBuffer.h"
#include "AudioCodec.h"
#include "DecoderManager.h"
#include "g726.h"
#include "Record.h"

#include "../protocol/HiroCam/HiroCamParse.h"

namespace HiroCamSDK
{
	typedef enum{
		LIVEVIEW_PLAY_MODE,
		PLAYBACK_PLAY_MODE,
	}MEDIA_PLAY_MODE;

	class CHandleManager;
 	class CApiImpl
 	{
		friend class CHandleManager;
	public:
		CApiImpl();
		virtual ~CApiImpl();
	protected:
		HR_U32 Ref();
		HR_U32 UnRef();
		HR_U32 GetRef();
        HR_U32 ARef();
        HR_U32 UnARef();
        HR_U32 GetARef();
        void Release();
        void WillBeRelease();
	public:
        void SetUsrInfo(char* uid);
        HRSDK_CMDRESULT Login(HR_S8* usr, HR_S8* pwd, HR_S32 bLive, HRSDK_Login* loginRst, HR_S32& timeout);
        HRSDK_CMDRESULT LogOut();
        
        HRSDK_CMDRESULT StartTalk(HR_S32 timeout);
        HRSDK_CMDRESULT StopTalk(HR_S32 timeout);
        
        HRSDK_CMDRESULT ChangePwd(char* oldPwd, char* newPwd, HR_S32 timeout);
        
        HRSDK_CMDRESULT GetDevInfo(HRSDK_DevInfo* devInfo, HR_S32& timeout);
        HRSDK_CMDRESULT SetDevInfo(HRSDK_SetDevInfo& devInfo, HR_S32 timeout);
        
        HRSDK_CMDRESULT GetPirInfo(HRSDK_PirInfo* pirInfo, HR_S32& timeout);
        HRSDK_CMDRESULT SetPirInfo(HRSDK_PirInfo& pirInfo, HR_S32 timeout);
        
        HRSDK_CMDRESULT FormatSDCardInfo(HRSDK_SDCardInfo* sdCardInfo, HR_S32 timeout);
        HRSDK_CMDRESULT GetRecordMap(int year, int month, int* map, HR_S32 timeout);
        HRSDK_CMDRESULT VideoFlip(int timeout);
        HRSDK_CMDRESULT VideoMirror(int timeout);
        
        HRSDK_CMDRESULT GetRecordList(int year, int month, int day, HRSDK_RecordListHead* list, int listBufSize, HR_S32 timeout);
        
        HRSDK_CMDRESULT SendTalkData(char* data, int len);
        HRSDK_CMDRESULT GetVideoData(HRSDK_DecFrame **data, int *outLen, HRDECFMT videoFmt);
        HRSDK_CMDRESULT GetAudioData(HRSDK_DecFrame **data, int *outLen);
        
        HRSDK_CMDRESULT StartPlayback(char* filename, HRSDK_RecordInfo* rcdInfo, int *timeout);
        HRSDK_CMDRESULT StopPlayback(int* timeout);
        
        HRSDK_CMDRESULT StartRecord(HRRECORDTYPE type,const HR_S8 *filename);
        HRSDK_CMDRESULT StopRecord();
        static HR_S32 RecordErrorCallBack(HRSDK_CMDRESULT errorcode, HR_VOID *pContext);
	private:
		HR_U32			m_iRef;
        HR_U32          m_iARef;
        HR_S8           m_pUid[64];
        
        CStreamBuff        *m_pVideoReader;
        CStreamBuff        *m_pVideoWriter;
        CStreamBuff        *m_pAudioReader;
        CStreamBuff        *m_pAudioWriter;
        Protocol::HiroCam::CHiroCamParse*       m_pParse;
        
        HRSDK_DecFrame*     m_pDecData;
        HRSDK_DecFrame*     m_pAudioData;
        MUTEX               m_mutexVDec;
        DecoderManager      m_VideoDecManager;

#ifdef _USE_RECORD
        CRecord m_record;
#endif
        
    };
}

#endif
