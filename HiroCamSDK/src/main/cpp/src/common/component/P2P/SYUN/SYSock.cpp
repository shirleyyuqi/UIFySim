#include "SYSock.h"
#include "PPCS_API.h"
#include "HRTime.h"
#include <string.h>
#include <cstdlib>
namespace HiroCamSDK {
    
    typedef struct tagPPCSServerInfo
    {
        HR_CS8* key;
        HR_CS8* initString;
        HR_CS8* crcKey;
    }PPCSServerInfo;
    
    static PPCSServerInfo s_SvrTbl[] = {
        {
            "AACS",
            "EFGHFDBLKEIAGDJIEKHAFFENGFNAHHMBHOFABEDLABJFLIKMDMAKCDPLGELGIGLHADNJKIDDPPNDBHCHIP",
            "HqTU2i53",
        },
        {
            "ABCS",
            "EEGDFHBMKGICGCJMEOHEFLEFGNNKHKNDHDFPBBDOAHJJLGKCDBADDFPAGLLCICLCAHNNKODAPMNDBOCOIG",
            "R2MepFsd",
        },
        {
            "PPCS",
            "EBGAEIBIKHJJGFJKEOGCFAEPHPMAHONDGJFPBKCPAJJMLFKBDBAGCJPBGOLKIKLKAJMJKFDOOFMOBECEJIMM",
            "EasyView",
        },
    };

    
    SYSocket::SYSocket(HR_S8* pUID)
    : m_hSession(-1)
    , m_bQuit(HR_FALSE)
    , m_bClose(HR_FALSE)
    , m_pUID(pUID)
    , m_bQuitConnect(HR_FALSE)
    , m_iSvrIdx(-1)
    {
        for (int i=0; i<SOCKET_MAXCHANNEL; i++) {
            m_pRcvBuf[i] = (HR_S8*)malloc(SOCKET_MAXBUF_SIZE);
            m_iRcvLen[i] = 0;
            INIT_MUTEX(&m_mutex[i]);
        }
        
        srand((unsigned int)time(0));
    }
    
    SYSocket::~SYSocket()
    {
        CloseSocket();
    }
    
    HR_S32 SYSocket::Init()
    {
        m_bQuitConnect = HR_FALSE;
        return 0;
    }
    
    HR_VOID SYSocket::ResetQuit()
    {
        m_bQuit = HR_FALSE;
    }
    
    HR_S32 SYSocket::CreateSocket()
    {
        return 0;
    }
    
    HR_S32 SYSocket::CloseSocket()
    {
        m_bQuitConnect = HR_TRUE;
        
        while (m_hCntThread[0].IsThreadRun()
               || m_hCntThread[1].IsThreadRun()
               || m_hCntThread[2].IsThreadRun()) {
            PPCS_Connect_Break();
            usleep(1000);
        }
        
        m_hCntThread[0].ExitThead(0, 0);
        m_hCntThread[1].ExitThead(0, 0);
        m_hCntThread[2].ExitThead(0, 0);
        if (!m_bClose) {
            LOCK_MUTEX(&m_mutex[0]);
            LOCK_MUTEX(&m_mutex[1]);
            if (m_hSession >= 0){
                PPCS_Close(m_hSession);
            }
            m_hSession = -1;
            UNLOCK_MUTEX(&m_mutex[1]);
            UNLOCK_MUTEX(&m_mutex[0]);
        }
        m_bClose = HR_TRUE;
        return 0;
    }
    
    HR_S32 SYSocket::Connect(HR_S8* ip, HR_U16 port, HR_S8* usrdata, HR_U32 timeout)
    {
        for (int i=0; i<sizeof(s_SvrTbl)/sizeof(s_SvrTbl[0]); i++) {
            if (strncmp(m_pUID, s_SvrTbl[i].key, 4) == 0) {
                m_iSvrIdx = i;
                break;
            }
        }
        
        if (m_iSvrIdx < 0) {
            return _SOCKET_IPERROR;
        }
        
        memset(m_hTmpSession, 0xff, sizeof(m_hTmpSession));
        m_hCntThread[0].CreateThread(1, 1, 256*1024, CntThread0, this);
        m_hCntThread[1].CreateThread(1, 1, 256*1024, CntThread1, this);
        m_hCntThread[2].CreateThread(1, 1, 256*1024, CntThread2, this);
        
        HR_U64 tStart = CTime::GetLocalTime_Ms();
        while (!m_bQuit && CTime::GetLocalTime_Ms() - tStart < timeout) {
            if (m_hTmpSession[0] >= 0 || m_hTmpSession[1] >= 0 || m_hTmpSession[2] >= 0) {
                break;
            }
        }
        
        m_bQuitConnect = HR_TRUE;
        while (m_hCntThread[0].IsThreadRun()
               || m_hCntThread[1].IsThreadRun()
               || m_hCntThread[2].IsThreadRun()) {
            PPCS_Connect_Break();
            usleep(1000);
        }
        
        m_hCntThread[0].ExitThead(0, 0);
        m_hCntThread[1].ExitThead(0, 0);
        m_hCntThread[2].ExitThead(0, 0);
        
        if (m_hTmpSession[0] >= 0) m_hSession = m_hTmpSession[0];
        else if (m_hTmpSession[1] >= 0) m_hSession = m_hTmpSession[1];
        else if (m_hTmpSession[2] >= 0) m_hSession = m_hTmpSession[2];
        else return (CTime::GetLocalTime_Ms() - tStart < timeout)?_SOCKET_USERQUIT:_SOCKET_TIMEOUT;
        
        for (int i=0; i<3; i++) {
            if (m_hTmpSession[i] != m_hSession && m_hTmpSession[i] >= 0) {
                PPCS_Close(m_hTmpSession[i]);
            }
        }
        
        return _SOCKET_SUCCESS;
    }
    
    HR_S32 SYSocket::Recv(HR_S8* data, HR_S32 len, HR_U32 channel)
    {
        HR_S32 dataSize = len;
        HR_U32 readBufSize = 0;
        if( PPCS_Check_Buffer(m_hSession, channel, NULL, &readBufSize) < 0 ){
            HRLOG_E("[HRSYSock_Read] check buf error, fd:%d\n", m_hSession);
            return -1;
        }
        
        HR_S32 ret = PPCS_Read(m_hSession, channel, data, &dataSize, 20);
        if (ret==ERROR_PPCS_SUCCESSFUL || ret==ERROR_PPCS_TIME_OUT){
            if (dataSize > 0){
                m_iRcvLen[channel] += dataSize;
                return dataSize;
            }
            return 0;
        }
        return 0;
    }
    
    HR_S32 SYSocket::Send(HR_S8* data, HR_S32 len, HR_U32 channel, HR_S32 timeout)
    {
        LOCK_MUTEX(&m_mutex[channel]);
        HR_U32 tSize = 0;
        while(1){
            if( PPCS_Check_Buffer(m_hSession, channel, &tSize, NULL) < 0 ){
                HRLOG_E("[HRSYSock_Read] check buf error, fd:%d\n", m_hSession);
                UNLOCK_MUTEX(&m_mutex[channel]);
                return -1;
            }
            
            m_iSYCacheSize[channel] = tSize;
            
            if( (tSize + len) < 256*1024) {
                break;
            }
            usleep(100000);
        }
        
        HR_U32 rSndSize = 0;
        while(len - rSndSize > 0){
            HR_S32 ret = PPCS_Write(m_hSession, channel, data+rSndSize, len - rSndSize);
            if (ret > 0 ){
                rSndSize += ret;
            }else if (ret == 0){
                usleep(100000);
            }else {
                HRLOG_I("PPCS_Write err\n");
                UNLOCK_MUTEX(&m_mutex[channel]);
                return -1;
            }
        }
        
        UNLOCK_MUTEX(&m_mutex[channel]);
        return rSndSize;
    }
    
    HR_S32 SYSocket::UserQuit()
    {
        m_bQuit = HR_TRUE;
        return 0;
    }
    
    HR_BOOL SYSocket::IsQuit()
    {
        return m_bQuit;
    }
    
    HR_S32 SYSocket::GetBufInfo(HR_U32 channel, HR_S8** ppBuf, HR_U32** ppRecv, HR_U32* pMaxLen)
    {
        
        *ppBuf = m_pRcvBuf[channel];
        *pMaxLen = SOCKET_MAXBUF_SIZE;
        *ppRecv = &m_iRcvLen[channel];
        
        return 0;
    }
    
    /*
     bEnableLanSearch:
     
     Bit 0 [LanSearch] , 0: Disable Lan search, 1: Enable Lan Search
     Bit 1~4 [P2P Try time]:
     0 (0b0000): 5 second (default)
     15 (0b1111): 0 second, No P2P trying
     Bit 5 [RelayOff], 0: Relay mode is allowed, 1: No Relay connection
     Bit 6 [ServerRelayOnly], 0: Device Relay is allowed, 1: Only Server relay (if Bit 5 = 1, this value is ignored)
    */
    HR_S8 SYSocket::GetPPCSLanSearchValue(HR_BOOL isNeedLan, HR_BOOL isNeedP2p, HR_BOOL isNeedRelay)
    {
        HR_S8 bEnableLanSearch = (isNeedLan & 0x1);
        if (isNeedP2p){
            bEnableLanSearch |= 0x0;
        }else{
            bEnableLanSearch |= 0x1E;
        }
        
        if (isNeedRelay){
            bEnableLanSearch |= (1 << 6);
        }else{
            bEnableLanSearch |= (1 << 5);
        }
        return bEnableLanSearch;
    }
    
    HR_THREADMETHOD(SYSocket::CntThread0, lpParam)
    {
        SYSocket* pThis = (SYSocket*)lpParam;
        pThis->CntThreadBody(0, pThis->m_iSvrIdx, pThis->GetPPCSLanSearchValue(HR_TRUE, HR_FALSE, HR_FALSE));
        THREAD_RETURN(0);
    }
    
    HR_THREADMETHOD(SYSocket::CntThread1, lpParam)
    {
        SYSocket* pThis = (SYSocket*)lpParam;
        pThis->CntThreadBody(1, pThis->m_iSvrIdx, pThis->GetPPCSLanSearchValue(HR_FALSE, HR_TRUE, HR_FALSE));
        THREAD_RETURN(0);
    }
    
    HR_THREADMETHOD(SYSocket::CntThread2, lpParam)
    {
        SYSocket* pThis = (SYSocket*)lpParam;
        pThis->CntThreadBody(2, pThis->m_iSvrIdx, pThis->GetPPCSLanSearchValue(HR_FALSE, HR_FALSE, HR_TRUE));
        THREAD_RETURN(0);
    }
    
    void SYSocket::CntThreadBody(HR_U32 id, HR_U32 srvIdx, HR_S8 bEnableLanSearch)
    {
        HR_U16 randomPort = rand()%60000+2048;
        HR_S32 ret = 0;
        
    RECONNECT:
        ret = PPCS_ConnectByServer((const char*)m_pUID, bEnableLanSearch, randomPort, (CHAR*)s_SvrTbl[srvIdx].initString);
        if (ret >= 0 ) {
            st_PPCS_Session Sinfo;
            if(PPCS_Check(ret, &Sinfo) == ERROR_PPCS_SUCCESSFUL){
                printf("Lan connect to remote success, mode:%s, cost time:%d, localAddr:(%s:%d), remoteAddr:(%s:%d)",
                     ((Sinfo.bMode ==0)? "P2P":"RLY"), Sinfo.ConnectTime,
                     inet_ntoa(Sinfo.MyLocalAddr.sin_addr),ntohs(Sinfo.MyLocalAddr.sin_port),
                     inet_ntoa(Sinfo.RemoteAddr.sin_addr),ntohs(Sinfo.RemoteAddr.sin_port));
            }
            
            int isGetAck = 0;
            char recvBuf[16] = {0};
            int recvSize = 3;
            int recvTimeIndex = 0;
            int recvDataIndex = 0;
            while (!m_bQuit && !m_bQuitConnect)
            {
                recvSize = 3;
                recvSize -= recvDataIndex;
                PPCS_Read(ret, 0, recvBuf+recvDataIndex, &recvSize, 10);        //10ms
                
                recvDataIndex += recvSize;
                if (recvDataIndex == 3)
                {
                    if (0 == strcmp(recvBuf, "ACK"))
                    {
                        isGetAck = 1;
                        break;
                    }
                }
                
                recvTimeIndex ++;
                if (recvTimeIndex >= 500){
                    goto RECONNECT;
                }
            }
            
            
            if (isGetAck){
                m_hTmpSession[id] = ret;
                m_bQuitConnect = HR_TRUE;
            }else{
                PPCS_ForceClose(ret);
            }
        }else if(ret != ERROR_PPCS_USER_CONNECT_BREAK){
            if ( m_bQuit || m_bQuitConnect ) {
                return ;
            }
            goto RECONNECT;
        }
    }
    
}
