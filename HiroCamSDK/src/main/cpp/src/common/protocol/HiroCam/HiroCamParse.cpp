#include "HiroCamParse.h"
#include "SYSock.h"
#include "adpcm.h"
namespace HiroCamSDK { namespace Protocol { namespace HiroCam {
    
    CHiroCamParse::CHiroCamParse(HR_S8* uid, CStreamBuff* pVideoWriter, CStreamBuff* pAudioWrite)
    : m_pVideoWriter(pVideoWriter)
    , m_pAudioWriter(pAudioWrite)
    {
        memset(m_pPwd, 0x00, sizeof(m_pPwd));
        memset(m_pIP, 0x00, sizeof(m_pIP));
        m_bLoginWithLive = HR_FALSE;
        m_bConnectSuc = HR_FALSE;
        m_pSocket = new HiroCamSDK::SYSocket(uid);
        INIT_MUTEX(&m_seqMutex);
        INIT_MUTEX(&m_mtxRespQue);
        m_seq = 0;
        m_stLoginResp.result = -1;
        
        memset(m_talkData, 0x00, sizeof(m_talkData));
        memset(&m_stPcmTalk, 0x00, sizeof(m_stPcmTalk));
        memset(&m_stPcm, 0x00, sizeof(m_stPcm));
    }
    
    CHiroCamParse::~CHiroCamParse()
    {
        m_pSocket->UserQuit();
        m_pSocket->CloseSocket();
        m_hRcvThread0.ExitThead(0, 0);
        m_hRcvThread1.ExitThead(0, 0);
        delete m_pSocket;
        RELEASE_MUTEX(&m_seqMutex);
        RELEASE_MUTEX(&m_mtxRespQue);
    }
    
    void CHiroCamParse::WillBeRelease()
    {
        m_pSocket->UserQuit();
    }
    
    HRSDK_CMDRESULT CHiroCamParse::LogOut()
    {
        m_pSocket->UserQuit();
        m_pSocket->CloseSocket();
        m_hRcvThread0.ExitThead(0, 0);
        m_hRcvThread1.ExitThead(0, 0);
        return HRSDKCMDRST_SUCCESS;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::Login(HR_S8* ip, HR_U16 port, HR_S8* pwd, HR_BOOL bLive, HR_S32& timeout)
    {
        if (pwd) strcpy(m_pPwd, pwd);
        if (ip) strcpy(m_pIP, ip);
        m_iPort = port;
        m_bLoginWithLive = bLive;
        m_iLoginTimeOut = timeout;
        
        if (m_hRcvThread0.IsThreadRun() || m_hRcvThread1.IsThreadRun()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        m_pSocket->ResetQuit();
        m_hRcvThread0.CreateThread(1, 1, 512*1024, RcvThread0, this);
        m_hRcvThread1.CreateThread(1, 1, 512*1024, RcvThread1, this);
        
        SyncReqMsg msg = {0, HRCMD_LOGIN, 0};
        if ( WaitMsg(msg, timeout) ){
            if (msg.result == HRRST_LOGIN_SUCCESS) {
                return HRSDKCMDRST_SUCCESS;
            }else if (msg.result == HRRST_LOGIN_USRORPWD_ERR){
                return HRSDKCMDRST_USRORPWD_ERR;
            }
            return HRSDKCMDRST_ERR_UNKNOW;
        }
        return HRSDKCMDRST_TIMEOUT;
    }
    
    void CHiroCamParse::getLoginRst(HRSDK_Login* loginRst)
    {
        loginRst->bFlip = m_stLoginResp.bFlip;
        loginRst->iBat = m_stLoginResp.iBat;
        loginRst->bInCharge = m_stLoginResp.bInCharge;
        loginRst->bMirror = m_stLoginResp.bMirror;
        loginRst->eventChannel = m_stLoginResp.eventChannel;
        loginRst->iUsrRight = 0;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::StartTalk(HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_START_TALK, 0};
        if (OnStartTalk(seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                
                return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
            }
            return HRSDKCMDRST_TIMEOUT;
        }
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::StopTalk(HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_STOP_TALK, 0};
        
        if (OnStopTalk(seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
            }
            return HRSDKCMDRST_TIMEOUT;
        }
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::ChangePwd(HR_S8* oldPwd, HR_S8* newPwd, HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_CHANGEPWD, 0};
        if (OnChangePwd(oldPwd, newPwd, seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                if (msg.result == 0) {
                    return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
                }
                return HRSDKCMDRST_ERR_UNKNOW;
            }
            return HRSDKCMDRST_TIMEOUT;
        }
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::GetDevInfo(HRSDK_DevInfo* devInfo, HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_GETDEVINFO, 0};
        SyncRespMsg msgResp = {seq, HRCMD_GETDEVINFO_RESP, (HR_S8*)devInfo, sizeof(HRSDK_DevInfo)};
        m_syncRespQue.PostMsg(msgResp);
        if (OnGetDevInfo(seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                if (msg.result == 0) {
                    return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
                }
                return HRSDKCMDRST_ERR_UNKNOW;
            }
            LOCK_MUTEX(&m_mtxRespQue);
            m_syncRespQue.PeekEqMsg(msgResp);
            UNLOCK_MUTEX(&m_mtxRespQue);
            return HRSDKCMDRST_TIMEOUT;
        }
        LOCK_MUTEX(&m_mtxRespQue);
        m_syncRespQue.PeekEqMsg(msgResp);
        UNLOCK_MUTEX(&m_mtxRespQue);
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::SetDevInfo(HRSDK_SetDevInfo* devInfo, HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_SETDEVINFO, 0};
        if (OnSetDevInfo(devInfo, seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
            }
            return HRSDKCMDRST_TIMEOUT;
        }
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::GetPirInfo(HRSDK_PirInfo* pirInfo, HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_GETPIRINFO, 0};
        SyncRespMsg msgResp = {seq, HRCMD_GETPIRINFO_RESP, (HR_S8*)pirInfo, sizeof(HRSDK_PirInfo)};
        m_syncRespQue.PostMsg(msgResp);
        if (OnGetPirInfo(seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
            }
            LOCK_MUTEX(&m_mtxRespQue);
            m_syncRespQue.PeekEqMsg(msgResp);
            UNLOCK_MUTEX(&m_mtxRespQue);
            return HRSDKCMDRST_TIMEOUT;
        }
        LOCK_MUTEX(&m_mtxRespQue);
        m_syncRespQue.PeekEqMsg(msgResp);
        UNLOCK_MUTEX(&m_mtxRespQue);
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::SetPirInfo(HRSDK_PirInfo* pirInfo, HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_SETPIRINFO, 0};
        if (OnSetPirInfo(pirInfo, seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                return HRSDKCMDRST_SUCCESS;
            }
            return HRSDKCMDRST_TIMEOUT;
        }
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::FormatSDCardInfo(HRSDK_SDCardInfo* sdCardInfo, HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_SDCART_FORMAT, 0};
        SyncRespMsg msgResp = {seq, HRCMD_SDCART_FORMAT_RESP, (HR_S8*)sdCardInfo, sizeof(HRSDK_SDCardInfo)};
        m_syncRespQue.PostMsg(msgResp);
        if (OnFormatSDCardInfo(seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
            }
            LOCK_MUTEX(&m_mtxRespQue);
            m_syncRespQue.PeekEqMsg(msgResp);
            UNLOCK_MUTEX(&m_mtxRespQue);
            return HRSDKCMDRST_TIMEOUT;
        }
        LOCK_MUTEX(&m_mtxRespQue);
        m_syncRespQue.PeekEqMsg(msgResp);
        UNLOCK_MUTEX(&m_mtxRespQue);
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::GetRecordMap(HR_S32 year, HR_S32 month, HR_S32* map, HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_RECORD_GETMAP, 0};
        if (OnGetRecordMap(seq, year, month, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                *map = msg.param;
                return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
            }
            return HRSDKCMDRST_TIMEOUT;
        }
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::VideoFlip(HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_VIDEO_FLIP, 0};
        if (OnVideoFlip(seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
            }
            return HRSDKCMDRST_TIMEOUT;
        }
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::VideoMirror(HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_VIDEO_MIRROR, 0};
        if (OnVideoMirror(seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
            }
            return HRSDKCMDRST_TIMEOUT;
        }
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::GetRecordList(int year, int month, int day, HRSDK_RecordListHead* list, int listBufSize, HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_RECORD_GETLIST, 0};
        SyncRespMsg msgResp = {seq, HRCMD_RECORD_GETLIST_RESP, (HR_S8*)list, listBufSize};
        m_syncRespQue.PostMsg(msgResp);
        
        if (OnGetRecordList(year, month, day, list, listBufSize, seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
            }
            LOCK_MUTEX(&m_mtxRespQue);
            m_syncRespQue.PeekEqMsg(msgResp);
            UNLOCK_MUTEX(&m_mtxRespQue);
            return HRSDKCMDRST_TIMEOUT;
        }
        LOCK_MUTEX(&m_mtxRespQue);
        m_syncRespQue.PeekEqMsg(msgResp);
        UNLOCK_MUTEX(&m_mtxRespQue);
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::StartPlayback(char* filename, HRSDK_RecordInfo* rcdInfo, HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_RECORD_START, 0};
        SyncRespMsg msgResp = {seq, HRCMD_RECORD_START_RESP, (HR_S8*)rcdInfo, sizeof(HRSDK_RecordInfo)};
        m_syncRespQue.PostMsg(msgResp);
        
        if (OnStartPlayback(filename, seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
            }
            LOCK_MUTEX(&m_mtxRespQue);
            m_syncRespQue.PeekEqMsg(msgResp);
            UNLOCK_MUTEX(&m_mtxRespQue);
            return HRSDKCMDRST_TIMEOUT;
        }
        LOCK_MUTEX(&m_mtxRespQue);
        m_syncRespQue.PeekEqMsg(msgResp);
        UNLOCK_MUTEX(&m_mtxRespQue);
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HRSDK_CMDRESULT CHiroCamParse::StopPlayback(HR_S32& timeout)
    {
        if (!IsLogin()) {
            return HRSDKCMDRST_TIMESYNC_ERR;
        }
        
        HR_U32 seq = GetSeq();
        SyncReqMsg msg = {seq, HRCMD_RECORD_STOP, 0};
        if (OnStopPlayback(seq, timeout)) {
            if ( WaitMsg(msg, timeout) ){
                return (msg.result == 0)?HRSDKCMDRST_SUCCESS:HRSDKCMDRST_ERR_UNKNOW;
            }
            return HRSDKCMDRST_TIMEOUT;
        }
        return HRSDKCMDRST_ERR_UNKNOW;
    }
    
    HR_S32 CHiroCamParse::WaitMsg(SyncReqMsg& msg, HR_S32& timeout){
        HR_U64 curTm = CTime::GetUTCTime_Ms();
        HR_BOOL bFind = HR_FALSE;
        /*
        HRLOG_I("tm : %d\n", (HR_S32)(CTime::GetUTCTime_Ms() - curTm));
        HRLOG_I("while? : %d\n", (m_hRcvThread0.IsThreadRun()
                                  && m_hRcvThread1.IsThreadRun()) &&
                ((HR_S32)(CTime::GetUTCTime_Ms() - curTm) < timeout));
        */
        while ((m_hRcvThread0.IsThreadRun()
               && m_hRcvThread1.IsThreadRun()) &&
               ((HR_S32)(CTime::GetUTCTime_Ms() - curTm) < timeout)) {
            if (m_syncQue.PeekEqMsg(msg)) {
                bFind = HR_TRUE;
                break;
            }else{
                usleep(10*1000);
            }
        }
        
        return bFind;
    }
    
    HR_THREADMETHOD(CHiroCamParse::RcvThread0, lpParam)
    {
        CHiroCamParse* pThis = (CHiroCamParse*)lpParam;
        pThis->ConnectRun(0);
        THREAD_RETURN(0);
    }
    
    HR_THREADMETHOD(CHiroCamParse::RcvThread1, lpParam)
    {
        CHiroCamParse* pThis = (CHiroCamParse*)lpParam;
        pThis->RcvThreadBody(1);
        THREAD_RETURN(0);
    }
    
    void CHiroCamParse::ConnectRun(HR_U32 channel)
    {
        m_pSocket->Init();
        m_pSocket->CreateSocket();
        HR_S32 ret = m_pSocket->Connect(m_pIP, m_iPort, NULL, m_iLoginTimeOut);
        if (ret == _SOCKET_SUCCESS) {
            OnLogin(m_iLoginTimeOut);
            m_bConnectSuc = HR_TRUE;
            RcvThreadBody(channel);
        }
    }
    
    void CHiroCamParse::RcvThreadBody(HR_U32 channel)
    {
        while ( !m_pSocket->IsQuit() && !m_bConnectSuc ) {
            usleep(10*1000);
        }
        
        while( !m_pSocket->IsQuit() )
        {
            switch(HRDataProc_Recv(this, m_pSocket, channel))
            {
                case _SOCKET_NODATA:
                    break;
                case _SOCKET_ERROR:
                {
                    m_pSocket->UserQuit();
                    break;
                }
            }
        }
    }
    
    HR_U32 CHiroCamParse::GetSeq()
    {
        CAutoLock lock(m_seqMutex);
        return ++m_seq;
    }
    
    HR_VOID CHiroCamParse::FillHead(HiroCamMsgHead* pHead, HR_S16 id, HR_S32 len, HR_S32 seq)
    {
        *(HP_S64*)pHead->magic = HIROCAMPROT_MAGIC;
        pHead->ver = HIROCAMPROT_VER;
        pHead->id = id;
        pHead->seq = seq;
        pHead->dataLen = len;
    }
    
    HR_BOOL CHiroCamParse::IsLogin()
    {
        if (m_stLoginResp.result == 0) {
            return HR_TRUE;
        }
        return HR_FALSE;
    }
    
    HR_S32 CHiroCamParse::OnLogin(HR_S32 timeout)
    {
        HR_S8 buf[sizeof(HiroLoginCmd) + sizeof(HiroCamMsgHead)];
        HiroCamMsgHead* pHead = (HiroCamMsgHead*)buf;
        HiroLoginCmd* pLogin = (HiroLoginCmd*)pHead->data;
        strcpy(pLogin->pwd, m_pPwd);
        pLogin->bVideo = m_bLoginWithLive;
        pLogin->bAudio = m_bLoginWithLive;
        
        FillHead(pHead, HRCMD_LOGIN, sizeof(HiroLoginCmd), 0);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnChangePwd(HR_S8* oldPwd, HR_S8* newPwd, HR_U32 seq, HR_S32 timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead) + sizeof(HiroChangePwd)];
        HiroCamMsgHead* pHead = (HiroCamMsgHead*)buf;
        HiroChangePwd* pPwd = (HiroChangePwd*)pHead->data;
        strcpy(pPwd->oldPwd, oldPwd);
        strcpy(pPwd->newPwd, newPwd);
        FillHead(pHead, HRCMD_CHANGEPWD, sizeof(HiroChangePwd), seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnGetDevInfo(HR_U32 seq, HR_S32 timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead)];
        FillHead((HiroCamMsgHead*)buf, HRCMD_GETDEVINFO, 0, seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnStartTalk(HR_U32 seq, HR_S32 timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead)];
        FillHead((HiroCamMsgHead*)buf, HRCMD_START_TALK, 0, seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnStopTalk(HR_U32 seq, HR_S32 timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead)];
        FillHead((HiroCamMsgHead*)buf, HRCMD_STOP_TALK, 0, seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnSetDevInfo(HRSDK_SetDevInfo* devInfo, HR_U32 seq, HR_S32 timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead) + sizeof(HiroSetDevInfo)];
        HiroSetDevInfo* pDevInfo = (HiroSetDevInfo*)((HiroCamMsgHead*)buf)->data;
        pDevInfo->utcTime = devInfo->utcTime;
        pDevInfo->timeZone = devInfo->timeZone;
        pDevInfo->powerFreq = devInfo->powerFreq;
        pDevInfo->recordMode = devInfo->recordMode;
        pDevInfo->langCode = devInfo->langCode;
        pDevInfo->talkVol = devInfo->talkVol;
        FillHead((HiroCamMsgHead*)buf, HRCMD_SETDEVINFO, sizeof(HiroSetDevInfo), seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnGetPirInfo(HR_U32 seq, HR_S32 timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead)];
        FillHead((HiroCamMsgHead*)buf, HRCMD_GETPIRINFO, 0, seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnSetPirInfo(HRSDK_PirInfo* pirInfo, HR_U32 seq, HR_S32 timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead) + sizeof(HiroSetPirInfo)];
        HiroSetPirInfo* pPirInfo = (HiroSetPirInfo*)((HiroCamMsgHead*)buf)->data;
        pPirInfo->bEnable = pirInfo->bEnable;
        pPirInfo->sensitivity = pirInfo->sensitivity;
        pPirInfo->weekMap = pirInfo->weekMap;
        pPirInfo->detectTimeSun = pirInfo->detectTimeSun;
        pPirInfo->detectTimeMon = pirInfo->detectTimeMon;
        pPirInfo->detectTimeTue = pirInfo->detectTimeTue;
        pPirInfo->detectTimeWed = pirInfo->detectTimeWed;
        pPirInfo->detectTimeThu = pirInfo->detectTimeThu;
        pPirInfo->detectTimeFri = pirInfo->detectTimeFri;
        pPirInfo->detectTimeSat = pirInfo->detectTimeSat;
        pPirInfo->recordDuration = pirInfo->recordDuration;
        FillHead((HiroCamMsgHead*)buf, HRCMD_SETPIRINFO, sizeof(HiroSetPirInfo), seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnGetRecordList(HR_S32 year, HR_S32 month, HR_S32 day, HRSDK_RecordListHead* list, HR_S32 listBufSize, HR_U32 seq, HR_U32 timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead) + sizeof(HiroGetRecordList)];
        HiroGetRecordList* pRcdList = (HiroGetRecordList*)((HiroCamMsgHead*)buf)->data;
        pRcdList->month = month;
        pRcdList->year = year;
        pRcdList->day = day;
        FillHead((HiroCamMsgHead*)buf, HRCMD_RECORD_GETLIST, sizeof(HiroGetRecordList), seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnFormatSDCardInfo(HR_U32 seq, HR_S32 timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead)];
        FillHead((HiroCamMsgHead*)buf, HRCMD_SDCART_FORMAT, 0, seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnGetRecordMap(HR_U32 seq, HR_S32 year, HR_S32 month, HR_S32 timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead) + sizeof(HiroGetRecordMap)];
        HiroGetRecordMap* pMap = (HiroGetRecordMap*)((HiroCamMsgHead*)buf)->data;
        pMap->month = month;
        pMap->year = year;
        FillHead((HiroCamMsgHead*)buf, HRCMD_RECORD_GETMAP, sizeof(HiroGetRecordMap), seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnVideoFlip(HR_U32 seq, HR_S32 timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead)];
        FillHead((HiroCamMsgHead*)buf, HRCMD_VIDEO_FLIP, 0, seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnVideoMirror(HR_U32 seq, HR_S32 timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead)];
        FillHead((HiroCamMsgHead*)buf, HRCMD_VIDEO_MIRROR, 0, seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HRSDK_CMDRESULT CHiroCamParse::SendTalkData(char* data, int len)
    {
        HiroCamMsgHead* pHead = (HiroCamMsgHead*)m_talkData;
        FrameHeadInfo* pFrame = (FrameHeadInfo*)pHead->data;
        
#if 1
        pFrame->audio_format = 0x11; // ADPCM
        adpcm_coder((short*)data, pFrame->data, len/2, &m_stPcmTalk);
        pFrame->audioLen = len / 4;
#else
        pFrame->audio_format = 0;
        pFrame->audioLen = len;
        memcpy(pFrame->data, data, len);
#endif
        FillHead(pHead, HRCMD_STREAM_TALK, pFrame->audioLen + sizeof(FrameHeadInfo), 0);
        m_pSocket->Send(m_talkData, pHead->dataLen + sizeof(HiroCamMsgHead), 1, 1000);
        return HRSDKCMDRST_SUCCESS;
    }
    
    HR_S32 CHiroCamParse::OnStartPlayback(char* filename, HR_U32 seq, int timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead) + sizeof(HiroRecordStart)];
        HiroRecordStart* pRecord = (HiroRecordStart*)((HiroCamMsgHead*)buf)->data;
        strncpy(pRecord->file, filename, 23);
        pRecord->file[23] = 0;
        FillHead((HiroCamMsgHead*)buf, HRCMD_RECORD_START, sizeof(HiroRecordStart), seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::OnStopPlayback(HR_U32 seq, int timeout)
    {
        HR_S8 buf[sizeof(HiroCamMsgHead)];
        FillHead((HiroCamMsgHead*)buf, HRCMD_RECORD_STOP, sizeof(HiroRecordStart), seq);
        return m_pSocket->Send(buf, sizeof(buf), 0, timeout);
    }
    
    HR_S32 CHiroCamParse::onParseMsg(ISock *pSock, HR_S8* head)
    {
        HiroCamMsgHead *pHead = (HiroCamMsgHead*)head;
        switch(pHead->id){
            case HRCMD_LOGIN_RESP:
            {
                if (pHead->ver >= HIROCAMPROT_VER) {
                    memcpy(&m_stLoginResp, pHead->data, sizeof(HiroLoginCmdResp));
                }else{
                    memcpy(&m_stLoginResp, pHead->data, pHead->dataLen);
                }
                SyncReqMsg msg = { pHead->seq, HRCMD_LOGIN, m_stLoginResp.result };
                m_syncQue.PostMsg(msg);
                break;
            }
            case HRCMD_GETDEVINFO_RESP:
            {
                HiroGetDevInfoResp  stDevInfo = {0};
                if (pHead->ver >= HIROCAMPROT_VER) {
                    memcpy(&stDevInfo, pHead->data, sizeof(stDevInfo));
                }else{
                    memcpy(&stDevInfo, pHead->data, pHead->dataLen);
                }
                
                SyncRespMsg msgResp = {pHead->seq, HRCMD_GETDEVINFO_RESP};
                LOCK_MUTEX(&m_mtxRespQue);
                if ( m_syncRespQue.PeekEqMsg(msgResp) ){
                    HRSDK_DevInfo* devInfo = (HRSDK_DevInfo*)msgResp.buf;
                    
                    devInfo->bInCharge = stDevInfo.bInCharge;
                    devInfo->iBat = stDevInfo.iBat;
                    devInfo->langCode = stDevInfo.langCode;
                    devInfo->powerFreq = stDevInfo.powerFreq;
                    devInfo->recordMode = stDevInfo.recordMode;
                    strncpy(devInfo->productName, stDevInfo.productName, 15);
                    devInfo->productName[15] = 0;
                    devInfo->sdCanRecordDays = stDevInfo.sdCanRecordDays;
                    devInfo->sdFreeMB = stDevInfo.sdFreeMB;
                    devInfo->sdTotalMB = stDevInfo.sdTotalMB;
                    devInfo->talkVol = stDevInfo.talkVol;
                    devInfo->timeZone = stDevInfo.timeZone;
                    devInfo->verCode = stDevInfo.verCode;
                    
                    SyncReqMsg msg = { pHead->seq, HRCMD_GETDEVINFO, stDevInfo.result };
                    m_syncQue.PostMsg(msg);
                }
                UNLOCK_MUTEX(&m_mtxRespQue);
                break;
            }
            case HRCMD_SETDEVINFO_RESP:
            {
                SyncReqMsg msg = { pHead->seq, HRCMD_SETDEVINFO,  ((HiroCmdResp*)pHead->data)->result};
                m_syncQue.PostMsg(msg);
                break;
            }
            case HRCMD_GETPIRINFO_RESP:
            {
                HiroGetPirInfoResp stPirInfo = {0};
                if (pHead->ver >= HIROCAMPROT_VER) {
                    memcpy(&stPirInfo, pHead->data, sizeof(stPirInfo));
                }else{
                    memcpy(&stPirInfo, pHead->data, pHead->dataLen);
                }
                
                SyncRespMsg msgResp = {pHead->seq, HRCMD_GETPIRINFO_RESP};
                LOCK_MUTEX(&m_mtxRespQue);
                if ( m_syncRespQue.PeekEqMsg(msgResp) ){
                    HRSDK_PirInfo* pirInfo = (HRSDK_PirInfo*)msgResp.buf;
                    
                    pirInfo->bEnable = stPirInfo.bEnable;
                    pirInfo->sensitivity = stPirInfo.sensitivity;
                    pirInfo->weekMap = stPirInfo.weekMap;
                    pirInfo->detectTimeSun = stPirInfo.detectTimeSun;
                    pirInfo->detectTimeMon = stPirInfo.detectTimeMon;
                    pirInfo->detectTimeTue = stPirInfo.detectTimeTue;
                    pirInfo->detectTimeWed = stPirInfo.detectTimeWed;
                    pirInfo->detectTimeThu = stPirInfo.detectTimeThu;
                    pirInfo->detectTimeFri = stPirInfo.detectTimeFri;
                    pirInfo->detectTimeSat = stPirInfo.detectTimeSat;
                    pirInfo->recordDuration = stPirInfo.recordDuration;
                    
                    SyncReqMsg msg = { pHead->seq, HRCMD_GETPIRINFO, stPirInfo.result };
                    m_syncQue.PostMsg(msg);
                }
                UNLOCK_MUTEX(&m_mtxRespQue);
                break;
            }
            case HRCMD_SETPIRINFO_RESP:
            {
                SyncReqMsg msg = { pHead->seq, HRCMD_SETPIRINFO, ((HiroCmdResp*)pHead->data)->result };
                m_syncQue.PostMsg(msg);
                break;
            }
            case HRCMD_SDCART_FORMAT_RESP:
            {
                HiroSDCardFormatResp stSDInfo = {0};
                if (pHead->ver >= HIROCAMPROT_VER) {
                    memcpy(&stSDInfo, pHead->data, sizeof(stSDInfo));
                }else{
                    memcpy(&stSDInfo, pHead->data, pHead->dataLen);
                }
                
                SyncRespMsg msgResp = {pHead->seq, HRCMD_SDCART_FORMAT_RESP};
                LOCK_MUTEX(&m_mtxRespQue);
                if ( m_syncRespQue.PeekEqMsg(msgResp) ){
                    HRSDK_SDCardInfo* pSDInfo = (HRSDK_SDCardInfo*)msgResp.buf;
                    
                    pSDInfo->sdTotalMB = stSDInfo.sdTotalMB;
                    pSDInfo->sdFreeMB = stSDInfo.sdFreeMB;
                    pSDInfo->sdCanRecordDays = stSDInfo.sdCanRecordDays;
                    
                    SyncReqMsg msg = { pHead->seq, HRCMD_SDCART_FORMAT, stSDInfo.result };
                    m_syncQue.PostMsg(msg);
                }
                UNLOCK_MUTEX(&m_mtxRespQue);
                break;
            }
            case HRCMD_HEART_BEAT:
                break;
            case HRCMD_VIDEO_MIRROR_RESP:
            {
                SyncReqMsg msg = { pHead->seq, HRCMD_VIDEO_MIRROR, ((HiroCmdResp*)pHead->data)->result };
                m_syncQue.PostMsg(msg);
                break;
            }
            case HRCMD_VIDEO_FLIP_RESP:
            {
                SyncReqMsg msg = { pHead->seq, HRCMD_VIDEO_FLIP, ((HiroCmdResp*)pHead->data)->result };
                m_syncQue.PostMsg(msg);
                break;
            }
            case HRCMD_START_TALK_RESP:
            {
                SyncReqMsg msg = { pHead->seq, HRCMD_START_TALK, ((HiroCmdResp*)pHead->data)->result };
                m_syncQue.PostMsg(msg);
                break;
            }
            case HRCMD_STOP_TALK_RESP:
            {
                SyncReqMsg msg = { pHead->seq, HRCMD_STOP_TALK, ((HiroCmdResp*)pHead->data)->result };
                m_syncQue.PostMsg(msg);
                break;
            }
            case HRCMD_START_SLEEP_RESP:
                //HiroCamParse_StartSleep(pSock, pHead->data, pHead->seq, pHead->ver);
                break;
            case HRCMD_CHANGEPWD_RESP:
            {
                SyncReqMsg msg = { pHead->seq, HRCMD_CHANGEPWD, ((HiroCmdResp*)pHead->data)->result };
                m_syncQue.PostMsg(msg);
                break;
            }
            case HRCMD_RECORD_GETMAP_RESP:
            {
                HiroGetRecordMapResp rcdMap;
                if (pHead->ver >= HIROCAMPROT_VER) {
                    memcpy(&rcdMap, pHead->data, sizeof(rcdMap));
                }else{
                    memcpy(&rcdMap, pHead->data, pHead->dataLen);
                }
                SyncReqMsg msg = { pHead->seq, HRCMD_RECORD_GETMAP, rcdMap.result, rcdMap.monthMap };
                m_syncQue.PostMsg(msg);
                break;
            }
            case HRCMD_RECORD_GETLIST_RESP:
            {
                SyncRespMsg msgResp = {pHead->seq, HRCMD_RECORD_GETLIST_RESP};
                LOCK_MUTEX(&m_mtxRespQue);
                if ( m_syncRespQue.PeekEqMsg(msgResp) ){
                    HRSDK_RecordListHead* pRst = (HRSDK_RecordListHead*)msgResp.buf;
                    HiroGetRecordListResp* pRecordList = (HiroGetRecordListResp*)pHead->data;
                    HRSDK_RecordListData* pListD = (HRSDK_RecordListData*)pRst->data;
                    HiroGetRecordListData* pList = (HiroGetRecordListData*)pRecordList->data;
                    pRst->result = pRecordList->result;
                    if (pRecordList->result == 0) {
                        int i=0;
                        for (; i<pRecordList->count && i < msgResp.bufSize; i++) {
                            strcpy(pListD[i].file, pList[i].file);
                        }
                        pRst->count = i;
                    }
                    
                    SyncReqMsg msg = { pHead->seq, HRCMD_RECORD_GETLIST, pRecordList->result};
                    m_syncQue.PostMsg(msg);
                }
                UNLOCK_MUTEX(&m_mtxRespQue);
                break;
            }
            case HRCMD_RECORD_START_RESP:
            {
                SyncRespMsg msgResp = {pHead->seq, HRCMD_RECORD_START_RESP};
                LOCK_MUTEX(&m_mtxRespQue);
                if ( m_syncRespQue.PeekEqMsg(msgResp) ){
                    HiroRecordStartResp rcdRsep;
                    if (pHead->ver >= HIROCAMPROT_VER) {
                        memcpy(&rcdRsep, pHead->data, sizeof(rcdRsep));
                    }else{
                        memcpy(&rcdRsep, pHead->data, pHead->dataLen);
                    }
                    
                    HRSDK_RecordInfo* pInfo = (HRSDK_RecordInfo*)msgResp.buf;
                    pInfo->iHeight = rcdRsep.iHeight;
                    pInfo->iWidth = rcdRsep.iWidth;
                    pInfo->iTotalTime = rcdRsep.iTotalTime;
                    pInfo->iTotalFrame = rcdRsep.iTotalFrame;
                    
                    SyncReqMsg msg = { pHead->seq, HRCMD_RECORD_START, rcdRsep.result};
                    m_syncQue.PostMsg(msg);
                }
                UNLOCK_MUTEX(&m_mtxRespQue);
                break;
            }
            case HRCMD_RECORD_STOP_RESP:
            {
                SyncReqMsg msg = { pHead->seq, HRCMD_RECORD_STOP_RESP, ((HiroCmdResp*)pHead->data)->result };
                m_syncQue.PostMsg(msg);
                break;
            }
                break;
            case HRCMD_RECORD_CAMMAND_RESP:
                //HiroCamParse_PlayRecordCmd(pSock, pHead->data, pHead->seq, pHead->ver);
                break;
            case HRCMD_STREAM_PLAYBACK:
            case HRCMD_STREAM_LIVE:
            {
                FrameHeadInfo frame;
                memcpy(&frame, pHead->data, sizeof(FrameHeadInfo));
                FrameHeadInfo* pRcvFrame = (FrameHeadInfo*)pHead->data;
                if (frame.videoLen > 0) {
                    HRSDK_DecFrame* pFrame = (HRSDK_DecFrame*)(pRcvFrame->data - sizeof(HRSDK_DecFrame));
                    pFrame->type = HRSDKMT_VIDEO;
                    pFrame->len = frame.videoLen;
                    pFrame->w = frame.videoWidth;
                    pFrame->h = frame.videoHeight;
                    pFrame->pts = frame.timeStamp;
                    pFrame->isKey = frame.isKeyFrame;
                    m_pVideoWriter->putFrame((HR_S8*)pFrame, pFrame->len+sizeof(HRSDK_DecFrame), SHM_ACCESS_WRITE_NONBLOCK, frame.isKeyFrame);
                }
                
                if (frame.audioLen > 0) {
                    HRSDK_DecFrame* pFrame = (HRSDK_DecFrame*)m_bufPcm;
                    if (frame.audio_format == 0x1) {
                        adpcm_decoder(pRcvFrame->data+frame.videoLen, (short*)(m_bufPcm+sizeof(HRSDK_DecFrame)), frame.audioLen*2, &m_stPcm);
                    }
                    pFrame->type = HRSDKMT_AUDIO;
                    pFrame->len = frame.audioLen*4;
                    pFrame->pts = frame.timeStamp;
                    m_pAudioWriter->putFrame((HR_S8*)pFrame, pFrame->len+sizeof(HRSDK_DecFrame), SHM_ACCESS_WRITE_NONBLOCK, 1);
                }
                
                break;
            }
        }
        return pHead->dataLen + sizeof(HiroCamMsgHead);
    }
    
    HR_S32 CHiroCamParse::onCheckData(HR_S8* pHead, HR_U32 len)
    {
        if (len < sizeof(HiroCamMsgHead))
            return HEADNOCOMP;
        
        HiroCamMsgHead d;
        memcpy(&d, pHead, sizeof(HiroCamMsgHead));
        if ( *(HR_S64*)(d.magic) == HIROCAMPROT_MAGIC ){
            if ( d.dataLen + (HR_S32)sizeof(HiroCamMsgHead) > len )
                return DATANOCOMP;
            return DATAOK;
        }
        return DATAERROR;
    }
    
    HR_S32 CHiroCamParse::onDataNoComp(HR_S8* pHead, HR_U32 MaxLen, HR_U32* pLen)
    {
        HiroCamMsgHead *head = (HiroCamMsgHead*)pHead;
        if (head->dataLen + sizeof(HiroCamMsgHead) >= MaxLen ){
            *pLen = 0;
        }
        return 0;
    }
} } }


