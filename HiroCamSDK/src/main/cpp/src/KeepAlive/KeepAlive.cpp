#include <stdio.h>
#include "BaseDataType.h"
#include "HiroCamDef.h"
#include "HiroCamSDK.h"
#include "NDT_API.h"
#include "NDT_Error.h"
extern "C"{
    #include "WiPN_API.h"
    #include "WiPN_StringEncDec.h"
}

using namespace HiroCamSDK;

#define SERVER_NUM                      3
#define ERROR_NotLogin                  -1
#define ERROR_InvalidParameter          -2
#define ERROR_SocketCreateFailed        -3
#define ERROR_SendToFailed              -4
#define ERROR_RecvFromFailed            -5
#define ERROR_UnKnown                   -99

typedef struct tagSYServerInfo
{
    const char *ip;
    unsigned short port;
}SYServerInfo;
typedef struct tagSYServerTblInfo
{
    const char* key;
    const char* encDecKey;
    SYServerInfo server[SERVER_NUM];
}SYServerTblInfo;


static SYServerTblInfo s_SYServerTbl[] = {
    {
        "AACS",
        "bz01KId90P9UzvnC",
        {
            {"35.169.223.223", 12305},
            {"52.67.156.96", 12305},
            {"52.32.31.202", 12305},
        },
    },
    {
        "ABCS",
        "OKi2TcYvnd8lhFud",
        {
            {"119.23.14.192", 12305},
            {"121.43.181.16", 12305},
            {"47.52.206.30", 12305},
        },
    },
    {
        "PPCS",
        "1234567890ABCDEF",
        {
            {"54.84.37.235", 12305},
            {"54.254.195.28", 12305},
            {"112.74.108.149", 12305},
        },
    },
};

typedef struct tagWakeupQueryTaskInfo{
    char uid[32];
}WakeupQueryTaskInfo;

static std::map<std::string, WakeupQueryTaskInfo> sQueTable;
static std::map<std::string, int> sQueRst;

static MUTEX sQue;

    void KeepAlive_Init()
    {
        INIT_MUTEX(&sQue);
    }
    
    void KeepAlive_DeInit()
    {
        RELEASE_MUTEX(&sQue);
    }
    
    static int GetStringItem(    const char *SrcStr,
                      const char *ItemName,
                      const char Seperator,
                      char *RetString,
                      const int MaxSize)
    {
        if (!SrcStr || !ItemName || !RetString || 0 == MaxSize)
            return -1;
        
        const char *pFand = SrcStr;
        while (1)
        {
            pFand = strstr(pFand, ItemName);
            if (!pFand)
                return -2;
            pFand += strlen(ItemName);
            if ('=' != *pFand)
                continue;
            else
                break;
        }
        pFand += 1;
        int i = 0;
        while (1)
        {
            if (Seperator == *(pFand + i) || '\0' == *(pFand + i) || i >= (MaxSize - 1))
                break;
            else
                *(RetString + i) = *(pFand + i);
            i++;
        }
        *(RetString + i) = '\0';
        return 0;
    }
    
    static int getMinNumFromLastLogin(const int *pLastLogin, const unsigned char Length)
    {
        if (!pLastLogin)
        {
            //printf("getMinNumFromLastLogin - Invalid parameter!!\n");
            return ERROR_UnKnown;
        }
        int min = pLastLogin[0];
        for (int i = 0; i < Length; i++)
        {
            //printf("pLastLogin[%d]=%d\n", i, pLastLogin[i]);
            if (0 > pLastLogin[i])         // LastLogin<0: -99 or -1. min: -1,-99 or >= 0
            {
                if (min < pLastLogin[i])// min<0:-1,-99
                    min = pLastLogin[i];// min:-1
            }
            else if (0 > min || min > pLastLogin[i])// LastLogin>=0, min: unknown
                min = pLastLogin[i];    // min>=0
        }
        return min;
    }
    
    static int WakeUp_Query(const struct sockaddr_in *address,
                     const unsigned char NumberOfServer,
                     const char *Cmd,
                     const char *WakeupKey,
                     const int tryCount,
                     const unsigned int timeout_ms,
                     int *LastLogin1,
                     int *LastLogin2,
                     int *LastLogin3)
    {
        if (!address || 0 == NumberOfServer
            || !WakeupKey || 0 == strlen(WakeupKey)
            || !Cmd || 0 == strlen(Cmd))
        {
            printf("Invalid address!!");
            return -1;
        }
        
        struct sockaddr_in myAddr[SERVER_NUM];
        struct sockaddr_in fromAddr;
        memset(myAddr, 0, sizeof(myAddr));
        memset(&fromAddr, 0, sizeof(fromAddr));
        socklen_t sin_len = sizeof(struct sockaddr_in);
        
        char dest[20];
        memset(dest, 0, sizeof(dest));
        for (int i = 0; i < NumberOfServer; i++)
        {
            memcpy((char*)&myAddr[i], (char*)&address[i], sin_len);
            //printf("%d-%s:%d\n", i, inet_ntop(myAddr[i].sin_family, (char *)&myAddr[i].sin_addr.s_addr, dest, sizeof(dest)), ntohs(myAddr[i].sin_port), ntohs(myAddr[i].sin_port));
        }
        //printf("\n");
        
        char CMD[60];
        memset(CMD, 0, sizeof(CMD));
        memcpy(CMD, Cmd, strlen(Cmd));
        
        int flag[SERVER_NUM];
        memset(&flag, 0, sizeof(flag));
        int LastLogin[SERVER_NUM];
        for (int i = 0; i < SERVER_NUM; i++)
            LastLogin[i] = ERROR_UnKnown;
        
        int count = 0;
        int counter = 0;
        int timeOutCount = 0;
        int size;
        fd_set readfds;
        struct timeval timeout;
        char recvBuf[256];
        char Message[128];
        
        int skt = socket(AF_INET, SOCK_DGRAM, 0);
        if (0 > skt)
        {
            printf("create UDP socket failed");
            return ERROR_SocketCreateFailed;
        }
        
        while (tryCount > timeOutCount)
        {
            count = 0;
            counter = 0;
            for (int i = 0; i < NumberOfServer; i++)
            {
                memset(dest, 0, sizeof(dest));
                inet_ntop(myAddr[i].sin_family, (char *)&myAddr[i].sin_addr.s_addr, dest, sizeof(dest));
                if (0 == strcmp(dest, "0.0.0.0"))
                {
                    counter++;
                    continue;
                }
                if (0 == flag[i])
                {
                    size = sendto(skt, CMD, strlen(CMD), 0, (struct sockaddr *)&myAddr[i], sizeof(struct sockaddr_in));
                    if (0 > size)
                    {
                        printf("Sendto Error!");
                        close(skt);
                        return ERROR_SendToFailed;
                    }
                    printf("%d-Send CMD(%lu byte) to Wakeup_Server-%d %s:%d", i, strlen(CMD), i, inet_ntop(myAddr[i].sin_family, (char *)&myAddr[i].sin_addr.s_addr, dest, sizeof(dest)), ntohs(myAddr[i].sin_port));
                }
                else if (1 == flag[i])
                {
                    count++;
                    printf("%d-Done LastLogin=%d", i, LastLogin[i]);
                }
            } // for
            if (NumberOfServer == counter || NumberOfServer == count)
                break;
            printf("\n");
            
            FD_ZERO(&readfds);
            FD_SET(skt, &readfds);
            timeout.tv_sec = (timeout_ms-(timeout_ms%1000))/1000;
            timeout.tv_usec = (timeout_ms%1000)*1000;
            //printf("timeout value:%ld sec %ld usec\n", timeout.tv_sec, timeout.tv_usec);
            int result = select(skt + 1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &timeout);
            
            switch (result)
            {
                case 0: // time out
                    printf("(timeout, Counter=%d)", timeOutCount);
                    timeOutCount++;
                    break;
                case -1:
                {
                    printf("select error!");
                    close(skt);
                    return getMinNumFromLastLogin(LastLogin, SERVER_NUM);
                }
                default:
                    if (FD_ISSET(skt, &readfds))
                    {
                        memset(recvBuf, 0, sizeof(recvBuf));
                        memset(Message, 0, sizeof(Message));
                        
                        size = recvfrom(skt, recvBuf, sizeof(recvBuf), 0, (struct sockaddr *)&fromAddr, &sin_len);
                        if (0 > size)
                        {
                            printf("WakeUp_Query-RecvFrom error");
                            close(skt);
                            return ERROR_RecvFromFailed;
                        }
                        recvBuf[size] = '\0';
                        
                        if (0 > iPN_StringDnc(WakeupKey, recvBuf, Message, sizeof(Message)))
                        {
                            printf("WakeUp_Query-iPN_StringDnc failed.");
                            close(skt);
                            return getMinNumFromLastLogin(LastLogin, SERVER_NUM);
                        }
                        
                        counter = 0;
                        for (int i = 0; i < NumberOfServer; i++)
                        {
                            if (fromAddr.sin_addr.s_addr == myAddr[i].sin_addr.s_addr && fromAddr.sin_port == myAddr[i].sin_port)
                            {
                                printf("%d-Recv %s, Data: %s", i, inet_ntop(fromAddr.sin_family, (char *)&fromAddr.sin_addr.s_addr, dest, sizeof(dest)), Message);
                                
                                int lastLogin = ERROR_UnKnown;
                                char buf[8];
                                memset(buf, 0, sizeof(buf));
                                if (0 > GetStringItem(Message, "LastLogin", '&', buf, sizeof(buf)))
                                    printf("can not get LastLogin Item!");
                                else if (0 != strlen(buf))
                                    lastLogin = atoi(buf);
                                
                                flag[i] = 1;
                                LastLogin[i] = lastLogin;
                            }
                            if (1 == flag[i])
                                counter++;
                        } // for
                    }
                    else
                        printf("FD_ISSET error, readfds no data!!");
            } // switch
            if (NumberOfServer == counter)
                break; // break while
            printf("\n");
        } // while
        close(skt);
        
        int MinLastLogin = getMinNumFromLastLogin(LastLogin, SERVER_NUM);
        printf("** LastLogin[%d, %d, %d], Result: LastLogin = %d %s",
             LastLogin[0],LastLogin[1], LastLogin[2], MinLastLogin,
             (0<=MinLastLogin)? "sec before":((-1==MinLastLogin)?"NotLogin":"UnKnown"));
        //LOGD("Value: [>=0]: LastLogin Second, [-1]: NotLogin, [%d]: LastLogin UnKnown.", ERROR_UnKnown);
        
        if (NULL != LastLogin1) *LastLogin1 = LastLogin[0];
        if (NULL != LastLogin2) *LastLogin2 = LastLogin[1];
        if (NULL != LastLogin3) *LastLogin3 = LastLogin[2];
        
        return MinLastLogin;
    }
    
    static void *WakeUp_Query_ThreadFunc(void *arg)
    {
        WakeupQueryTaskInfo *pQueryTaskInfo = (WakeupQueryTaskInfo* )arg;
        
        // 根据UID获取查询服务器地址
        int serverIdx = 0;
        char uidPrefix[32] = {0};
        char* p = strchr((char* )pQueryTaskInfo->uid, '-');
        if( p != NULL )
        {
            memcpy( uidPrefix, pQueryTaskInfo->uid, (long long)p-(long long)pQueryTaskInfo->uid);
        }
        for( int i = 0; i < sizeof(s_SYServerTbl)/sizeof(s_SYServerTbl[0]); i++ )
        {
            if( !strcmp(uidPrefix, s_SYServerTbl[i].key) )
            {
                serverIdx = i;
                break;
            }
        }
        
        struct sockaddr_in address[SERVER_NUM];
        
        // 准备唤醒服务器查询地址（IP,Port...）
        for (int i = 0; i < SERVER_NUM; i++)
        {
            address[i].sin_family = AF_INET;
            address[i].sin_addr.s_addr = inet_addr(s_SYServerTbl[serverIdx].server[i].ip);
            address[i].sin_port = htons(s_SYServerTbl[serverIdx].server[i].port);
        }
        
        char CMD[30];
        memset(CMD, 0, sizeof(CMD));
        snprintf(CMD, sizeof(CMD), "DID=%s&", pQueryTaskInfo->uid);
        printf("Query Cmd: %s, size=%u byte", CMD, (unsigned)strlen(CMD));
        
        // 加密查询指令
        char CMD_Enc[60];
        memset(CMD_Enc, 0, sizeof(CMD_Enc));
        if (0 > iPN_StringEnc(s_SYServerTbl[serverIdx].encDecKey, CMD, CMD_Enc, sizeof(CMD_Enc)))
        {
            printf("WakeUp Query CMD StringEnc failed.");
            return NULL;
        }
        printf("[%s] %lu byte -> [%s] %lu byte", CMD, strlen(CMD), CMD_Enc, strlen(CMD_Enc));
        
        int LastLogin[3] = {-99, -99, -99};
        int LastLoginNum = WakeUp_Query(address,
                                        SERVER_NUM,
                                        CMD_Enc,
                                        s_SYServerTbl[serverIdx].encDecKey,
                                        3, 2000,
                                        &LastLogin[0],
                                        &LastLogin[1],
                                        &LastLogin[2]);
        
        
        if (-1 == LastLoginNum) {
            sQueRst[pQueryTaskInfo->uid] = HRKLSDKRST_FAIL_UNKOWN;
        } else if (-99 == LastLoginNum) {
            sQueRst[pQueryTaskInfo->uid] = HRKLSDKRST_SERVER_NOTRESPONSE;
        } else if (0 <= LastLoginNum) {
            sQueRst[pQueryTaskInfo->uid] = HRKLSDKRST_SUCCESS;
        } else {
            sQueRst[pQueryTaskInfo->uid] = HRKLSDKRST_FAIL_UNKOWN;
        }
        
        // 从任务队列中删除当前任务
        LOCK_MUTEX( &sQue );
        sQueTable.erase( pQueryTaskInfo->uid );
        UNLOCK_MUTEX( &sQue );
        return NULL;
    }

extern "C"{
    HRKEEPALIVESDK_RESULT HiroCamKlSDK_Query(const char* uid)
    {
        LOCK_MUTEX( &sQue );
        if( sQueTable.find(uid) == sQueTable.end() )
        {
            // 新建线程查询该UID的在线状态
            pthread_t thread;
            WakeupQueryTaskInfo queryTaskInfo;
            memset( &queryTaskInfo, 0x00, sizeof(WakeupQueryTaskInfo) );
            strcpy( queryTaskInfo.uid, uid );
            sQueTable[uid] = queryTaskInfo;
            pthread_create( &thread, NULL, WakeUp_Query_ThreadFunc, &sQueTable[uid]);
            pthread_detach(thread);
            UNLOCK_MUTEX( &sQue );
        }else{
            UNLOCK_MUTEX( &sQue );
            
            // 如果该UID正在查询中，则等待返回查询结果，不要再新建线程查询
            printf("Online query for uid:%s is running", uid);
            return HRKLSDKRST_QUERIING;
        }
        return HRKLSDKRST_SUCCESS;
    }
    
    HRKEEPALIVESDK_RESULT HiroCamKlSDK_GetResult(const char* uid)
    {
        HRKEEPALIVESDK_RESULT result = HRKLSDKRST_QUERIING;
        LOCK_MUTEX( &sQue );
        if( sQueRst.find(uid) != sQueRst.end() ) {
            result = (HRKEEPALIVESDK_RESULT)sQueRst[uid];
            sQueRst.erase( uid );
        }
        UNLOCK_MUTEX( &sQue );
        return result;
    }
    
}
