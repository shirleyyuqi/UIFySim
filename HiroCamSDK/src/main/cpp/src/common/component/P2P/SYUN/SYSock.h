/*
* Copyright (c) 2018,hiro 研发体系
* All rights reserved.
*
* 文件名称：SYSock.h
* 文件标识：
* 摘 要：
*
* 当前版本：1.0
* 作 者：luoc
* 完成日期：
*
* 取代版本：
* 原作者 ：
* 完成日期：
*/

#ifndef __SRC_NETSERVER_P2P_SYUN_SYSOCK_H__
#define __SRC_NETSERVER_P2P_SYUN_SYSOCK_H__
#include "BaseDataType.h"
#include "ISock.h"
#include "Thread.h"
namespace HiroCamSDK {
#define SOCKET_MAXCHANNEL       2
#define SOCKET_MAXBUF_SIZE      1024*1024
#define CONNECT_METHOD          3
    class SYSocket : public ISock{
    public:
        SYSocket(HR_S8* pUID);
        virtual ~SYSocket();
        
        virtual HR_S32 Init();
        virtual HR_S32 CreateSocket();
        virtual HR_S32 CloseSocket();
        virtual HR_S32 Connect(HR_S8* ip, HR_U16 port, HR_S8* usrdata, HR_U32 timeout=3000);
        virtual HR_S32 Recv(HR_S8* buf, HR_S32 n, HR_U32 channel);
        virtual HR_S32 Send(HR_S8* buf, HR_S32 n, HR_U32 channel, HR_S32 timeout);
        virtual HR_S32 UserQuit();
        virtual HR_BOOL IsQuit();
        virtual HR_VOID ResetQuit();
        virtual HR_S32 GetBufInfo(HR_U32 channel, HR_S8** ppBuf, HR_U32** ppRecv, HR_U32* pMaxLen);
    protected:
        HR_THREADMETHOD_DEC(CntThread0, lpParam);
        HR_THREADMETHOD_DEC(CntThread1, lpParam);
        HR_THREADMETHOD_DEC(CntThread2, lpParam);
        void CntThreadBody(HR_U32 id, HR_U32 srvIdx, HR_S8 bEnableLanSearch);
    private:
        HR_S8 GetPPCSLanSearchValue(HR_BOOL isNeedLan, HR_BOOL isNeedP2p, HR_BOOL isNeedRelay);
    private:
        HR_BOOL     m_bQuit;
        HR_BOOL     m_bClose;
        HR_BOOL     m_bQuitConnect;
        HR_S32      m_iSvrIdx;
        
        HR_S8*      m_pRcvBuf[SOCKET_MAXCHANNEL];
        HR_U32      m_iRcvLen[SOCKET_MAXCHANNEL];
        MUTEX       m_mutex[SOCKET_MAXCHANNEL];
        HR_U32      m_iSYCacheSize[SOCKET_MAXCHANNEL];
        HR_S8*      m_pUID;
        HR_S32      m_hSession;
        
        HR_S32      m_hTmpSession[CONNECT_METHOD];
        CThread     m_hCntThread[CONNECT_METHOD];
    };
}

#endif
