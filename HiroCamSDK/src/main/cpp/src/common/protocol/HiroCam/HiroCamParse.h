/*
* Copyright (c) 2014,foscam 研发体系
* All rights reserved.
*
* 文件名称：H264Work.h
* 文件标识：
* 摘 要：H264工作类
*
* 当前版本：1.0
* 作 者：luoc
* 完成日期：2014年8月7日
*
* 取代版本：
* 原作者 ：
* 完成日期：
*/

#ifndef __HIROCAMSDK_SRC_COMMON_PROTOCOL_HIROCAM_HIROCAMPARSE_H__
#define __HIROCAMSDK_SRC_COMMON_PROTOCOL_HIROCAM_HIROCAMPARSE_H__
#include "BaseDataType.h"
#include "DataProc.h"
#include "HiroCamProtocol.h"
#include "Thread.h"
#include "MsgQue.hpp"
#include "HRTime.h"
#include "HiroCamDef.h"
#include "CStreamBuffer.h"
#include "adpcm.h"
namespace HiroCamSDK {
	namespace Protocol {
        namespace HiroCam {
        
            typedef struct tagSyncReqMsg{
                HR_U32 req;
                HR_U32 message;
                HR_S32 result;
                HR_S32 param;
                bool operator == (struct tagSyncReqMsg msg) const
                {
                    return (req == msg.req && message == msg.message);
                }
            }SyncReqMsg;
            typedef std::vector<SyncReqMsg>  VT_SYNCREQMSG;
            
            typedef struct tagSyncRespMsg{
                HR_U32 req;
                HR_U32 message;
                HR_S8* buf;
                HR_S32 bufSize;
                bool operator == (struct tagSyncRespMsg msg) const
                {
                    return (req == msg.req && message == msg.message);
                }
            }SyncRespMsg;
            typedef std::vector<SyncRespMsg>  VT_SYNCRESPMSG;
            
class CHiroCamParse: public IParse{
public:
	CHiroCamParse(HR_S8* uid, CStreamBuff* pVideoWriter, CStreamBuff* pAudioWrite);
	virtual ~CHiroCamParse();
    void WillBeRelease();
    
    HRSDK_CMDRESULT Login(HR_S8* ip, HR_U16 port, HR_S8* pwd, HR_BOOL bLive, HR_S32& timeout);
    void getLoginRst(HRSDK_Login* loginRst);
    
    HRSDK_CMDRESULT LogOut();
    
    HRSDK_CMDRESULT StartTalk(HR_S32& timeout);
    HRSDK_CMDRESULT StopTalk(HR_S32& timeout);
    
    HRSDK_CMDRESULT ChangePwd(HR_S8* oldPwd, HR_S8* newPwd, HR_S32& timeout);
    
    HRSDK_CMDRESULT GetDevInfo(HRSDK_DevInfo* devInfo, HR_S32& timeout);
    HRSDK_CMDRESULT SetDevInfo(HRSDK_SetDevInfo* devInfo, HR_S32& timeout);
    
    HRSDK_CMDRESULT GetPirInfo(HRSDK_PirInfo* pirInfo, HR_S32& timeout);
    HRSDK_CMDRESULT SetPirInfo(HRSDK_PirInfo* pirInfo, HR_S32& timeout);
    
    HRSDK_CMDRESULT FormatSDCardInfo(HRSDK_SDCardInfo* sdCardInfo, HR_S32& timeout);
    
    HRSDK_CMDRESULT GetRecordMap(HR_S32 year, HR_S32 month, HR_S32* map, HR_S32& timeout);
    HRSDK_CMDRESULT VideoFlip(HR_S32& timeout);
    HRSDK_CMDRESULT VideoMirror(HR_S32& timeout);
    
    HRSDK_CMDRESULT StartPlayback(char* filename, HRSDK_RecordInfo* rcdInfo, HR_S32& timeout);
    HRSDK_CMDRESULT StopPlayback(HR_S32& timeout);

    HRSDK_CMDRESULT GetRecordList(int year, int month, int day, HRSDK_RecordListHead* list, int listBufSize, HR_S32& timeout);
    
    HRSDK_CMDRESULT SendTalkData(char* data, int len);
protected:
    HR_THREADMETHOD_DEC(RcvThread0, lpParam);
    HR_THREADMETHOD_DEC(RcvThread1, lpParam);
    void RcvThreadBody(HR_U32 channel);
    void ConnectRun(HR_U32 channel);

protected:
    HR_VOID FillHead(HiroCamMsgHead* pHead, HR_S16 id, HR_S32 len, HR_S32 seq);
    virtual HR_S32 onParseMsg(ISock *pSock, HR_S8* pHead);
    virtual HR_S32 onCheckData(HR_S8* pHead, HR_U32 len);
    virtual HR_S32 onDataNoComp(HR_S8* pHead, HR_U32 Maxlen, HR_U32* RecvBufLen);
protected:
    HR_S32 WaitMsg(SyncReqMsg& msg, HR_S32& timeout);
    HR_U32 GetSeq();
    HR_BOOL IsLogin();
    HR_S32 OnLogin(HR_S32 timeout);
    HR_S32 OnStartTalk(HR_U32 seq, HR_S32 timeout);
    HR_S32 OnStopTalk(HR_U32 seq, HR_S32 timeout);
    HR_S32 OnChangePwd(HR_S8* oldPwd, HR_S8* newPwd, HR_U32 seq, HR_S32 timeout);
    HR_S32 OnGetDevInfo(HR_U32 seq, HR_S32 timeout);
    HR_S32 OnSetDevInfo(HRSDK_SetDevInfo* devInfo, HR_U32 seq, HR_S32 timeout);
    HR_S32 OnGetPirInfo(HR_U32 seq, HR_S32 timeout);
    HR_S32 OnSetPirInfo(HRSDK_PirInfo* pirInfo, HR_U32 seq, HR_S32 timeout);
    HR_S32 OnFormatSDCardInfo(HR_U32 seq, HR_S32 timeout);
    HR_S32 OnGetRecordMap(HR_U32 seq, HR_S32 year, HR_S32 month, HR_S32 timeout);
    HR_S32 OnVideoFlip(HR_U32 seq, HR_S32 timeout);
    HR_S32 OnVideoMirror(HR_U32 seq, HR_S32 timeout);
    HR_S32 OnGetRecordList(HR_S32 year, HR_S32 month, HR_S32 day, HRSDK_RecordListHead* list, HR_S32 listBufSize, HR_U32 seq, HR_U32 timeout);
    HR_S32 OnStartPlayback(char* filename, HR_U32 seq, int timeout);
    HR_S32 OnStopPlayback(HR_U32 seq, int timeout);
protected:
    CThread         m_hRcvThread0;
    CThread         m_hRcvThread1;
    ISock*          m_pSocket;
    
    HR_S8           m_pPwd[32];
    HR_BOOL         m_bLoginWithLive;
    HR_S32          m_iLoginTimeOut;
    HR_S8           m_pIP[32];
    HR_U16          m_iPort;
    HR_BOOL         m_bConnectSuc;
    HR_U32          m_seq;
    MUTEX           m_seqMutex;
    CStreamBuff*    m_pVideoWriter;
    CStreamBuff*    m_pAudioWriter;
    adpcm_state     m_stPcm;
    adpcm_state     m_stPcmTalk;
    HR_S8           m_bufPcm[1024*5];
    HR_S8           m_talkData[sizeof(FrameHeadInfo) + sizeof(HiroCamMsgHead) + 1024*5];
    
    HiroLoginCmdResp    m_stLoginResp;
    
private:
    CMsgQue<SyncReqMsg, VT_SYNCREQMSG>      m_syncQue;
    CMsgQue<SyncRespMsg, VT_SYNCRESPMSG>    m_syncRespQue;
    MUTEX                                   m_mtxRespQue;
};
        }
	}
}
#endif
