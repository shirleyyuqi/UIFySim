#include "WiPN_API.h"

//// Record RecvFrom return time
time_t g_Time_ServerRet = 0;

//// Mode:	0->Sending via Both LAN and Internet
//// 		1->Sending via Internet only
////		2->Sending via LAN only.
INT32 g_SendToMode = 0;	

typedef struct
{
	char *pDID;
	int Time;
	int Handle;
} st_DID;
static char *gEncDecKey;
#if defined(WIN32DLL) || defined(WINDOWS) 
unsigned int MyGetTickCount() 
{
	return GetTickCount();
}
#elif defined(LINUX)
char bFlagGetTickCount= 0;
struct timeval gTime_Begin;
unsigned int MyGetTickCount()
{
	if (!bFlagGetTickCount)
	{
		bFlagGetTickCount = 1;
		gettimeofday(&gTime_Begin, NULL);
		return 0;
	}
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec - gTime_Begin.tv_sec)*1000 + (tv.tv_usec - gTime_Begin.tv_usec)/1000;
}
#endif

void mSecSleep(UINT32 ms)
{
#if defined WINDOWS
	Sleep(ms);
#elif defined LINUX
	usleep(ms * 1000);
#endif
}

// get Error Code Info
const char *getErrorCodeInfo(int err)
{
    if (0 < err)
	{
		return "NoError, May be a Socket handle value or Data Size!";
	}
    switch (err)
    {
		// NDT Error
        case 0: return "NDT_ERROR_NoError";
        case -1: return "NDT_ERROR_AlreadyInitialized";
        case -2: return "NDT_ERROR_NotInitialized";
        case -3: return "NDT_ERROR_TimeOut";
        case -4: return "NDT_ERROR_ScketCreateFailed";
        case -5: return "NDT_ERROR_ScketBindFailed";
        case -6: return "NDT_ERROR_HostResolveFailed";
        case -7: return "NDT_ERROR_ThreadCreateFailed";
        case -8: return "NDT_ERROR_MemoryAllocFailed";
        case -9: return "NDT_ERROR_NotEnoughBufferSize";
        case -10: return "NDT_ERROR_InvalidInitString";
        case -11: return "NDT_ERROR_InvalidAES128Key";
        case -12: return "NDT_ERROR_InvalidDataOrSize";
        case -13: return "NDT_ERROR_InvalidDID";
        case -14: return "NDT_ERROR_InvalidNDTLicense";
        case -15: return "NDT_ERROR_InvalidHandle";
        case -16: return "NDT_ERROR_ExceedMaxDeviceHandle";
        case -17: return "NDT_ERROR_ExceedMaxClientHandle";
        case -18: return "NDT_ERROR_NetworkDetectRunning";
        case -19: return "NDT_ERROR_SendToRunning";
        case -20: return "NDT_ERROR_RecvRunning";
        case -21: return "NDT_ERROR_RecvFromRunning";
        case -22: return "NDT_ERROR_SendBackRunning";
        case -23: return "NDT_ERROR_DeviceNotOnRecv";
        case -24: return "NDT_ERROR_ClientNotOnRecvFrom";
        case -25: return "NDT_ERROR_NoAckFromCS";
        case -26: return "NDT_ERROR_NoAckFromPushServer";
        case -27: return "NDT_ERROR_NoAckFromDevice";
        case -28: return "NDT_ERROR_NoAckFromClient";
        case -29: return "NDT_ERROR_NoPushServerKnowDevice";
        case -30: return "NDT_ERROR_NoPushServerKnowClient";
        case -31: return "NDT_ERROR_UserBreak";
        case -32: return "NDT_ERROR_SendToNotRunning";
        case -33: return "NDT_ERROR_RecvNotRunning";
        case -34: return "NDT_ERROR_RecvFromNotRunning";
        case -35: return "NDT_ERROR_SendBackNotRunning";
        case -36: return "NDT_ERROR_RemoteHandleClosed";
        case -99: return "NDT_ERROR_FAESupportNeeded";
        // WiPN Error
        case WiPN_ERROR_InvalidParameter: return "WiPN_ERROR_InvalidParameter";
        case WiPN_ERROR_iPNStringEncFailed: return "WiPN_ERROR_iPNStringEncFailed";
        case WiPN_ERROR_iPNStringDncFailed: return "WiPN_ERROR_iPNStringDncFailed";
        case WiPN_ERROR_GetPostServerStringItemFailed: return "WiPN_ERROR_GetPostServerStringItemFailed";
        case WiPN_ERROR_GetSubscribeServerStringItemFailed: return "WiPN_ERROR_GetSubscribeServerStringItemFailed";
        case WiPN_ERROR_GetUTCTStringItemFailed: return "WiPN_ERROR_GetUTCTStringItemFailed";
        case WiPN_ERROR_GetNumberFromPostServerStringFailed: return "WiPN_ERROR_GetNumberFromPostServerStringFailed";
        case WiPN_ERROR_GetNumberFromSubscribeServerStringFailed: return "WiPN_ERROR_GetNumberFromSubscribeServerStringFailed";
        case WiPN_ERROR_GetDIDFromPostServerStringFailed: return "WiPN_ERROR_GetDIDFromPostServerStringFailed";
        case WiPN_ERROR_GetDIDFromSubscribeServerStringFailed: return "WiPN_ERROR_GetDIDFromSubscribeServerStringFailed";
        case WiPN_ERROR_GetRETStringItemFailed: return "WiPN_ERROR_GetRETStringItemFailed";
        case WiPN_ERROR_MallocFailed: return "WiPN_ERROR_MallocFailed";
		case WiPN_ERROR_ExceedMaxSize: return "WiPN_ERROR_ExceedMaxSize";
		// WiPN Web Error
		case WiPN_ERROR_TimeOut: return "WiPN_ERROR_TimeOut";
		case WiPN_ERROR_SocketCreateFailed: return "WiPN_ERROR_SocketCreateFailed";
		case WiPN_ERROR_SocketConnectFailed: return "WiPN_ERROR_SocketConnectFailed";
		case WiPN_ERROR_InvalidSocketID: return "WiPN_ERROR_InvalidSocketID";
		case WiPN_ERROR_SetSockOptFailed: return "WiPN_ERROR_SetSockOptFailed";
		case WiPN_ERROR_SocketWriteFailed: return "WiPN_ERROR_SocketWriteFailed";
		case WiPN_ERROR_SocketReadFailed: return "WiPN_ERROR_SocketReadFailed";
		case WiPN_ERROR_RemoteSocketClosed: return "WiPN_ERROR_RemoteSocketClosed";
		case WiPN_ERROR_SelectError: return "WiPN_ERROR_SelectError";
		case WiPN_ERROR_GetContentFromHttpRetDataFailed: return "WiPN_ERROR_GetContentFromHttpRetDataFailed";
		case WiPN_ERROR_GetNDTRETItemFailed: return "WiPN_ERROR_GetNDTRETItemFailed";
		case WiPN_ERROR_DeviceTokenIsEmpty: return "WiPN_ERROR_DeviceTokenIsEmpty";
        default:
            return "Unknow, something is wrong!";
    }
}

//// ret=0 OK, ret=-1: Invalid Parameter, ret=-2: No such Item
int GetStringItem(	const char *SrcStr, 
					const char *ItemName, 
					const char Seperator, 
					char *RetString, 
					const int MaxSize)
{
    if (!SrcStr || !ItemName || !RetString || 0 == MaxSize)
	{
		return -1;
	}
    const char *pFand = SrcStr;
    while (1)
    {
        pFand = strstr(pFand, ItemName);
        if (!pFand)
		{
			return -2;
		} 
        pFand += strlen(ItemName);
        if ('=' != *pFand)
		{
			continue;
		}     
        else
		{
			break;
		}    
    }
    
    pFand += 1;
    int i = 0;
    while (1)
    {
        if (Seperator == *(pFand + i) || '\0' == *(pFand + i) || i >= (MaxSize - 1))
		{
			break;
		} 
        else
		{
			*(RetString + i) = *(pFand + i);
		}     
        i++;
    }
    *(RetString + i) = '\0';
    
    return 0;
}

// Get Number From PostServer/SubscribeServer 
int pharse_number(const CHAR *pServerString, unsigned short *Number, unsigned short *SizeOfDID)
{
    if (!pServerString || !Number)
	{
		return -1;
	}
    CHAR buf[8];
    memset(buf, 0, sizeof(buf));
    const CHAR *pS = pServerString;
    const CHAR *p1 = strstr(pServerString, ",");
    if (!p1)
	{
		return -1;
	}  
    if (p1 - pS > sizeof(buf) - 1)
	{
		return -1;
	}  
    int i = 0;
    while (1)
    {
        if (pS + i >= p1)
		{
			break;
		} 
        buf[i] = *(pS + i);
        i++;
    }
    buf[i] = '\0';
    *Number = atoi(buf);
    
	// Get PostServerDID length
	p1 += 1; 	// -> point the First DID
	const char *p2 = strstr(p1, ",");
	if (!p2)	// -> Only one DID -> "01,ABCD-123456-ABCDEF"
	{
		*SizeOfDID = strlen(p1);
	}	
	else
	{
		*SizeOfDID = (unsigned short)(p2 - p1);
	}	
	//printf("SizeOfDID= %d\n", *SizeOfDID);
    return 0;
}

// Get PostServer/SubscribeServer DID
const char *pharse_DID(const char *pServerString, int index)
{
    if (!pServerString || 0 > index)
	{
		return NULL;
	}
    const char *p1 = strstr(pServerString, ",");
    if (!p1)
	{
		return NULL;
	}  
    p1 += 1;		// -> point the First DID
    
    const char *p2 = strstr(p1, ",");
    if (!p2) // -> Only one DID -> "01,ABCD-123456-ABCDEF"
	{
		unsigned short LengthOfDID = strlen(p1);
		if (0 == LengthOfDID)
		{
			return NULL;
		}			
		//printf("LengthOfDID= %d\n", LengthOfDID);
		char *pDID = (char *)malloc((LengthOfDID/4+1)*4);
		if (!pDID)
		{
			printf("pharse_DID - malloc failed!!\n");
			return NULL;
		}
		memset(pDID, '\0', (LengthOfDID/4+1)*4);
		memcpy(pDID, p1, LengthOfDID);
		return pDID;
	}
    unsigned short SizeOfDID = (unsigned short)(p2 - p1);
    //printf("SizeOfDID= %u\n", SizeOfDID);
    
    p1 = pServerString;
    int i = 0;
    for (; i < index + 1; i++)
    {
        p1 = strstr(p1, ",");
        if (!p1)
		{
			 break;
		}    
        p1 += 1;
    }
    if (!p1)
	{
		return NULL;
	}  
    char *pDID = (char *)malloc((SizeOfDID/4+1)*4);
	if (!pDID)
	{
		printf("pharse_DID - Malloc failed!!\n");
		return NULL;
	}
    memset(pDID, '\0', (SizeOfDID/4+1)*4);
    memcpy(pDID, p1, SizeOfDID);
    //printf("p_DID = %s\n", p_DID);
    
    return pDID;
}

void st_swap(st_DID arr[], unsigned int x, unsigned int y)
{
	char *pTemp = arr[x].pDID;
	arr[x].pDID = arr[y].pDID;
	arr[y].pDID = pTemp;
	
	arr[x].Time = arr[x].Time ^ arr[y].Time;
	arr[y].Time = arr[x].Time ^ arr[y].Time;
	arr[x].Time = arr[x].Time ^ arr[y].Time;
}

void st_selectSort(st_DID arr[], unsigned int length)
{
	for (int x = 0; x < length - 1; x++)
	{
		for (int y = x + 1; y < length; y++)
		{
			if (arr[x].Time > arr[y].Time)
			{
				st_swap(arr, x, y);
			}
		}
	}
}

//// ------------------- WiPN API Begin -----------------
//// WiPN_Query for WiPN API
INT32 WiPN_Query(char *encDecKey,
                 const CHAR *pDeviceDID, 			// The target device DID
                 const CHAR *QueryServerDID[], 		// QueryServer DID
                 CHAR *pPostServerString, 			// Save PostServerString
                 UINT32 SizeOfPostServerString, 		// PostServerString Buf Size
                 CHAR *pSubscribeServerString, 		// Save SubscribeServerString
                 UINT32 SizeOfSubscribeServerString, // SubscribeServerString Buf Size
                 CHAR *pUTCTString, 					// Save UTCT From QueryServer response
                 UINT32 SizeOfUTCTString)			// UTCTString Buf Size
{
    gEncDecKey = encDecKey;
    if (!pDeviceDID || 0 == strlen(pDeviceDID))
    {
        printf("WiPN_Query - DeviceDID is NULL!!\n");
        return WiPN_ERROR_InvalidParameter;
    }
	if (!QueryServerDID)
    {
        printf("WiPN_Query - QueryServerDID Buf is NULL!!\n");
        return WiPN_ERROR_InvalidParameter;
    }
    int i = 0;
    for (; i < QUERY_SERVER_NUMBER; i++)
    {
        if (NULL == QueryServerDID[i])
        {
            printf("WiPN_Query - QueryServerDIDbuf[%d] have no DID!!\n", i);
            return WiPN_ERROR_InvalidParameter;
        }
    }
    if (!pPostServerString && !pSubscribeServerString)
    {
        printf("WiPN_Query - PostServerString && SubscribeServerString Buf is NULL!!\n");
        return WiPN_ERROR_InvalidParameter;
    }
    if (NULL != pPostServerString && 0 == SizeOfPostServerString)
    {
        printf("WiPN_Query - SizeOfPostServerString= %d\n", SizeOfPostServerString);
        return WiPN_ERROR_InvalidParameter;
    }
    if (NULL != pSubscribeServerString && 0 == SizeOfSubscribeServerString)
    {
        printf("WiPN_Query - SizeOfSubscribeServerString= %d\n", SizeOfSubscribeServerString);
        return WiPN_ERROR_InvalidParameter;
    }
    if (!pUTCTString)
    {
        printf("WiPN_Query - pUTCTString is NULL!!\n");
        return WiPN_ERROR_InvalidParameter;
    }
    if (NULL != pUTCTString && 0 == SizeOfUTCTString)
    {
        printf("WiPN_Query - SizeOfUTCTString= 0 !!\n");
        return WiPN_ERROR_InvalidParameter;
    }
	
	// Save: 1.Cmd (Encryption before)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOf_CR_Buffer = 512;
	char CR_Buffer[512] = {0};
	/*char *CR_Buffer = (char *)malloc(SizeOf_CR_Buffer);
	if (!CR_Buffer)
	{
		printf("WiPN_Query - Malloc Query Cmd Buf failed!!\n");
		return WiPN_ERROR_MallocFailed;
	}*/  
	
	// Save: 1.Cmd (After encryption)
	//		2.Response: Server response data (Not decrypt yet)
	unsigned short SizeOfBuffer = SizeOf_CR_Buffer*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("WiPN_Query - Malloc Buffer failed!!\n");
		//free((void *)CR_Buffer);
		return WiPN_ERROR_MallocFailed;
	}
	
	// Formatted query CMD -> CR_Buffer
    sprintf(CR_Buffer, "DID=%s&", pDeviceDID);
    printf("QueryCmd= %s\nSize= %u byte\n", CR_Buffer, (UINT32)strlen(CR_Buffer));	
	
	// Encryption query CMD -> Buffer
//    if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, CR_Buffer, Buffer, SizeOfBuffer))
    if (0 > iPN_StringEnc(gEncDecKey, CR_Buffer, Buffer, SizeOfBuffer))
    {
        printf("WiPN_Query - iPN_StringEnc: Query Cmd Enc failed!\n");
		//free((void *)CR_Buffer);
		free((void *)Buffer);
		return WiPN_ERROR_iPNStringEncFailed;
    }

	int ret = 0;
    int QueryServerHandle = -1;
    short retryCount_RecvFrom = 0;
	srand((UINT32)time(NULL));
	
	short index = abs(rand() % QUERY_SERVER_NUMBER);
    while (1)
    {
        if (0 > QueryServerHandle)
        {      
            for (i = 0; i < QUERY_SERVER_NUMBER; i++)
            {
				index = (index + 1) % QUERY_SERVER_NUMBER;
				
				unsigned short SizeOfQueryCmd = strlen(Buffer);
				printf("send cmd to QueryServer, QueryServerDID[%d]= %s. SizeOfQueryCmd= %u byte. sending...\n", index, QueryServerDID[index], SizeOfQueryCmd);
				
				unsigned int tick = MyGetTickCount();
				
				// Send query cmd to QueryServer
				ret = NDT_PPCS_SendTo(	QueryServerDID[index], 
										Buffer, 
										SizeOfQueryCmd,
										g_SendToMode);
				printf("NDT_PPCS_SendTo done! TimeUsed: %u ms\n", MyGetTickCount() - tick);
				
				if (0 > ret)
                {
                    printf("send cmd to QueryServer failed!! ret= %d [%s]\n\n", ret, getErrorCodeInfo(ret));
					printf("retry to send ...\n\n");
                    continue;
                }
                else
                {
                    printf("send cmd to QueryServer success!! \n");
					QueryServerHandle = ret;
                    break;
                }
            }
            if (0 > QueryServerHandle)
            {
                printf("WiPN_Query - Get QueryServerHandle failed! QueryServerDID[%d]= %s. ret= %d [%s]\n", index, QueryServerDID[index], ret, getErrorCodeInfo(ret));
                break;
            }
        }
        else
        {
			printf("Waiting for QueryServer response, NDT_PPCS_RecvFrom ...\n");                
			UINT16 SizeToRead = SizeOfBuffer;
			memset(Buffer, 0, SizeOfBuffer);	
			
			ret = NDT_PPCS_RecvFrom(QueryServerHandle, Buffer, &SizeToRead, 3000); 

			// Record RecvFrom return time
			time_t Time_ServerRet = time(NULL);
			struct tm *ptm = localtime((const time_t *)&Time_ServerRet);
            
            if (0 > ret)
            {
				printf("WiPN_Query - NDT_PPCS_RecvFrom: QueryServerDID[%d]= %s. ret= %d. [%s]\n", index, QueryServerDID[index], ret, getErrorCodeInfo(ret));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				} 
				
                // -26 -27 -3
                if ((NDT_ERROR_NoAckFromPushServer == ret || NDT_ERROR_NoAckFromDevice == ret || NDT_ERROR_TimeOut == ret) && 3 > retryCount_RecvFrom)
                {
                    retryCount_RecvFrom++;
                    continue;
                }
                else if (NDT_ERROR_RemoteHandleClosed == ret) // -36
				{
					printf("WiPN_Query - QueryServer already call CloseHandle(). QueryServerDID[%d]= %s\n", index, QueryServerDID[index]);
				} 
                break;
            }
            else
            {
				// Decrypt received data, Save in CR_Buffer             
//                if (0 > iPN_StringDnc(STRING_ENC_DEC_KEY, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                if (0 > iPN_StringDnc(gEncDecKey, Buffer, CR_Buffer, SizeOf_CR_Buffer))

                {
                    printf("WiPN_Query - NDT_PPCS_RecvFrom - iPN_StringDnc: RecvFrom Data Dec failed! QueryServerDID[%d]= %s. \n", index, QueryServerDID[index]);
                    ret = WiPN_ERROR_iPNStringDncFailed;
                    break;
                }
				printf("\nFrom QueryServer: \n");
				printf("QueryServerDID[%d]= %s\n", index, QueryServerDID[index]);
				printf("QueryServerHandle= %d\n", QueryServerHandle);
				//printf("RecvFromData: %s\nSizeOfRecvFromData: %u byte\n", Buffer, (UINT32)strlen(Buffer));
				printf("Data: %s\nSizeOfData: %u byte\n", CR_Buffer, (UINT32)strlen(CR_Buffer));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}		
				
                // split CR_Buffer, Get PostServerString , From the end of the identifier '&'
                if (NULL != pPostServerString && 0 > GetStringItem(CR_Buffer, "Post", '&', pPostServerString, SizeOfPostServerString))
                {
                    printf("WiPN_Query - Get PostServerString failed!\n");
                    ret = WiPN_ERROR_GetPostServerStringItemFailed;
                }
				// split CR_Buffer, Get SubscribeString 
                if (NULL != pSubscribeServerString && 0 > GetStringItem(CR_Buffer, "Subs", '&', pSubscribeServerString, SizeOfSubscribeServerString))
                {
                    printf("WiPN_Query - Get SubscribeServerString failed!\n");
                    ret = WiPN_ERROR_GetSubscribeServerStringItemFailed;
                }
				// split CR_Buffer, Get UTCTString 
                memset(pUTCTString, 0, SizeOfUTCTString);
                if (0 > GetStringItem(CR_Buffer, "UTCT", '&', pUTCTString, SizeOfUTCTString))
                {
                    printf("WiPN_Query - Get UTCTString failed!\n");
                    ret = WiPN_ERROR_GetUTCTStringItemFailed;
                }
				else // g_Time_ServerRet Must be synchronized with the UTCTString time update
				{
					g_Time_ServerRet = Time_ServerRet;
				}
            } // ret > 0
            break;
        } // Handle > 0
    } // while
	if (0 <= QueryServerHandle)
	{	
		NDT_PPCS_CloseHandle(QueryServerHandle);
		printf("NDT_PPCS_CloseHandle(%d) done!\n\n", QueryServerHandle);
	}
	//if (CR_Buffer) free((void *)CR_Buffer);
	if (Buffer) free((void *)Buffer);
    
	return ret;
} // WiPN Query

// WiPN_QueryByServer 
INT32 WiPN_QueryByServer(
                         char *encDecKey,
                         const CHAR *pServerString,
						const CHAR *pAES128Key,
						const CHAR *pDeviceDID, 
						const CHAR *QueryServerDID[], 
						CHAR *pPostServerString, 
						UINT32 SizeOfPostServerString, 
						CHAR *pSubscribeServerString, 
						UINT32 SizeOfSubscribeServerString, 
						CHAR *pUTCTString, 
						UINT32 SizeOfUTCTString)
{
    gEncDecKey = encDecKey;
    if (!pServerString || 0 == strlen(pServerString))
	{
		printf("WiPN_QueryByServer - ServerString is NULL!!\n");
        return WiPN_ERROR_InvalidParameter;
	}
	if (!pAES128Key || 0 == strlen(pAES128Key))
	{
		printf("WiPN_QueryByServer - AES128Key is NULL!!\n");
        return WiPN_ERROR_InvalidParameter;
	}
	if (!pDeviceDID || 0 == strlen(pDeviceDID))
    {
        printf("WiPN_QueryByServer - DeviceDID is NULL!!\n");
        return WiPN_ERROR_InvalidParameter;
    }
	if (!QueryServerDID)
    {
        printf("WiPN_QueryByServer - QueryServerDID Buf is NULL!!\n");
        return WiPN_ERROR_InvalidParameter;
    }
    int i = 0;
    for (; i < QUERY_SERVER_NUMBER; i++)
    {
        if (NULL == QueryServerDID[i])
        {
            printf("WiPN_QueryByServer - QueryServerDIDbuf[%d] have no DID!!\n", i);
            return WiPN_ERROR_InvalidParameter;
        }
    }
    if (!pPostServerString && !pSubscribeServerString)
    {
        printf("WiPN_QueryByServer - PostServerString && SubscribeServerString Buf is NULL!!\n");
        return WiPN_ERROR_InvalidParameter;
    }
    if (NULL != pPostServerString && 0 == SizeOfPostServerString)
    {
        printf("WiPN_QueryByServer - SizeOfPostServerString= %d\n", SizeOfPostServerString);
        return WiPN_ERROR_InvalidParameter;
    }
    if (NULL != pSubscribeServerString && 0 == SizeOfSubscribeServerString)
    {
        printf("WiPN_QueryByServer - SizeOfSubscribeServerString= %d\n", SizeOfSubscribeServerString);
        return WiPN_ERROR_InvalidParameter;
    }
    if (!pUTCTString)
    {
        printf("WiPN_QueryByServer - UTCTString Buf is NULL!!\n");
        return WiPN_ERROR_InvalidParameter;
    }
    if (NULL != pUTCTString && 0 == SizeOfUTCTString)
    {
        printf("WiPN_QueryByServer - SizeOfUTCTString= 0 !!\n");
        return WiPN_ERROR_InvalidParameter;
    }
	
	// Save: 1.Cmd (Encryption before)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOf_CR_Buffer = 512;
	char CR_Buffer[512] = {0};
	/*char *CR_Buffer = (char *)malloc(SizeOf_CR_Buffer);
	if (!CR_Buffer)
	{
		printf("WiPN_QueryByServer - Malloc Query Cmd Buf failed!!\n");
		return WiPN_ERROR_MallocFailed;
	}*/  
	
	// Save: 1.Cmd (After encryption)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOfBuffer = SizeOf_CR_Buffer*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("WiPN_QueryByServer - Malloc Buffer failed!!\n");
		//free((void *)CR_Buffer);
		return WiPN_ERROR_MallocFailed;
	}
	
	// Formatted query CMD -> CR_Buffer
    sprintf(CR_Buffer, "DID=%s&", pDeviceDID);
    printf("QueryCmd= %s\nSize= %u byte\n", CR_Buffer, (UINT32)strlen(CR_Buffer));	
	
	// Encryption query CMD -> Buffer
//    if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, CR_Buffer, Buffer, SizeOfBuffer))
    if (0 > iPN_StringEnc(gEncDecKey, CR_Buffer, Buffer, SizeOfBuffer))
    {
        printf("WiPN_QueryByServer - iPN_StringEnc: Query Cmd Enc failed!\n");
		//free((void *)CR_Buffer);
		free((void *)Buffer);
		return WiPN_ERROR_iPNStringEncFailed;
    }
    
	int ret = 0;
    int QueryServerHandle = -1;
    short retryCount_RecvFrom = 0;
	srand((UINT32)time(NULL));
	
	short index = abs(rand() % QUERY_SERVER_NUMBER);
    while (1)
    {
        if (0 > QueryServerHandle)
        {      
            for (i = 0; i < QUERY_SERVER_NUMBER; i++)
            {
				index = (index + 1) % QUERY_SERVER_NUMBER;
				
				unsigned short SizeOfQueryCmd = strlen(Buffer);
				printf("send cmd to QueryServer, QueryServerDID[%d]= %s. SizeOfQueryCmd= %u byte. sending...\n", index, QueryServerDID[index], SizeOfQueryCmd);
				
				unsigned int tick = MyGetTickCount();	
				
				// Send query cmd to QueryServer
				ret = NDT_PPCS_SendToByServer(	QueryServerDID[index], 
												Buffer, SizeOfQueryCmd, 
												g_SendToMode, 
												pServerString,
												pAES128Key);
				printf("NDT_PPCS_SendToByServer done! TimeUsed: %u ms\n", MyGetTickCount() - tick);

				if (0 > ret)
                {
                    printf("send cmd to QueryServer failed!! ret= %d [%s]\n\n", ret, getErrorCodeInfo(ret));
					printf("retry to send ...\n\n");
                    continue;
                }
                else
                {
                    printf("send cmd to QueryServer Success!! \n");
					QueryServerHandle = ret;
                    break;
                }
            }
            if (0 > QueryServerHandle)
            {
                printf("WiPN_QueryByServer - Get QueryServerHandle failed! QueryServerDID[%d]= %s. ret= %d [%s]\n", index, QueryServerDID[index], ret, getErrorCodeInfo(ret));
                break;
            }
        }
        else
        {
			printf("Waiting for QueryServer response, NDT_PPCS_RecvFrom ... %d\n", QueryServerHandle);
            UINT16 SizeToRead = SizeOfBuffer;
			memset(Buffer, 0, SizeOfBuffer);
            //ret = 0;
            //int tm = 0;
            //while(ret == 0){
                ret = NDT_PPCS_RecvFrom(QueryServerHandle, Buffer, &SizeToRead, 3000);
            //    printf("%d Waiting for QueryServer response, NDT_PPCS_RecvFrom ... %d\n", tm, QueryServerHandle);
            //    tm ++;
            //    if (tm >= 10)break;
            //}
			// Record RecvFrom return time
			time_t Time_ServerRet = time(NULL);
			struct tm *ptm = localtime((const time_t *)&Time_ServerRet);
            
            if (0 > ret)
            {
				printf("WiPN_QueryByServer - NDT_PPCS_RecvFrom: QueryServerDID[%d]= %s. ret= %d. [%s]\n", index, QueryServerDID[index], ret, getErrorCodeInfo(ret));
				if (ptm)
				{
					printf("LocalTime: %d-%d-%d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}
				
                // -26 -27 -3
                if ((NDT_ERROR_NoAckFromPushServer == ret || NDT_ERROR_NoAckFromDevice == ret || NDT_ERROR_TimeOut == ret) && 3 > retryCount_RecvFrom)
                {
                    retryCount_RecvFrom++;
                    continue;
                }
                else if (NDT_ERROR_RemoteHandleClosed == ret) // -36
				{
					printf("WiPN_QueryByServer - QueryServer already call CloseHandle(). QueryServerDID[%d]= %s\n", index, QueryServerDID[index]);
				} 
                break;
            }
            else
            {
				// Decrypt received data, Save in CR_Buffer
                
//                if (0 > iPN_StringDnc(STRING_ENC_DEC_KEY, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                if (0 > iPN_StringDnc(gEncDecKey, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                {
                    printf("WiPN_QueryByServer - NDT_PPCS_RecvFrom - iPN_StringDnc: RecvFrom Data Dec failed! QueryServerDID[%d]= %s. \n", index, QueryServerDID[index]);
                    ret = WiPN_ERROR_iPNStringDncFailed;
                    break;
                }
				printf("\nFrom QueryServer: \n");
				printf("QueryServerDID[%d]= %s\n", index, QueryServerDID[index]);
				printf("QueryServerHandle= %d\n", QueryServerHandle);
				//printf("RecvFromData: %s\nSizeOfRecvFromData: %u byte\n", Buffer, (UINT32)strlen(Buffer));
				printf("Data: %s\nSizeOfData: %u byte\n", CR_Buffer, (UINT32)strlen(CR_Buffer));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}	
				
                // split CR_Buffer, Get PostServerString , From the end of the identifier '&'
                if (NULL != pPostServerString && 0 > GetStringItem(CR_Buffer, "Post", '&', pPostServerString, SizeOfPostServerString))
                {
                    printf("WiPN_QueryByServer - Get PostServerString failed!\n");
                    ret = WiPN_ERROR_GetPostServerStringItemFailed;
                }
				// split CR_Buffer, Get SubscribeString 
                if (NULL != pSubscribeServerString && 0 > GetStringItem(CR_Buffer, "Subs", '&', pSubscribeServerString, SizeOfSubscribeServerString))
                {
                    printf("WiPN_QueryByServer - Get SubscribeServerString failed!\n");
                    ret = WiPN_ERROR_GetSubscribeServerStringItemFailed;
                }
				// split CR_Buffer, Get UTCTString 
                memset(pUTCTString, 0, SizeOfUTCTString);
                if (0 > GetStringItem(CR_Buffer, "UTCT", '&', pUTCTString, SizeOfUTCTString))
                {
                    printf("WiPN_QueryByServer - Get UTCTString failed!\n");
                    ret = WiPN_ERROR_GetUTCTStringItemFailed;
                }
				else // g_Time_ServerRet Must be synchronized with the UTCTString time update
				{
					g_Time_ServerRet = Time_ServerRet;
				}
            } // ret > 0
            break;
        } // Handle > 0
    } // while
	if (0 <= QueryServerHandle)
	{
		NDT_PPCS_CloseHandle(QueryServerHandle);
		printf("WiPN_QueryByServer - NDT_PPCS_CloseHandle(%d)\n\n", QueryServerHandle);
	}
	//if (CR_Buffer)
	//	free((void *)CR_Buffer);
	if (Buffer)
		free((void *)Buffer);
	
    return ret;
}

#if SUBSCRIBE_FLAG
//// WiPN_Subscribe 
INT32 WiPN_Subscribe(const CHAR *pSubscribeServerString,
                     const CHAR *pSubCmd,
                     CHAR *pRETString,
                     UINT32 SizeOfRETString,
                     CHAR *pUTCTString,
                     UINT32 SizeOfUTCTString)
{
	if (   !pSubscribeServerString 
		|| !pSubCmd 
		|| !pRETString 
		|| !pUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
    if (   0 == strlen(pSubscribeServerString) 
		|| 0 == strlen(pSubCmd) 
		|| 0 == SizeOfRETString
		|| 0 == strlen(pUTCTString)
		|| 0 == SizeOfUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
	//// -------------------------- Get Server DID -------------------------
	//// ServerDIDString: "NN,DID,DID,DID,...,DID"
    //// sample: "03,PPCS-123456-ETLRN,PPCS-123456-HYEXE,PPCS-123456-RFVMV"
	//// Get Number From ServerDIDString
    unsigned short NumberOfServer = 0;
    unsigned short SizeOfDID = 0;
    if (0 > pharse_number(pSubscribeServerString, &NumberOfServer, &SizeOfDID) || 0 == NumberOfServer)
	{
		return WiPN_ERROR_GetNumberFromSubscribeServerStringFailed;
	}
    SizeOfDID = (SizeOfDID/4+1)*4; // Keep enough distance between DID
    
    // Allocate memory space according to the number of SubscribeServerDID
    CHAR *pServerDID = (CHAR *)malloc(SizeOfDID*NumberOfServer);
    if (!pServerDID)
    {
        printf("WiPN_Subscribe - Malloc ServerDID Buf failed!!\n");
        return WiPN_ERROR_MallocFailed;
    }
    memset(pServerDID, '\0', SizeOfDID*NumberOfServer);
    
    //// Get DID From ServerDIDString
    const CHAR *pDID = NULL;
    int i = 0;
    for ( ; i < NumberOfServer; i++)
    {
        pDID = pharse_DID(pSubscribeServerString, i);
        if (!pDID)
        {
            free((void *)pServerDID);
            return WiPN_ERROR_GetDIDFromSubscribeServerStringFailed;
        }
        memcpy(&pServerDID[SizeOfDID*i], pDID, strlen(pDID));
        //printf("SubscribeServerDID[%d]= %s\n", i, &pServerDID[SizeOfDID*i]);
        free((void*)pDID);
		pDID = NULL;
    }
	//// -------------------------------------------------------------------------
    // Calculate the size of SubscribeCmd
    UINT32 Length_Cmd = strlen(pSubCmd)+strlen("UTCT=0x&")+strlen(pUTCTString); 
	
	// NDT can only send a maximum of 1280 bytes of data, according to the encryption algorithm, encryption data can not be more than 630 bytes
    UINT32 MaxSizeOfCmd = 630;
    if (MaxSizeOfCmd < Length_Cmd)
    {
        printf("WiPN_Subscribe - Length Of SubscribeCmd is Exceed %d bytes!!\n", MaxSizeOfCmd);
        free((void *)pServerDID);
        return WiPN_ERROR_ExceedMaxSize;
    }
	
	// Save: 1.Cmd (Encryption before)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOf_CR_Buffer = (Length_Cmd/4+1)*4;
	char *CR_Buffer = (char *)malloc(SizeOf_CR_Buffer);
	if (!CR_Buffer)
	{
		printf("WiPN_Subscribe - Malloc Cmd Buf failed!!\n");
		free((void *)pServerDID);
		return WiPN_ERROR_MallocFailed;
	}   
	
	// Save: 1.Cmd (After encryption)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOfBuffer = SizeOf_CR_Buffer*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("WiPN_Subscribe - Malloc Buffer failed!!\n");
		free((void *)pServerDID);
		free((void *)CR_Buffer);
		return WiPN_ERROR_MallocFailed;
	}
	
    int ret = 0;
    int SubscribeServerHandle = -1;
	short retryCount_RecvFrom = 0;
    srand((UINT32)time(NULL));
	unsigned short PrintfFlag = 0;
	
    short index = 0;
    unsigned short j = abs(rand() % NumberOfServer);
    while (1)
    {
        if (0 > SubscribeServerHandle)
        {
            for (i = 0; i < NumberOfServer; i++)
            {
                j = (j + 1) % NumberOfServer;
                index = SizeOfDID * j;

                // Calculate UTCT time
                UINT32 UTCT_Subscribe = time(NULL) - g_Time_ServerRet + strtol(pUTCTString, NULL, 16);
                
                // Format Subscribe CMD -> CR_Buffer
                memset(CR_Buffer, 0, SizeOf_CR_Buffer);
                SNPRINTF(CR_Buffer, SizeOf_CR_Buffer, "%sUTCT=0x%X&", pSubCmd, (UINT32)UTCT_Subscribe);
				if (0 == PrintfFlag)
				{
					PrintfFlag = 1;
					printf("SubscribeCmd= %s\nSubscribeCmdSize= %u byte (Not Encrypted Size)\n\n", CR_Buffer, (UINT32)strlen(CR_Buffer));
				}
				
                // Encryption Subscribe CMD -> Buffer            
//                if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, CR_Buffer, Buffer, SizeOfBuffer))
                if (0 > iPN_StringEnc(gEncDecKey, CR_Buffer, Buffer, SizeOfBuffer))

                {
                    printf("WiPN_Subscribe - iPN_StringEnc: Subscribe Cmd Enc failed!\n");
                    free((void *)pServerDID);
                    free((void *)CR_Buffer);
                    free((void *)Buffer);
                    return WiPN_ERROR_iPNStringEncFailed;
                }
				
				UINT16 SizeOfSubsCmd = strlen(Buffer);
                printf("send cmd to SubscribeServer, SubscribeServerDID[%d]= %s. SizeOfSubsCmd= %d byte (Encrypted Size). sending...\n", j, &pServerDID[index], SizeOfSubsCmd);
                
                ret = NDT_PPCS_SendTo(&pServerDID[index], Buffer, SizeOfSubsCmd, g_SendToMode);
				
                if (0 > ret)
                {
                    printf("send cmd to SubscribeServer failed! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
                    printf("retry to send ...\n\n");
                    continue;
                }
                else
                {
					SubscribeServerHandle = ret;
                    printf("send cmd to SubscribeServer success! \n");
                    break;
                }
            }
            if (0 > SubscribeServerHandle)
            {
                printf("WiPN_Subscribe - Get SubscribeServerHandle failed! SubscribeServerDID[%d]= %s. ret= %d [%s]\n", j, &pServerDID[index], ret, getErrorCodeInfo(ret));
                break;
            }
        }
        else
        {
            printf("Waiting for SubscribeServer response, please wait ...\n");
			UINT16 SizeToRead = SizeOfBuffer;
            memset(Buffer, 0, SizeOfBuffer);
            
			ret = NDT_PPCS_RecvFrom(SubscribeServerHandle, Buffer, &SizeToRead, 3000);
            
            // Record RecvFrom return time
            time_t Time_ServerRet = time(NULL);         
			struct tm *ptm = localtime((const time_t *)&Time_ServerRet); 
            if (0 > ret)
            {
				printf("WiPN_Subscribe - NDT_PPCS_RecvFrom: SubscribeServerDID[%d]= %s. ret= %d. [%s]\n", j, &pServerDID[index], ret, getErrorCodeInfo(ret));
               	if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d]\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  
   
                // -26 -27 -3
                if ((NDT_ERROR_NoAckFromPushServer == ret || NDT_ERROR_NoAckFromDevice == ret || NDT_ERROR_TimeOut == ret) && 3 > retryCount_RecvFrom)
                {
                    retryCount_RecvFrom++;
                    continue;
                }
                else if (NDT_ERROR_RemoteHandleClosed == ret) // -36
				{
					printf("WiPN_Subscribe - SubscribeServer already call CloseHandle(). SubscribeServerDID[%d]= %s\n", j, &pServerDID[index]);
				} 
                break;
            }
            else
            {
                // Decrypt received data, Save in CR_Buffer             
//                if (0 > iPN_StringDnc(STRING_ENC_DEC_KEY, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                if (0 > iPN_StringDnc(gEncDecKey, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                {
                    printf("WiPN_Subscribe - NDT_PPCS_RecvFrom - iPN_StringDnc: RecvFrom Data Dec failed! SubscribeServerDID[%d]= %s.\n", j, &pServerDID[index]);
                    ret = WiPN_ERROR_iPNStringDncFailed;
                    break ;
                }
				
				printf("\nFrom SubscribeServer(Handle:%d, DID[%d]=%s): \n", SubscribeServerHandle, j, &pServerDID[index]);
				printf("Data: %s\n", CR_Buffer);
				printf("Size: %u byte\n", (UINT32)strlen(CR_Buffer));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  
              
                if (0 > GetStringItem(CR_Buffer, "RET", '&', pRETString, SizeOfRETString))
                {
                    printf("WiPN_Subscribe - Get RETString failed!\n");
                    ret = WiPN_ERROR_GetRETStringItemFailed;
                    break;
                }
                if (0 > GetStringItem(CR_Buffer, "UTCT", '&', pUTCTString, SizeOfUTCTString))
                {
                    printf("WiPN_Subscribe - Get UTCTString failed!\n");
                    ret =  WiPN_ERROR_GetUTCTStringItemFailed;
                    break;
                }
                else // g_Time_ServerRet Must be synchronized with the UTCTString time update
				{
					g_Time_ServerRet = Time_ServerRet;
				}        
                break;
            } // ret > 0
        } // Handle > 0
    } // while (1)
    
    if (0 <= SubscribeServerHandle)
	{
		NDT_PPCS_CloseHandle(SubscribeServerHandle);
		printf("NDT_PPCS_CloseHandle(%d) done!\n\n", SubscribeServerHandle);
	}
	if (pServerDID)
		free((void *)pServerDID);
    if (CR_Buffer)
		free((void *)CR_Buffer);	
    if (Buffer)
		free((void *)Buffer);
    
    return ret;
} // WiPN_Subscribe

//// WiPN_SubscribeByServer 
INT32 WiPN_SubscribeByServer(const CHAR *pServerString,
							const CHAR *pAES128Key,
							const CHAR *pSubscribeServerString,
							const CHAR *pSubCmd,
							CHAR *pRETString,
							UINT32 SizeOfRETString,
							CHAR *pUTCTString,
							UINT32 SizeOfUTCTString)
{
    if (   !pServerString
		|| !pAES128Key
		|| !pSubscribeServerString 
		|| !pSubCmd 
		|| !pRETString 
		|| !pUTCTString )
	{
		return WiPN_ERROR_InvalidParameter;
	}
    if (   0 == strlen(pServerString) 
		|| 0 == strlen(pAES128Key) 
		|| 0 == strlen(pSubscribeServerString) 
		|| 0 == strlen(pSubCmd) 
		|| 0 == strlen(pUTCTString)
		|| 0 == SizeOfRETString
		|| 0 == SizeOfUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
	//// -------------------------- Get Server DID -------------------------
	//// ServerDIDString: "NN,DID,DID,DID,...,DID"
    //// sample: "03,PPCS-123456-ETLRN,PPCS-123456-HYEXE,PPCS-123456-RFVMV"
	//// Get Number From ServerDIDString
    unsigned short NumberOfServer = 0;
    unsigned short SizeOfDID = 0;
    if (0 > pharse_number(pSubscribeServerString, &NumberOfServer, &SizeOfDID) || 0 == NumberOfServer)
	{
		return WiPN_ERROR_GetNumberFromSubscribeServerStringFailed;
	}
    SizeOfDID = (SizeOfDID/4+1)*4; // Keep enough distance between DID
    
    // Allocate memory space according to the number of SubscribeServerDID
    CHAR *pServerDID = (CHAR *)malloc(SizeOfDID*NumberOfServer);
    if (!pServerDID)
    {
        printf("WiPN_SubscribeByServer - Malloc ServerDID Buf failed!!\n");
        return WiPN_ERROR_MallocFailed;
    }
    memset(pServerDID, '\0', SizeOfDID*NumberOfServer);
    
    //// Get DID From ServerDIDString
    const CHAR *pDID = NULL;
    int i = 0;
    for ( ; i < NumberOfServer; i++)
    {
        pDID = pharse_DID(pSubscribeServerString, i);
        if (!pDID)
        {
            free((void *)pServerDID);
            return WiPN_ERROR_GetDIDFromSubscribeServerStringFailed;
        }
        memcpy(&pServerDID[SizeOfDID*i], pDID, strlen(pDID));
        //printf("SubscribeServerDID[%d]= %s\n", i, &pServerDID[SizeOfDID*i]);
        free((void*)pDID);
		pDID = NULL;
    }
	//// -------------------------------------------------------------------------
	
    // Calculate the size of SubscribeCmd
    UINT32 Length_Cmd = strlen(pSubCmd)+strlen("UTCT=0x&")+strlen(pUTCTString);
	
	// NDT can only send a maximum of 1280 bytes of data, according to the encryption algorithm, encryption data can not be more than 630 bytes
    UINT32 MaxSizeOfCmd = 630;
    if (MaxSizeOfCmd < Length_Cmd)
    {
        printf("WiPN_SubscribeByServer - Length Of SubscribeCmd is Exceed %d bytes!!\n", MaxSizeOfCmd);
        free((void *)pServerDID);
        return WiPN_ERROR_ExceedMaxSize;
    }
	
	// Save: 1.Cmd (Encryption before)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOf_CR_Buffer = (Length_Cmd/4+1)*4;
	char *CR_Buffer = (char *)malloc(SizeOf_CR_Buffer);
	if (!CR_Buffer)
	{
		printf("WiPN_SubscribeByServer - Malloc Cmd Buf failed!!\n");
		free((void *)pServerDID);
		return WiPN_ERROR_MallocFailed;
	}   
	
	// Save: 1.Cmd (After encryption)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOfBuffer = SizeOf_CR_Buffer*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("WiPN_SubscribeByServer - Malloc Buffer failed!!\n");
		free((void *)pServerDID);
		free((void *)CR_Buffer);
		return WiPN_ERROR_MallocFailed;
	}
	
	int ret = 0;
    int SubscribeServerHandle = -1;
    short retryCount_RecvFrom = 0;
	srand((UINT32)time(NULL));
	unsigned short PrintfFlag = 0;
	
    short index = 0;
    unsigned short j = abs(rand() % NumberOfServer);
    while (1)
    {
        if (0 > SubscribeServerHandle)
        {
            for (i = 0; i < NumberOfServer; i++)
            {
                j = (j + 1) % NumberOfServer;
                index = SizeOfDID * j;

                // Calculate UTCT time
                UINT32 UTCT_Subscribe = time(NULL) - g_Time_ServerRet + strtol(pUTCTString, NULL, 16);
                
                // Format Subscribe CMD to CR_Buffer
                memset(CR_Buffer, 0, SizeOf_CR_Buffer);
                SNPRINTF(CR_Buffer, SizeOf_CR_Buffer, "%sUTCT=0x%X&", pSubCmd, (UINT32)UTCT_Subscribe);
				if (0 == PrintfFlag)
				{
					PrintfFlag = 1;
					printf("SubscribeCmd= %s\nSubscribeCmdSize= %u byte (Not Encrypted Size)\n\n", CR_Buffer, (UINT32)strlen(CR_Buffer));
				}
				
                // Encryption Subscribe CMD, Save in Buffer               
//                if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, CR_Buffer, Buffer, SizeOfBuffer))
                if (0 > iPN_StringEnc(gEncDecKey, CR_Buffer, Buffer, SizeOfBuffer))
                {
                    printf("WiPN_SubscribeByServer - iPN_StringEnc: SubscribeCmd Enc failed!\n");
                    free((void *)pServerDID);
                    free((void *)CR_Buffer);
                    free((void *)Buffer);
                    return WiPN_ERROR_iPNStringEncFailed;
                }
				
				UINT16 SizeOfSubsCmd = strlen(Buffer);
                printf("send cmd to SubscribeServer, SubscribeServerDID[%d]= %s. SizeOfSubsCmd= %u byte (Encrypted Size). sending...\n", j, &pServerDID[index], SizeOfSubsCmd);
                
				// Send CMD to SubscribeServer
				ret = NDT_PPCS_SendToByServer(&pServerDID[index], Buffer, SizeOfSubsCmd, g_SendToMode, pServerString, pAES128Key);
                
                if (0 > ret)
                {
                    printf("send cmd to SubscribeServer failed! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
                    printf("retry to send ...\n\n");
                    continue;
                }
                else
                {
					SubscribeServerHandle = ret;
                    printf("send cmd to SubscribeServer success! \n");
                    break;
                }
            }
            if (0 > SubscribeServerHandle)
            {
                printf("WiPN_SubscribeByServer - Get SubscribeServerHandle failed! SubscribeServerDID[%d]= %s. ret= %d [%s]\n", j, &pServerDID[index], ret, getErrorCodeInfo(ret));
                break;
            }
        }
        else
        {
            printf("Waiting for SubscribeServer response, please wait ...\n");
			UINT16 SizeToRead = SizeOfBuffer;
            memset(Buffer, 0, SizeOfBuffer);
			
            ret = NDT_PPCS_RecvFrom(SubscribeServerHandle, Buffer, &SizeToRead, 3000);
            
            // Record RecvFrom return time
            time_t Time_ServerRet = time(NULL);          
			struct tm *ptm = localtime((const time_t *)&Time_ServerRet);  
            
			if (0 > ret)
            {      
				printf("WiPN_SubscribeByServer - NDT_PPCS_RecvFrom: SubscribeServerDID[%d]= %s. ret= %d. [%s]\n", j, &pServerDID[index], ret, getErrorCodeInfo(ret));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				} 
       
                // -26 -27 -3
                if ((NDT_ERROR_NoAckFromPushServer == ret || NDT_ERROR_NoAckFromDevice == ret || NDT_ERROR_TimeOut == ret) && 3 > retryCount_RecvFrom)
                {
                    retryCount_RecvFrom++;
                    continue;
                }
                else if (NDT_ERROR_RemoteHandleClosed == ret) // -36
				{
					printf("WiPN_SubscribeByServer - SubscribeServer already call CloseHandle(). SubscribeServerDID[%d]= %s.\n", j, &pServerDID[index]);
				} 
                break;
            }
            else
            {
                // Decrypt received data, Save in CR_Buffer                
//                if (0 > iPN_StringDnc(STRING_ENC_DEC_KEY, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                if (0 > iPN_StringDnc(gEncDecKey, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                {
                    printf("WiPN_SubscribeByServer - NDT_PPCS_RecvFrom - iPN_StringDnc: RecvFrom Data Dec failed! SubscribeServerDID[%d]= %s.\n", j, &pServerDID[index]);
                    ret = WiPN_ERROR_iPNStringDncFailed;
                    break ;
                }
				
				printf("\nFrom SubscribeServer(Handle:%d, DID[%d]=%s): \n", SubscribeServerHandle, j, &pServerDID[index]);
				printf("Data: %s\n", CR_Buffer);
				printf("Size: %u byte\n", (UINT32)strlen(CR_Buffer));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  
            
                if (0 > GetStringItem(CR_Buffer, "RET", '&', pRETString, SizeOfRETString))
                {
                    printf("WiPN_SubscribeByServer - Get RETString failed!\n");
                    ret = WiPN_ERROR_GetRETStringItemFailed;
                    break;
                }
                if (0 > GetStringItem(CR_Buffer, "UTCT", '&', pUTCTString, SizeOfUTCTString))
                {
                    printf("WiPN_SubscribeByServer - Get UTCTString failed!\n");
                    ret =  WiPN_ERROR_GetUTCTStringItemFailed;
                    break;
                }
                else // g_Time_ServerRet Must be synchronized with the UTCTString time update
				{
					g_Time_ServerRet = Time_ServerRet;
				}        
                break;
            } // ret > 0
        } // Handle > 0
    } // while (1)
    
    if (0 <= SubscribeServerHandle)
	{
		NDT_PPCS_CloseHandle(SubscribeServerHandle);
		printf("NDT_PPCS_CloseHandle(%d) done!\n\n", SubscribeServerHandle);
	}
	if (pServerDID)
		free((void *)pServerDID);
	if (CR_Buffer)
		free((void *)CR_Buffer);
	if (Buffer)
		free((void *)Buffer);
    
    return ret;
} // WiPN_SubscribeByServer


//// WiPN_UnSubscribe 
INT32 WiPN_UnSubscribe(const CHAR *pSubscribeServerString,
                     const CHAR *pSubCmd,
                     CHAR *pRETString,
                     UINT32 SizeOfRETString,
                     CHAR *pUTCTString,
                     UINT32 SizeOfUTCTString)
{
	if (   !pSubscribeServerString 
		|| !pSubCmd 
		|| !pRETString 
		|| !pUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
    if (   0 == strlen(pSubscribeServerString) 
		|| 0 == strlen(pSubCmd) 
		|| 0 == SizeOfRETString
		|| 0 == strlen(pUTCTString)
		|| 0 == SizeOfUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
	//// -------------------------- Get Server DID -------------------------
	//// ServerDIDString: "NN,DID,DID,DID,...,DID"
    //// sample: "03,PPCS-123456-ETLRN,PPCS-123456-HYEXE,PPCS-123456-RFVMV"
	//// Get Number From ServerDIDString
    unsigned short NumberOfServer = 0;
    unsigned short SizeOfDID = 0;
    if (0 > pharse_number(pSubscribeServerString, &NumberOfServer, &SizeOfDID) || 0 == NumberOfServer)
	{
		return WiPN_ERROR_GetNumberFromSubscribeServerStringFailed;
	} 
    SizeOfDID = (SizeOfDID/4+1)*4; // Keep enough distance between DID
    
    // Allocate memory space according to the number of SubscribeServerDID
    CHAR *pServerDID = (CHAR *)malloc(SizeOfDID*NumberOfServer);
    if (!pServerDID)
    {
        printf("WiPN_UnSubscribe - Malloc SubscribeServerDID Buf failed!!\n");
        return WiPN_ERROR_MallocFailed;
    }
    memset(pServerDID, '\0', SizeOfDID*NumberOfServer);
    
    //// Get DID From ServerDIDString
    const CHAR *pDID = NULL;
    int i = 0;
    for (; i < NumberOfServer; i++)
    {
        pDID = pharse_DID(pSubscribeServerString, i);
        if (!pDID)
        {
            free((void *)pServerDID);
            return WiPN_ERROR_GetDIDFromSubscribeServerStringFailed;
        }
        memcpy(&pServerDID[SizeOfDID*i], pDID, strlen(pDID));
        //printf("SubscribeServerDID[%d]= %s\n", i, &pServerDID[SizeOfDID*i]);
        free((void*)pDID);
		pDID = NULL;
    }
    //// ---------------------------------------------------------------------------
    // Calculate the size of UnSubscribeCmd
    UINT32 Length_Cmd = strlen(pSubCmd)+strlen("UTCT=0x&")+strlen(pUTCTString);
    
	// NDT can only send a maximum of 1280 bytes of data, according to the encryption algorithm, encryption data can not be more than 630 bytes
    UINT32 MaxSizeOfCmd = 630;
    if (MaxSizeOfCmd < Length_Cmd)
    {
        printf("WiPN_UnSubscribe - Length Of UnSubscribeCmd is Exceed %d bytes!!\n", MaxSizeOfCmd);
        free((void *)pServerDID);
        return WiPN_ERROR_ExceedMaxSize;
    }
	
	// Save: 1.Cmd (Encryption before)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOf_CR_Buffer = (Length_Cmd/4+1)*4;
	char *CR_Buffer = (char *)malloc(SizeOf_CR_Buffer);
	if (!CR_Buffer)
	{
		printf("WiPN_UnSubscribe - Malloc Cmd Buf failed!!\n");
		free((void *)pServerDID);
		return WiPN_ERROR_MallocFailed;
	}   
	
	// Save: 1.Cmd (After encryption)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOfBuffer = SizeOf_CR_Buffer*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("WiPN_UnSubscribe - Malloc Buffer failed!!\n");
		free((void *)pServerDID);
		free((void *)CR_Buffer);
		return WiPN_ERROR_MallocFailed;
	}
    	
    int ret = 0;
    int SubscribeServerHandle = -1;
	short retryCount_RecvFrom = 0;
	unsigned short PrintfFlag = 0;
	
	srand((UINT32)time(NULL));
    short index = 0;
    unsigned short j = abs(rand() % NumberOfServer);
    while (1)
    {
        if (0 > SubscribeServerHandle)
        {
            for (i = 0; i < NumberOfServer; i++)
            {
                j = (j + 1) % NumberOfServer;
                index = SizeOfDID * j;
                
                // Calculate UTCT time
                UINT32 UTCT_UnSubscribe = time(NULL) - g_Time_ServerRet + strtol(pUTCTString, NULL, 16);
                
                // Format UnSubscribe cmd -> CR_Buffer
                memset(CR_Buffer, 0, SizeOf_CR_Buffer);
                SNPRINTF(CR_Buffer, SizeOf_CR_Buffer, "%sUTCT=0x%X&", pSubCmd, (UINT32)UTCT_UnSubscribe); 
				if (0 == PrintfFlag)
				{
					PrintfFlag = 1;
					printf("UnSubscribeCmd= %s\nSize= %u byte (Not Encrypted Size)\n\n", CR_Buffer, (UINT32)strlen(CR_Buffer));
				}		
				
                // encryption UnSubscribe cmd -> Buffer           
                if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, CR_Buffer, Buffer, SizeOfBuffer))
                {
                    printf("WiPN_UnSubscribe - iPN_StringEnc failed!\n");
                    free((void *)pServerDID);
                    free((void *)CR_Buffer);
                    free((void *)Buffer);
                    return WiPN_ERROR_iPNStringEncFailed;
                }
				             
				UINT16 SizeOfUnSubsCmd = strlen(Buffer);
                printf("send cmd to SubscribeServer, SubscribeServerDID[%d]= %s. SizeOfUnSubsCmd= %d byte (Encrypted Size). sending...\n", j, &pServerDID[index], SizeOfUnSubsCmd);
				
                ret = NDT_PPCS_SendTo(&pServerDID[index], Buffer, SizeOfUnSubsCmd, g_SendToMode);
                
				if (0 > ret)
                {
                    printf("send cmd to SubscribeServer failed! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
                    printf("retry to send ...\n\n");
                    continue;
                }
                else
                {
					SubscribeServerHandle = ret;
                    printf("send cmd to SubscribeServer success! \n");
                    break;
                }
            }
            if (0 > SubscribeServerHandle)
            {
                printf("WiPN_UnSubscribe - Get SubscribeServerHandle failed! SubscribeServerDID[%d]= %s. ret= %d [%s]\n", j, &pServerDID[index], ret, getErrorCodeInfo(ret));
                break;
            }
        }
        else
        {
            printf("Waiting for SubscribeServer response, NDT_PPCS_RecvFrom ...\n");         
			UINT16 SizeToRead = SizeOfBuffer;
			memset(Buffer, 0, SizeOfBuffer);
			
            ret = NDT_PPCS_RecvFrom(SubscribeServerHandle, Buffer, &SizeToRead, 3000);
            
            // Record RecvFrom return time
            time_t Time_ServerRet = time(NULL);          			
			struct tm *ptm = localtime((const time_t *)&Time_ServerRet);

            if (0 > ret)
            {
				printf("WiPN_UnSubscribe - NDT_PPCS_RecvFrom: SubscribeServerDID[%d]= %s. ret= %d. [%s]\n", j, &pServerDID[index], ret, getErrorCodeInfo(ret));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				} 
              
                // -26 -27 -3
                if ((NDT_ERROR_NoAckFromPushServer == ret 
					|| NDT_ERROR_NoAckFromDevice == ret 
					|| NDT_ERROR_TimeOut == ret) 
					&& 3 > retryCount_RecvFrom)
                {
                    retryCount_RecvFrom++;
                    continue;
                }
                else if (NDT_ERROR_RemoteHandleClosed == ret) // -36
				{
					printf("WiPN_UnSubscribe - SubscribeServer already call CloseHandle().  SubscribeServerDID[%d]= %s\n", j, &pServerDID[index]);
				}  
                break;
            }
            else
            {
                // Decrypt received data, Save in CR_Buffer               
//                if (0 > iPN_StringDnc(STRING_ENC_DEC_KEY, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                if (0 > iPN_StringDnc(gEncDecKey, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                {
                    printf("WiPN_UnSubscribe - NDT_PPCS_RecvFrom: iPN_StringDnc failed! SubscribeServerDID[%d]= %s.\n", j, &pServerDID[index]);
                    ret = WiPN_ERROR_iPNStringDncFailed;
                    break ;
                }  
				
				printf("\nFrom SubscribeServer(Handle:%d, DID[%d]=%s): \n", SubscribeServerHandle, j, &pServerDID[index]);
				printf("Data: %s\n", CR_Buffer);
				printf("Size: %u byte\n", (UINT32)strlen(CR_Buffer));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}   

                if (0 > GetStringItem(CR_Buffer, "RET", '&', pRETString, SizeOfRETString))
                {
                    printf("WiPN_UnSubscribe - Get RETString failed!\n");
                    ret = WiPN_ERROR_GetRETStringItemFailed;
                    break;
                }
                if (0 > GetStringItem(CR_Buffer, "UTCT", '&', pUTCTString, SizeOfUTCTString))
                {
                    printf("WiPN_UnSubscribe - Get UTCTString failed!\n");
                    ret =  WiPN_ERROR_GetUTCTStringItemFailed;
                    break;
                }
                else // g_Time_ServerRet Must be synchronized with the UTCTString time update
				{
					g_Time_ServerRet = Time_ServerRet;
				}           
                break;
            } // ret > 0
        } // Handle > 0
    } // while (1)
    
    if (0 <= SubscribeServerHandle)
	{
		NDT_PPCS_CloseHandle(SubscribeServerHandle);
		printf("NDT_PPCS_CloseHandle(%d) done!\n\n", SubscribeServerHandle);
	}
	if (pServerDID)
		free((void *)pServerDID);
	if (CR_Buffer)
		free((void *)CR_Buffer);
	if (Buffer)
		free((void *)Buffer);
    
    return ret;
} // WiPN_UnSubscribe

//// WiPN_UnSubscribe 
INT32 WiPN_UnSubscribeByServer(	const CHAR *pServerString,
								const CHAR *pAES128Key,
								const CHAR *pSubscribeServerString,
								const CHAR *pSubCmd,
								CHAR *pRETString,
								UINT32 SizeOfRETString,
								CHAR *pUTCTString,
								UINT32 SizeOfUTCTString)
{
    if (   !pServerString
		|| !pAES128Key
		|| !pSubscribeServerString 
		|| !pSubCmd 
		|| !pRETString 
		|| !pUTCTString )
	{
		return WiPN_ERROR_InvalidParameter;
	}
    if (   0 == strlen(pServerString) 
		|| 0 == strlen(pAES128Key) 
		|| 0 == strlen(pSubscribeServerString) 
		|| 0 == strlen(pSubCmd) 
		|| 0 == strlen(pUTCTString)
		|| 0 == SizeOfRETString
		|| 0 == SizeOfUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
	//// -------------------------- Get Server DID -------------------------
	//// ServerDIDString: "NN,DID,DID,DID,...,DID"
    //// sample: "03,PPCS-123456-ETLRN,PPCS-123456-HYEXE,PPCS-123456-RFVMV"
	//// Get Number From ServerDIDString
    unsigned short NumberOfServer = 0;
    unsigned short SizeOfDID = 0;
    if (0 > pharse_number(pSubscribeServerString, &NumberOfServer, &SizeOfDID) || 0 == NumberOfServer)
	{
		return WiPN_ERROR_GetNumberFromSubscribeServerStringFailed;
	} 
    SizeOfDID = (SizeOfDID/4+1)*4; // Keep enough distance between DID
    
    // Allocate memory space according to the number of SubscribeServerDID
    CHAR *pServerDID = (CHAR *)malloc(SizeOfDID*NumberOfServer);
    if (!pServerDID)
    {
        printf("WiPN_UnSubscribeByServer - Malloc SubscribeServerDID Buf failed!!\n");
        return WiPN_ERROR_MallocFailed;
    }
    memset(pServerDID, '\0', SizeOfDID*NumberOfServer);
    
    //// Get DID From ServerDIDString
    const CHAR *pDID = NULL;
    int i = 0;
    for (; i < NumberOfServer; i++)
    {
        pDID = pharse_DID(pSubscribeServerString, i);
        if (!pDID)
        {
            free((void *)pServerDID);
            return WiPN_ERROR_GetDIDFromSubscribeServerStringFailed;
        }
        memcpy(&pServerDID[SizeOfDID*i], pDID, strlen(pDID));
        //printf("SubscribeServerDID[%d]= %s\n", i, &pServerDID[SizeOfDID*i]);
        free((void*)pDID);
		pDID = NULL;
    }
    //// ---------------------------------------------------------------------------
    // Calculate the size of UnSubscribeCmd
    UINT32 Length_Cmd = strlen(pSubCmd)+strlen("UTCT=0x&")+strlen(pUTCTString); 
	
	// NDT can only send a maximum of 1280 bytes of data, according to the encryption algorithm, encryption data can not be more than 630 bytes
    UINT32 MaxSizeOfCmd = 630;
    if (MaxSizeOfCmd < Length_Cmd)
    {
        printf("WiPN_UnSubscribeByServer - Length Of UnSubscribeCmd is Exceed %d bytes!!\n", MaxSizeOfCmd);
        free((void *)pServerDID);
        return WiPN_ERROR_ExceedMaxSize;
    }
	
	// Save: 1.Cmd (Encryption before)
	//		2.Response: Server response data (After decryption)
	UINT32 SizeOf_CR_Buffer = (Length_Cmd/4+1)*4;
	CHAR *CR_Buffer = (CHAR *)malloc(SizeOf_CR_Buffer);
	if (!CR_Buffer)
	{
		printf("WiPN_UnSubscribeByServer - Malloc UnSubscribeCmd Buf failed!!\n");
		free((void *)pServerDID);
		return WiPN_ERROR_MallocFailed;
	}   
	
	// Save: 1.Cmd (After encryption)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOfBuffer = SizeOf_CR_Buffer*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("WiPN_UnSubscribeByServer - Malloc Buffer failed!!\n");
		free((void *)pServerDID);
		free((void *)CR_Buffer);
		return WiPN_ERROR_MallocFailed;
	}
	
    int ret = 0;
    int SubscribeServerHandle = -1;
    short retryCount_RecvFrom = 0;
	srand((UINT32)time(NULL));
	unsigned short PrintfFlag = 0;
	
    short index = 0;
    unsigned short j = abs(rand() % NumberOfServer);
    while (1)
    {
        if (0 > SubscribeServerHandle)
        {
            for (i = 0; i < NumberOfServer; i++)
            {
                j = (j + 1) % NumberOfServer;
                index = SizeOfDID * j;
                
                // Calculate UTCT time
                UINT32 UTCT_UnSubscribe = time(NULL) - g_Time_ServerRet + strtol(pUTCTString, NULL, 16);
                
                // Format UnSubscribe cmd -> CR_Buffer
                memset(CR_Buffer, 0, SizeOf_CR_Buffer);
                SNPRINTF(CR_Buffer, SizeOf_CR_Buffer, "%sUTCT=0x%X&", pSubCmd, (UINT32)UTCT_UnSubscribe);
				if (0 == PrintfFlag)
				{
					PrintfFlag = 1;
					printf("UnSubscribeCmd= %s\nSize= %u byte (Not Encrypted Size)\n\n", CR_Buffer, (UINT32)strlen(CR_Buffer));
				}
				
                // encryption UnSubscribe cmd -> Buffer           
//                if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, CR_Buffer, Buffer, SizeOfBuffer))
                if (0 > iPN_StringEnc(gEncDecKey, CR_Buffer, Buffer, SizeOfBuffer))
                {
                    printf("WiPN_UnSubscribeByServer - iPN_StringEnc: UnSubscribe Cmd Enc failed!\n");
                    free((void *)pServerDID);
                    free((void *)CR_Buffer);
                    free((void *)Buffer);
                    return WiPN_ERROR_iPNStringEncFailed;
                }
				
				UINT16 SizeOfUnSubsCmd = strlen(Buffer);
                printf("send cmd to SubscribeServer, SubscribeServerDID[%d]= %s. SizeOfUnSubsCmd= %u byte (Encrypted Size). sending...\n", j, &pServerDID[index], SizeOfUnSubsCmd);
                          
				// Send CMD to SubscribeServer
				ret = NDT_PPCS_SendToByServer(&pServerDID[index], Buffer, SizeOfUnSubsCmd, g_SendToMode, pServerString, pAES128Key);
                
				if (0 > ret)
                {
                    printf("send cmd to SubscribeServer failed! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
                    printf("retry to send ...\n\n");
                    continue;
                }
                else
                {
					SubscribeServerHandle = ret;
                    printf("send cmd to SubscribeServer success! \n");
                    break;
                }
            }
            if (0 > SubscribeServerHandle)
            {
                printf("WiPN_UnSubscribeByServer - Get SubscribeServerHandle failed! SubscribeServerDID[%d]= %s. ret= %d [%s]\n", j, &pServerDID[index], ret, getErrorCodeInfo(ret));
                break;
            }
        }
        else
        {
            printf("Waiting for SubscribeServer response, please wait ...\n");        
            memset(Buffer, 0, SizeOfBuffer);
			UINT16 SizeToRead = SizeOfBuffer;
			
            ret = NDT_PPCS_RecvFrom(SubscribeServerHandle, Buffer, &SizeToRead, 3000);
            
            // Record RecvFrom return time
            time_t Time_ServerRet = time(NULL);         
			struct tm *ptm = localtime((const time_t *)&Time_ServerRet);
            if (0 > ret)
            {
				printf("WiPN_UnSubscribeByServer - NDT_PPCS_RecvFrom: SubscribeServerDID[%d]= %s. ret= %d. [%s]\n", j, &pServerDID[index], ret, getErrorCodeInfo(ret));             
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}   
               
                // -26 -27 -3
                if ((NDT_ERROR_NoAckFromPushServer == ret 
					|| NDT_ERROR_NoAckFromDevice == ret 
					|| NDT_ERROR_TimeOut == ret) 
					&& 3 > retryCount_RecvFrom)
                {
                    retryCount_RecvFrom++;
                    continue;
                }
                else if (NDT_ERROR_RemoteHandleClosed == ret) // -36
				{
					printf("WiPN_UnSubscribeByServer - SubscribeServer already call CloseHandle(), SubscribeServerDID[%d]= %s.\n", j, &pServerDID[index]);
				}  
                break;
            }
            else
            {
                // Decrypt received data, Save in CR_Buffer
//                if (0 > iPN_StringDnc(STRING_ENC_DEC_KEY, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                if (0 > iPN_StringDnc(gEncDecKey, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                {
                    printf("WiPN_UnSubscribeByServer - NDT_PPCS_RecvFrom - iPN_StringDnc:  RecvFrom Data Dec failed! SubscribeServerDID[%d]= %s.\n", j, &pServerDID[index]);
                    ret = WiPN_ERROR_iPNStringDncFailed;
                    break ;
                }
				
				printf("\nFrom SubscribeServer(Handle:%d, DID[%d]=%s): \n", SubscribeServerHandle, j, &pServerDID[index]);
				printf("Data: %s\n", CR_Buffer);
				printf("Size: %u byte\n", (UINT32)strlen(CR_Buffer));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  

                if (0 > GetStringItem(CR_Buffer, "RET", '&', pRETString, SizeOfRETString))
                {
                    printf("WiPN_UnSubscribeByServer - Get RETString failed!\n");
                    ret = WiPN_ERROR_GetRETStringItemFailed;
                    break;
                }
                if (0 > GetStringItem(CR_Buffer, "UTCT", '&', pUTCTString, SizeOfUTCTString))
                {
                    printf("WiPN_UnSubscribeByServer - Get UTCTString failed!\n");
                    ret =  WiPN_ERROR_GetUTCTStringItemFailed;
                    break;
                }
                else // g_Time_ServerRet Must be synchronized with the UTCTString time update
				{
					g_Time_ServerRet = Time_ServerRet;
				}           
                break;
            } // ret > 0
        } // Handle > 0
    } // while (1)
    
    if (0 <= SubscribeServerHandle)
	{
		NDT_PPCS_CloseHandle(SubscribeServerHandle);
		printf("NDT_PPCS_CloseHandle(%d) done!\n\n", SubscribeServerHandle);
	} 
	if (pServerDID)
		free((void *)pServerDID);
	if (CR_Buffer)
		free((void *)CR_Buffer);
	if (Buffer)
		free((void *)Buffer);

	
    return ret;
} // WiPN_UnSubscribe

INT32 WiPN_ChkSubscribe(const CHAR *pSubscribeServerString,
						 const CHAR *pChkSubCmd,
						 CHAR *pListString,
						 UINT32 SizeOfListString,
						 CHAR *pUTCTString,
						 UINT32 SizeOfUTCTString)
{
	if (   !pSubscribeServerString 
		|| !pChkSubCmd 
		|| !pListString 
		|| !pUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
    if (   0 == strlen(pSubscribeServerString) 
		|| 0 == strlen(pChkSubCmd) 
		|| 0 == SizeOfListString
		|| 0 == strlen(pUTCTString)
		|| 0 == SizeOfUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
	//// -------------------------- Get Server DID -------------------------
	//// ServerDIDString: "NN,DID,DID,DID,...,DID"
    //// sample: "03,PPCS-123456-ETLRN,PPCS-123456-HYEXE,PPCS-123456-RFVMV"
	//// Get Number From ServerDIDString
    unsigned short NumberOfServer = 0;
    unsigned short SizeOfDID = 0;
    if (0 > pharse_number(pSubscribeServerString, &NumberOfServer, &SizeOfDID) || 0 == NumberOfServer)
	{
		return WiPN_ERROR_GetNumberFromSubscribeServerStringFailed;
	}
    SizeOfDID = (SizeOfDID/4+1)*4; // Keep enough distance between DID
    
    // Allocate memory space according to the number of SubscribeServerDID
    CHAR *pServerDID = (CHAR *)malloc(SizeOfDID*NumberOfServer);
    if (!pServerDID)
    {
        printf("WiPN_ChkSubscribe - Malloc SubscribeServerDID Buf failed!!\n");
        return WiPN_ERROR_MallocFailed;
    }
    memset(pServerDID, '\0', SizeOfDID*NumberOfServer);
    
    //// Get DID From ServerDIDString
    const CHAR *pDID = NULL;
    int i = 0;
    for ( ; i < NumberOfServer; i++)
    {
        pDID = pharse_DID(pSubscribeServerString, i);
        if (!pDID)
        {
            free((void *)pServerDID);
            return WiPN_ERROR_GetDIDFromSubscribeServerStringFailed;
        }
        memcpy(&pServerDID[SizeOfDID*i], pDID, strlen(pDID));
        //printf("SubscribeServerDID[%d]= %s\n", i, &pServerDID[SizeOfDID*i]);
        free((void*)pDID);
		pDID = NULL;
    }
	//// -------------------------------------------------------------------------
	
	// NDT can only send a maximum of 1280 bytes of data, according to the encryption algorithm, encryption data can not be more than 630 bytes
    UINT32 MaxSizeOfCmd = 630;
    if (MaxSizeOfCmd < strlen(pChkSubCmd)+strlen("UTCT=0x&")+strlen(pUTCTString))
    {
        printf("WiPN_ChkSubscribe - Length Of ChkSubscribeCmd is Exceed %d bytes!!\n", MaxSizeOfCmd);
        free((void *)pServerDID);
        return WiPN_ERROR_ExceedMaxSize;
    }
	
	// Save: 1.Cmd (Encryption before)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOf_CR_Buffer = MaxSizeOfCmd;
	char *CR_Buffer = (char *)malloc(SizeOf_CR_Buffer);
	if (!CR_Buffer)
	{
		printf("WiPN_ChkSubscribe - Malloc ChkSubscribe Cmd Buf failed!!\n");
		free((void *)pServerDID);
		return WiPN_ERROR_MallocFailed;
	}   
	
	// Save: 1.Cmd (After encryption)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOfBuffer = SizeOf_CR_Buffer*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("WiPN_ChkSubscribe - Malloc Buffer failed!!\n");
		free((void *)pServerDID);
		free((void *)CR_Buffer);
		return WiPN_ERROR_MallocFailed;
	}
	
    int ret = 0;
    int SubscribeServerHandle = -1;
	short retryCount_RecvFrom = 0;
    srand((UINT32)time(NULL));
	unsigned short PrintfFlag = 0;
	
    short index = 0;
    unsigned short j = abs(rand() % NumberOfServer);
    while (1)
    {
        if (0 > SubscribeServerHandle)
        {
            for (i = 0; i < NumberOfServer; i++)
            {
                j = (j + 1) % NumberOfServer;
                index = SizeOfDID * j;

                // Calculate UTCT time
                UINT32 UTCT_ChkSubscribe = time(NULL) - g_Time_ServerRet + strtol(pUTCTString, NULL, 16);
                
                // Format Subscribe CMD -> CR_Buffer
                memset(CR_Buffer, 0, SizeOf_CR_Buffer);
                SNPRINTF(CR_Buffer, SizeOf_CR_Buffer, "%sUTCT=0x%X&", pChkSubCmd, (UINT32)UTCT_ChkSubscribe);
				if (0 == PrintfFlag)
				{
					PrintfFlag = 1;
					printf("ChkSubscribeCmd= %s\nChkSubscribeCmdSize= %u byte (Not Encrypted Size)\n\n", CR_Buffer, (UINT32)strlen(CR_Buffer));
				}
				
                // Encryption Subscribe CMD -> Buffer            
//                if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, CR_Buffer, Buffer, SizeOfBuffer))
                if (0 > iPN_StringEnc(gEncDecKey, CR_Buffer, Buffer, SizeOfBuffer))
                {
                    printf("WiPN_ChkSubscribe - iPN_StringEnc: ChkSubscribe Cmd Enc failed!\n");
                    free((void *)pServerDID);
                    free((void *)CR_Buffer);
                    free((void *)Buffer);
                    return WiPN_ERROR_iPNStringEncFailed;
                }
				
				UINT16 SizeOfChkSubsCmd = strlen(Buffer);
                printf("send cmd to SubscribeServer, SubscribeServerDID[%d]= %s. SizeOfChkSubsCmd= %d byte (Encrypted Size). sending...\n", j, &pServerDID[index], SizeOfChkSubsCmd);
                
                ret = NDT_PPCS_SendTo(&pServerDID[index], Buffer, SizeOfChkSubsCmd, g_SendToMode);
				
                if (0 > ret)
                {
                    printf("send cmd to SubscribeServer failed! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
                    printf("retry to send ...\n\n");
                    continue;
                }
                else
                {
					SubscribeServerHandle = ret;
                    printf("send cmd to SubscribeServer success! \n");
                    break;
                }
            }
            if (0 > SubscribeServerHandle)
            {
                printf("WiPN_ChkSubscribe - Get SubscribeServerHandle failed! SubscribeServerDID[%d]= %s. ret= %d [%s]\n", j, &pServerDID[index], ret, getErrorCodeInfo(ret));
                break;
            }
        }
        else
        {
            printf("Waiting for SubscribeServer response, please wait ...\n");
			UINT16 SizeToRead = SizeOfBuffer;
            memset(Buffer, 0, SizeOfBuffer);
            
			ret = NDT_PPCS_RecvFrom(SubscribeServerHandle, Buffer, &SizeToRead, 3000);
            
            // Record RecvFrom return time
            time_t Time_ServerRet = time(NULL);         
			struct tm *ptm = localtime((const time_t *)&Time_ServerRet); 
            if (0 > ret)
            {
				printf("WiPN_ChkSubscribe - NDT_PPCS_RecvFrom: SubscribeServerDID[%d]= %s. ret= %d. [%s]\n", j, &pServerDID[index], ret, getErrorCodeInfo(ret));
               	if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d]\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  
   
                // -26 -27 -3
                if ((NDT_ERROR_NoAckFromPushServer == ret 
					|| NDT_ERROR_NoAckFromDevice == ret 
					|| NDT_ERROR_TimeOut == ret) 
					&& 3 > retryCount_RecvFrom)
                {
                    retryCount_RecvFrom++;
                    continue;
                }
                else if (NDT_ERROR_RemoteHandleClosed == ret) // -36
				{
					printf("WiPN_ChkSubscribe - SubscribeServer already call CloseHandle(). SubscribeServerDID[%d]= %s\n", j, &pServerDID[index]);
				} 
                break;
            }
            else
            {
                
                // Decrypt received data, Save in CR_Buffer             
//                if (0 > iPN_StringDnc(STRING_ENC_DEC_KEY, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                if (0 > iPN_StringDnc(gEncDecKey, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                {
                    printf("WiPN_ChkSubscribe - NDT_PPCS_RecvFrom - iPN_StringDnc: RecvFrom Data Dec failed! SubscribeServerDID[%d]= %s.\n", j, &pServerDID[index]);
                    ret = WiPN_ERROR_iPNStringDncFailed;
                    break ;
                }
				
				printf("\nFrom SubscribeServer(Handle:%d, DID[%d]=%s): \n", SubscribeServerHandle, j, &pServerDID[index]);
				printf("Data: %s\n", CR_Buffer);
				printf("Size: %u byte\n", (UINT32)strlen(CR_Buffer));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  
              
                if (0 > GetStringItem(CR_Buffer, "List", '&', pListString, SizeOfListString))
                {
                    printf("WiPN_ChkSubscribe - Get ListString failed!\n");
                    ret = WiPN_ERROR_GetRETStringItemFailed;
                    break;
                }
                if (0 > GetStringItem(CR_Buffer, "UTCT", '&', pUTCTString, SizeOfUTCTString))
                {
                    printf("WiPN_ChkSubscribe - Get UTCTString failed!\n");
                    ret =  WiPN_ERROR_GetUTCTStringItemFailed;
                    break;
                }
                else // g_Time_ServerRet Must be synchronized with the UTCTString time update
				{
					g_Time_ServerRet = Time_ServerRet;
				}        
                break;
            } // ret > 0
        } // Handle > 0
    } // while (1)
    
    if (0 <= SubscribeServerHandle)
	{
		NDT_PPCS_CloseHandle(SubscribeServerHandle);
		printf("NDT_PPCS_CloseHandle(%d) done!\n\n", SubscribeServerHandle);
	}
	if (pServerDID)
		free((void *)pServerDID);
    if (CR_Buffer)
		free((void *)CR_Buffer);	
    if (Buffer)
		free((void *)Buffer);
    
    return ret;
} // WiPN_ChkSubscribe

INT32 WiPN_ChkSubscribeByServer(const CHAR *pServerString,
								const CHAR *pAES128Key,
								const CHAR *pSubscribeServerString,
								const CHAR *pChkSubCmd,
								CHAR *pListString,
								UINT32 SizeOfListString,
								CHAR *pUTCTString,
								UINT32 SizeOfUTCTString)
{
	if (   !pServerString
		|| !pAES128Key
		|| !pSubscribeServerString 
		|| !pChkSubCmd 
		|| !pListString 
		|| !pUTCTString )
	{
		return WiPN_ERROR_InvalidParameter;
	}
    if (   0 == strlen(pServerString) 
		|| 0 == strlen(pAES128Key) 
		|| 0 == strlen(pSubscribeServerString) 
		|| 0 == strlen(pChkSubCmd) 
		|| 0 == strlen(pUTCTString)
		|| 0 == SizeOfListString
		|| 0 == SizeOfUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
	//// -------------------------- Get Server DID -------------------------
	//// ServerDIDString: "NN,DID,DID,DID,...,DID"
    //// sample: "03,PPCS-123456-ETLRN,PPCS-123456-HYEXE,PPCS-123456-RFVMV"
	//// Get Number From ServerDIDString
    unsigned short NumberOfServer = 0;
    unsigned short SizeOfDID = 0;
    if (0 > pharse_number(pSubscribeServerString, &NumberOfServer, &SizeOfDID) || 0 == NumberOfServer)
	{
		return WiPN_ERROR_GetNumberFromSubscribeServerStringFailed;
	}
    SizeOfDID = (SizeOfDID/4+1)*4; // Keep enough distance between DID
    
    // Allocate memory space according to the number of SubscribeServerDID
    CHAR *pServerDID = (CHAR *)malloc(SizeOfDID*NumberOfServer);
    if (!pServerDID)
    {
        printf("WiPN_ChkSubscribeByServer - Malloc SubscribeServerDID Buf failed!!\n");
        return WiPN_ERROR_MallocFailed;
    }
    memset(pServerDID, '\0', SizeOfDID*NumberOfServer);
    
    //// Get DID From ServerDIDString
    const CHAR *pDID = NULL;
    int i = 0;
    for ( ; i < NumberOfServer; i++)
    {
        pDID = pharse_DID(pSubscribeServerString, i);
        if (!pDID)
        {
            free((void *)pServerDID);
            return WiPN_ERROR_GetDIDFromSubscribeServerStringFailed;
        }
        memcpy(&pServerDID[SizeOfDID*i], pDID, strlen(pDID));
        //printf("SubscribeServerDID[%d]= %s\n", i, &pServerDID[SizeOfDID*i]);
        free((void*)pDID);
		pDID = NULL;
    }
	//// -------------------------------------------------------------------------
	
	// NDT can only send a maximum of 1280 bytes of data, according to the encryption algorithm, encryption data can not be more than 630 bytes
    UINT32 MaxSizeOfCmd = 630;
    if (MaxSizeOfCmd < strlen(pChkSubCmd)+strlen("UTCT=0x&")+strlen(pUTCTString))
    {
        printf("WiPN_ChkSubscribeByServer - Length Of ChkSubscribeCmd is Exceed %d bytes!!\n", MaxSizeOfCmd);
        free((void *)pServerDID);
        return WiPN_ERROR_ExceedMaxSize;
    }
	
	// Save: 1.Cmd (Encryption before)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOf_CR_Buffer = MaxSizeOfCmd;
	char *CR_Buffer = (char *)malloc(SizeOf_CR_Buffer);
	if (!CR_Buffer)
	{
		printf("WiPN_ChkSubscribeByServer - Malloc ChkSubscribe Cmd Buf failed!!\n");
		free((void *)pServerDID);
		return WiPN_ERROR_MallocFailed;
	}   
	
	// Save: 1.Cmd (After encryption)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOfBuffer = SizeOf_CR_Buffer*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("WiPN_ChkSubscribeByServer - Malloc Buffer failed!!\n");
		free((void *)pServerDID);
		free((void *)CR_Buffer);
		return WiPN_ERROR_MallocFailed;
	}
	
    int ret = 0;
    int SubscribeServerHandle = -1;
	short retryCount_RecvFrom = 0;
    srand((UINT32)time(NULL));
	unsigned short PrintfFlag = 0;
	
    short index = 0;
    unsigned short j = abs(rand() % NumberOfServer);
    while (1)
    {
        if (0 > SubscribeServerHandle)
        {
            for (i = 0; i < NumberOfServer; i++)
            {
                j = (j + 1) % NumberOfServer;
                index = SizeOfDID * j;

                // Calculate UTCT time
                UINT32 UTCT_ChkSubscribe = time(NULL) - g_Time_ServerRet + strtol(pUTCTString, NULL, 16);
                
                // Format Subscribe CMD -> CR_Buffer
                memset(CR_Buffer, 0, SizeOf_CR_Buffer);
                SNPRINTF(CR_Buffer, SizeOf_CR_Buffer, "%sUTCT=0x%X&", pChkSubCmd, (UINT32)UTCT_ChkSubscribe);
				if (0 == PrintfFlag)
				{
					PrintfFlag = 1;
					printf("ChkSubscribeCmd= %s\nChkSubscribeCmdSize= %u byte (Not Encrypted Size)\n\n", CR_Buffer, (UINT32)strlen(CR_Buffer));
				}
				
                // Encryption Subscribe CMD -> Buffer            
//                if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, CR_Buffer, Buffer, SizeOfBuffer))
                if (0 > iPN_StringEnc(gEncDecKey, CR_Buffer, Buffer, SizeOfBuffer))
                {
                    printf("WiPN_ChkSubscribeByServer - iPN_StringEnc: ChkSubscribe Cmd Enc failed!\n");
                    free((void *)pServerDID);
                    free((void *)CR_Buffer);
                    free((void *)Buffer);
                    return WiPN_ERROR_iPNStringEncFailed;
                }
				
				UINT16 SizeOfChkSubsCmd = strlen(Buffer);
                printf("send cmd to SubscribeServer, SubscribeServerDID[%d]= %s. SizeOfChkSubsCmd= %d byte (Encrypted Size). sending...\n", j, &pServerDID[index], SizeOfChkSubsCmd);
                
				// Send CMD to SubscribeServer
				ret = NDT_PPCS_SendToByServer(&pServerDID[index], Buffer, SizeOfChkSubsCmd, g_SendToMode, pServerString, pAES128Key);
				
                if (0 > ret)
                {
                    printf("send cmd to SubscribeServer failed! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
                    printf("retry to send ...\n\n");
                    continue;
                }
                else
                {
					SubscribeServerHandle = ret;
                    printf("send cmd to SubscribeServer success! \n");
                    break;
                }
            }
            if (0 > SubscribeServerHandle)
            {
                printf("WiPN_ChkSubscribeByServer - Get SubscribeServerHandle failed! SubscribeServerDID[%d]= %s. ret= %d [%s]\n", j, &pServerDID[index], ret, getErrorCodeInfo(ret));
                break;
            }
        }
        else
        {
            printf("Waiting for SubscribeServer response, please wait ...\n");
			UINT16 SizeToRead = SizeOfBuffer;
            memset(Buffer, 0, SizeOfBuffer);
            
			ret = NDT_PPCS_RecvFrom(SubscribeServerHandle, Buffer, &SizeToRead, 3000);
            
            // Record RecvFrom return time
            time_t Time_ServerRet = time(NULL);         
			struct tm *ptm = localtime((const time_t *)&Time_ServerRet); 
            if (0 > ret)
            {
				printf("WiPN_ChkSubscribeByServer - NDT_PPCS_RecvFrom: SubscribeServerDID[%d]= %s. ret= %d. [%s]\n", j, &pServerDID[index], ret, getErrorCodeInfo(ret));
               	if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d]\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  
   
                // -26 -27 -3
                if ((NDT_ERROR_NoAckFromPushServer == ret || NDT_ERROR_NoAckFromDevice == ret || NDT_ERROR_TimeOut == ret) && 3 > retryCount_RecvFrom)
                {
                    retryCount_RecvFrom++;
                    continue;
                }
                else if (NDT_ERROR_RemoteHandleClosed == ret) // -36
				{
					printf("WiPN_ChkSubscribeByServer - SubscribeServer already call CloseHandle(). SubscribeServerDID[%d]= %s\n", j, &pServerDID[index]);
				} 
                break;
            }
            else
            {
                // Decrypt received data, Save in CR_Buffer             
//                if (0 > iPN_StringDnc(STRING_ENC_DEC_KEY, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                if (0 > iPN_StringDnc(gEncDecKey, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                {
                    printf("WiPN_ChkSubscribeByServer - NDT_PPCS_RecvFrom - iPN_StringDnc: RecvFrom Data Dec failed! SubscribeServerDID[%d]= %s.\n", j, &pServerDID[index]);
                    ret = WiPN_ERROR_iPNStringDncFailed;
                    break ;
                }
				
				printf("\nFrom SubscribeServer(Handle:%d, DID[%d]=%s): \n", SubscribeServerHandle, j, &pServerDID[index]);
				printf("Data: %s\n", CR_Buffer);
				printf("Size: %u byte\n", (UINT32)strlen(CR_Buffer));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  
              
                if (0 > GetStringItem(CR_Buffer, "List", '&', pListString, SizeOfListString))
                {
                    printf("WiPN_ChkSubscribeByServer - Get ListString failed!\n");
                    ret = WiPN_ERROR_GetRETStringItemFailed;
                    break;
                }
                if (0 > GetStringItem(CR_Buffer, "UTCT", '&', pUTCTString, SizeOfUTCTString))
                {
                    printf("WiPN_ChkSubscribeByServer - Get UTCTString failed!\n");
                    ret =  WiPN_ERROR_GetUTCTStringItemFailed;
                    break;
                }
                else // g_Time_ServerRet Must be synchronized with the UTCTString time update
				{
					g_Time_ServerRet = Time_ServerRet;
				}        
                break;
            } // ret > 0
        } // Handle > 0
    } // while (1)
    
    if (0 <= SubscribeServerHandle)
	{
		NDT_PPCS_CloseHandle(SubscribeServerHandle);
		printf("NDT_PPCS_CloseHandle(%d) done!\n\n", SubscribeServerHandle);
	}
	if (pServerDID)
		free((void *)pServerDID);
    if (CR_Buffer)
		free((void *)CR_Buffer);	
    if (Buffer)
		free((void *)Buffer);
    
    return ret;
} // WiPN_ChkSubscribeByServer

INT32 WiPN_ResetBadge(	const CHAR *pSubscribeServerString,
						const CHAR *pResetBadgeCmd,
						CHAR *pRETString,
						UINT32 SizeOfRETString,
						CHAR *pUTCTString,
						UINT32 SizeOfUTCTString)
{
	if (   !pSubscribeServerString 
		|| !pResetBadgeCmd 
		|| !pRETString 
		|| !pUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
    if (   0 == strlen(pSubscribeServerString) 
		|| 0 == strlen(pResetBadgeCmd) 
		|| 0 == SizeOfRETString
		|| 0 == strlen(pUTCTString)
		|| 0 == SizeOfUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
	char APIName[] = "WiPN_ResetBadge";
	//// -------------------------- Get Server DID -------------------------
	//// ServerDIDString: "NN,DID,DID,DID,...,DID"
    //// sample: "03,PPCS-123456-ETLRN,PPCS-123456-HYEXE,PPCS-123456-RFVMV"
	//// Get Number From ServerDIDString
    unsigned short NumberOfServer = 0;
    unsigned short SizeOfDID = 0;
    if (0 > pharse_number(pSubscribeServerString, &NumberOfServer, &SizeOfDID) || 0 == NumberOfServer)
	{
		return WiPN_ERROR_GetNumberFromSubscribeServerStringFailed;
	}
    SizeOfDID = (SizeOfDID/4+1)*4; // Keep enough distance between DID
    
    // Allocate memory space according to the number of SubscribeServerDID
    CHAR *pServerDID = (CHAR *)malloc(SizeOfDID*NumberOfServer);
    if (!pServerDID)
    {
        printf("%s - Malloc SubscribeServerDID Buf failed!!\n", APIName);
        return WiPN_ERROR_MallocFailed;
    }
    memset(pServerDID, '\0', SizeOfDID*NumberOfServer);
    
    //// Get DID From ServerDIDString
    const CHAR *pDID = NULL;
    int i = 0;
    for ( ; i < NumberOfServer; i++)
    {
        pDID = pharse_DID(pSubscribeServerString, i);
        if (!pDID)
        {
            free((void *)pServerDID);
            return WiPN_ERROR_GetDIDFromSubscribeServerStringFailed;
        }
        memcpy(&pServerDID[SizeOfDID*i], pDID, strlen(pDID));
        //printf("SubscribeServerDID[%d]= %s\n", i, &pServerDID[SizeOfDID*i]);
        free((void*)pDID);
		pDID = NULL;
    }
	//// -------------------------------------------------------------------------
    // Calculate the size of ResetBadgeCmd
    UINT32 Length_Cmd = strlen(pResetBadgeCmd)+strlen("UTCT=0x&")+strlen(pUTCTString); 
	
	// NDT can only send a maximum of 1280 bytes of data, according to the encryption algorithm, encryption data can not be more than 630 bytes
    UINT32 MaxSizeOfCmd = 630;
    if (MaxSizeOfCmd < Length_Cmd)
    {
        printf("%s - Length Of ResetBadgeCmd is Exceed %d bytes!!\n", APIName, MaxSizeOfCmd);
        free((void *)pServerDID);
        return WiPN_ERROR_ExceedMaxSize;
    }
	
	// Save: 1.Cmd (Encryption before)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOf_CR_Buffer = (Length_Cmd/4+1)*4;
	char *CR_Buffer = (char *)malloc(SizeOf_CR_Buffer);
	if (!CR_Buffer)
	{
		printf("%s - Malloc Cmd Buf failed!!\n", APIName);
		free((void *)pServerDID);
		return WiPN_ERROR_MallocFailed;
	}   
	
	// Save: 1.Cmd (After encryption)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOfBuffer = SizeOf_CR_Buffer*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("%s - Malloc Buffer failed!!\n", APIName);
		free((void *)pServerDID);
		free((void *)CR_Buffer);
		return WiPN_ERROR_MallocFailed;
	}
	
    int ret = 0;
    int SubscribeServerHandle = -1;
	short retryCount_RecvFrom = 0;
    srand((UINT32)time(NULL));
	unsigned short PrintfFlag = 0;
	
    short index = 0;
    unsigned short j = abs(rand() % NumberOfServer);
    while (1)
    {
        if (0 > SubscribeServerHandle)
        {
            for (i = 0; i < NumberOfServer; i++)
            {
                j = (j + 1) % NumberOfServer;
                index = SizeOfDID * j;

                // Calculate UTCT time
                UINT32 UTCT = time(NULL) - g_Time_ServerRet + strtol(pUTCTString, NULL, 16);
                
                // Format Subscribe CMD -> CR_Buffer
                memset(CR_Buffer, 0, SizeOf_CR_Buffer);
                SNPRINTF(CR_Buffer, SizeOf_CR_Buffer, "%sUTCT=0x%X&", pResetBadgeCmd, (UINT32)UTCT);
				if (0 == PrintfFlag)
				{
					PrintfFlag = 1;
					printf( "ResetBadgeCmd= %s\nResetBadgeCmdSize= %u byte (Not Encrypted Size)\n\n", CR_Buffer, (UINT32)strlen(CR_Buffer));
				}
				
                // Encryption Subscribe CMD -> Buffer            
//                if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, CR_Buffer, Buffer, SizeOfBuffer))
                if (0 > iPN_StringEnc(gEncDecKey, CR_Buffer, Buffer, SizeOfBuffer))
                {
                    printf("%s - iPN_StringEnc: ResetBadge Cmd Enc failed!\n", APIName);
                    free((void *)pServerDID);
                    free((void *)CR_Buffer);
                    free((void *)Buffer);
                    return WiPN_ERROR_iPNStringEncFailed;
                }
				
				UINT16 SizeOfCmd = strlen(Buffer);
                printf("send cmd to SubscribeServer, SubscribeServerDID[%d]= %s. SizeOfCmd= %d byte (Encrypted Size). sending...\n", j, &pServerDID[index], SizeOfCmd);
                
                ret = NDT_PPCS_SendTo(&pServerDID[index], Buffer, SizeOfCmd, g_SendToMode);
				
                if (0 > ret)
                {
                    printf("send cmd to SubscribeServer failed! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
                    printf("retry to send ...\n\n");
                    continue;
                }
                else
                {
					SubscribeServerHandle = ret;
                    printf("send cmd to SubscribeServer success! \n");
                    break;
                }
            }
            if (0 > SubscribeServerHandle)
            {
                printf("%s - Get SubscribeServerHandle failed! SubscribeServerDID[%d]= %s. ret= %d [%s]\n", APIName, j, &pServerDID[index], ret, getErrorCodeInfo(ret));
                break;
            }
        }
        else
        {
            printf("Waiting for SubscribeServer response, please wait ...\n");
			UINT16 SizeToRead = SizeOfBuffer;
            memset(Buffer, 0, SizeOfBuffer);
            
			ret = NDT_PPCS_RecvFrom(SubscribeServerHandle, Buffer, &SizeToRead, 3000);
            
            // Record RecvFrom return time
            time_t Time_ServerRet = time(NULL);         
			struct tm *ptm = localtime((const time_t *)&Time_ServerRet); 
            if (0 > ret)
            {
				printf("%s - NDT_PPCS_RecvFrom: SubscribeServerDID[%d]= %s. ret= %d. [%s]\n", APIName, j, &pServerDID[index], ret, getErrorCodeInfo(ret));
               	if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d]\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  
   
                // -26 -27 -3
                if ((NDT_ERROR_NoAckFromPushServer == ret 
					|| NDT_ERROR_NoAckFromDevice == ret 
					|| NDT_ERROR_TimeOut == ret) 
					&& 3 > retryCount_RecvFrom)
                {
                    retryCount_RecvFrom++;
                    continue;
                }
                else if (NDT_ERROR_RemoteHandleClosed == ret) // -36
				{
					printf("%s - SubscribeServer already call CloseHandle(). SubscribeServerDID[%d]= %s\n", APIName, j, &pServerDID[index]);
				} 
                break;
            }
            else
            {
                // Decrypt received data, Save in CR_Buffer             
                if (0 > iPN_StringDnc(gEncDecKey, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                {
                    printf("%s - NDT_PPCS_RecvFrom - iPN_StringDnc: RecvFrom Data Dec failed! SubscribeServerDID[%d]= %s.\n", APIName, j, &pServerDID[index]);
                    ret = WiPN_ERROR_iPNStringDncFailed;
                    break ;
                }
				
				printf("\nFrom SubscribeServer(Handle:%d, DID[%d]=%s): \n", SubscribeServerHandle, j, &pServerDID[index]);
				printf("Data: %s\n", CR_Buffer);
				printf("Size: %u byte\n", (UINT32)strlen(CR_Buffer));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  
				
				if (0 > GetStringItem(CR_Buffer, "RET", '&', pRETString, SizeOfRETString))
                {
                    printf("%s - Get RETString from RecvData failed!\n", APIName);
                    ret = WiPN_ERROR_GetRETStringItemFailed;
                    break;
                }
                if (0 > GetStringItem(CR_Buffer, "UTCT", '&', pUTCTString, SizeOfUTCTString))
                {
                    printf("%s - Get UTCTString from RecvData failed!\n", APIName);
                    ret =  WiPN_ERROR_GetUTCTStringItemFailed;
                    break;
                }
                else // g_Time_ServerRet Must be synchronized with the UTCTString time update
				{
					g_Time_ServerRet = Time_ServerRet;
				}        
                break;
            } // ret > 0
        } // Handle > 0
    } // while (1)
    
    if (0 <= SubscribeServerHandle)
	{
		NDT_PPCS_CloseHandle(SubscribeServerHandle);
		printf("NDT_PPCS_CloseHandle(%d) done!\n\n", SubscribeServerHandle);
	}
	if (pServerDID)
		free((void *)pServerDID);
    if (CR_Buffer)
		free((void *)CR_Buffer);	
    if (Buffer)
		free((void *)Buffer);
    
    return ret;
} // WiPN_ResetBadge

INT32 WiPN_ResetBadgeByServer(	const CHAR *pServerString,
								const CHAR *pAES128Key,
								const CHAR *pSubscribeServerString,
								const CHAR *pResetBadgeCmd,
								CHAR *pRETString,
								UINT32 SizeOfRETString,
								CHAR *pUTCTString,
								UINT32 SizeOfUTCTString)
{
	if (   !pServerString
		|| !pAES128Key
		|| !pSubscribeServerString 
		|| !pResetBadgeCmd 
		|| !pRETString 
		|| !pUTCTString )
	{
		return WiPN_ERROR_InvalidParameter;
	}
    if (   0 == strlen(pServerString) 
		|| 0 == strlen(pAES128Key) 
		|| 0 == strlen(pSubscribeServerString) 
		|| 0 == strlen(pResetBadgeCmd) 
		|| 0 == strlen(pUTCTString)
		|| 0 == SizeOfRETString
		|| 0 == SizeOfUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
	char APIName[] = "WiPN_ResetBadgeByServer";
	//// -------------------------- Get Server DID -------------------------
	//// ServerDIDString: "NN,DID,DID,DID,...,DID"
    //// sample: "03,PPCS-123456-ETLRN,PPCS-123456-HYEXE,PPCS-123456-RFVMV"
	//// Get Number From ServerDIDString
    unsigned short NumberOfServer = 0;
    unsigned short SizeOfDID = 0;
    if (0 > pharse_number(pSubscribeServerString, &NumberOfServer, &SizeOfDID) || 0 == NumberOfServer)
	{
		return WiPN_ERROR_GetNumberFromSubscribeServerStringFailed;
	}
    SizeOfDID = (SizeOfDID/4+1)*4; // Keep enough distance between DID
    
    // Allocate memory space according to the number of SubscribeServerDID
    CHAR *pServerDID = (CHAR *)malloc(SizeOfDID*NumberOfServer);
    if (!pServerDID)
    {
        printf("%s - Malloc SubscribeServerDID Buf failed!!\n", APIName);
        return WiPN_ERROR_MallocFailed;
    }
    memset(pServerDID, '\0', SizeOfDID*NumberOfServer);
    
    //// Get DID From ServerDIDString
    const CHAR *pDID = NULL;
    int i = 0;
    for ( ; i < NumberOfServer; i++)
    {
        pDID = pharse_DID(pSubscribeServerString, i);
        if (!pDID)
        {
            free((void *)pServerDID);
            return WiPN_ERROR_GetDIDFromSubscribeServerStringFailed;
        }
        memcpy(&pServerDID[SizeOfDID*i], pDID, strlen(pDID));
        //printf("SubscribeServerDID[%d]= %s\n", i, &pServerDID[SizeOfDID*i]);
        free((void*)pDID);
		pDID = NULL;
    }
	//// -------------------------------------------------------------------------
    // Calculate the size of ResetBadgeCmd
    UINT32 Length_Cmd = strlen(pResetBadgeCmd)+strlen("UTCT=0x&")+strlen(pUTCTString); 
	
	// NDT can only send a maximum of 1280 bytes of data, according to the encryption algorithm, encryption data can not be more than 630 bytes
    UINT32 MaxSizeOfCmd = 630;
    if (MaxSizeOfCmd < Length_Cmd)
    {
        printf("%s - Length Of ResetBadgeCmd is Exceed %d bytes!!\n", APIName, MaxSizeOfCmd);
        free((void *)pServerDID);
        return WiPN_ERROR_ExceedMaxSize;
    }
	
	// Save: 1.Cmd (Encryption before)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOf_CR_Buffer = (Length_Cmd/4+1)*4;
	char *CR_Buffer = (char *)malloc(SizeOf_CR_Buffer);
	if (!CR_Buffer)
	{
		printf("%s - Malloc ResetBadgeCmd Cmd Buf failed!!\n", APIName);
		free((void *)pServerDID);
		return WiPN_ERROR_MallocFailed;
	}   
	
	// Save: 1.Cmd (After encryption)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOfBuffer = SizeOf_CR_Buffer*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("%s - Malloc Buffer failed!!\n", APIName);
		free((void *)pServerDID);
		free((void *)CR_Buffer);
		return WiPN_ERROR_MallocFailed;
	}
	
    int ret = 0;
    int SubscribeServerHandle = -1;
	short retryCount_RecvFrom = 0;
    srand((UINT32)time(NULL));
	unsigned short PrintfFlag = 0;
	
    short index = 0;
    unsigned short j = abs(rand() % NumberOfServer);
    while (1)
    {
        if (0 > SubscribeServerHandle)
        {
            for (i = 0; i < NumberOfServer; i++)
            {
                j = (j + 1) % NumberOfServer;
                index = SizeOfDID * j;

                // Calculate UTCT time
                UINT32 UTCT = time(NULL) - g_Time_ServerRet + strtol(pUTCTString, NULL, 16);
                
                // Format Subscribe CMD -> CR_Buffer
                memset(CR_Buffer, 0, SizeOf_CR_Buffer);
                SNPRINTF(CR_Buffer, SizeOf_CR_Buffer, "%sUTCT=0x%X&", pResetBadgeCmd, (UINT32)UTCT);
				if (0 == PrintfFlag)
				{
					PrintfFlag = 1;
					printf("ResetBadgeCmd= %s\nResetBadgeCmdSize= %u byte (Not Encrypted Size)\n\n", CR_Buffer, (UINT32)strlen(CR_Buffer));
				}
				
                // Encryption Subscribe CMD -> Buffer            
                //                if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, CR_Buffer, Buffer, SizeOfBuffer))
                if (0 > iPN_StringEnc(gEncDecKey, CR_Buffer, Buffer, SizeOfBuffer))
                {
                    printf("%s - iPN_StringEnc: ResetBadgeCmd Enc failed!\n", APIName);
                    free((void *)pServerDID);
                    free((void *)CR_Buffer);
                    free((void *)Buffer);
                    return WiPN_ERROR_iPNStringEncFailed;
                }
				
				UINT16 SizeOfCmd = strlen(Buffer);
                printf("send cmd to SubscribeServer, SubscribeServerDID[%d]= %s. SizeOfCmd= %d byte (Encrypted Size). sending...\n", j, &pServerDID[index], SizeOfCmd);
                
				// Send CMD to SubscribeServer
				ret = NDT_PPCS_SendToByServer(&pServerDID[index], Buffer, SizeOfCmd, g_SendToMode, pServerString, pAES128Key);
				
                if (0 > ret)
                {
                    printf("send cmd to SubscribeServer failed! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
                    printf("retry to send ...\n\n");
                    continue;
                }
                else
                {
					SubscribeServerHandle = ret;
                    printf("send cmd to SubscribeServer success! \n");
                    break;
                }
            }
            if (0 > SubscribeServerHandle)
            {
                printf("%s - Get SubscribeServerHandle failed! SubscribeServerDID[%d]= %s. ret= %d [%s]\n", APIName, j, &pServerDID[index], ret, getErrorCodeInfo(ret));
                break;
            }
        }
        else
        {
            printf("Waiting for SubscribeServer response, please wait ...\n");
			UINT16 SizeToRead = SizeOfBuffer;
            memset(Buffer, 0, SizeOfBuffer);
            
			ret = NDT_PPCS_RecvFrom(SubscribeServerHandle, Buffer, &SizeToRead, 3000);
            
            // Record RecvFrom return time
            time_t Time_ServerRet = time(NULL);         
			struct tm *ptm = localtime((const time_t *)&Time_ServerRet); 
            if (0 > ret)
            {
				printf("%s - NDT_PPCS_RecvFrom: SubscribeServerDID[%d]= %s. ret= %d. [%s]\n", APIName, j, &pServerDID[index], ret, getErrorCodeInfo(ret));
               	if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d]\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  
   
                // -26 -27 -3
                if ((NDT_ERROR_NoAckFromPushServer == ret || NDT_ERROR_NoAckFromDevice == ret || NDT_ERROR_TimeOut == ret) && 3 > retryCount_RecvFrom)
                {
                    retryCount_RecvFrom++;
                    continue;
                }
                else if (NDT_ERROR_RemoteHandleClosed == ret) // -36
				{
					printf("%s - SubscribeServer already call CloseHandle(). SubscribeServerDID[%d]= %s\n", APIName, j, &pServerDID[index]);
				} 
                break;
            }
            else
            {
                // Decrypt received data, Save in CR_Buffer             
                if (0 > iPN_StringDnc(gEncDecKey, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                {
                    printf("%s - NDT_PPCS_RecvFrom - iPN_StringDnc: RecvFrom Data Dec failed! SubscribeServerDID[%d]= %s.\n", APIName, j, &pServerDID[index]);
                    ret = WiPN_ERROR_iPNStringDncFailed;
                    break ;
                }
				
				printf("\nFrom SubscribeServer(Handle:%d, DID[%d]=%s): \n", SubscribeServerHandle, j, &pServerDID[index]);
				printf("Data: %s\n", CR_Buffer);
				printf("Size: %u byte\n", (UINT32)strlen(CR_Buffer));
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  
              
                if (0 > GetStringItem(CR_Buffer, "RET", '&', pRETString, SizeOfRETString))
                {
                    printf("%s - Get RETString from RecvData failed!\n", APIName);
                    ret = WiPN_ERROR_GetRETStringItemFailed;
                    break;
                }
                if (0 > GetStringItem(CR_Buffer, "UTCT", '&', pUTCTString, SizeOfUTCTString))
                {
                    printf("%s - Get UTCTString from RecvData failed!\n", APIName);
                    ret =  WiPN_ERROR_GetUTCTStringItemFailed;
                    break;
                }
                else // g_Time_ServerRet Must be synchronized with the UTCTString time update
				{
					g_Time_ServerRet = Time_ServerRet;
				}        
                break;
            } // ret > 0
        } // Handle > 0
    } // while (1)
    
    if (0 <= SubscribeServerHandle)
	{
		NDT_PPCS_CloseHandle(SubscribeServerHandle);
		printf("NDT_PPCS_CloseHandle(%d) done!\n\n", SubscribeServerHandle);
	}
	if (pServerDID)
		free((void *)pServerDID);
    if (CR_Buffer)
		free((void *)CR_Buffer);	
    if (Buffer)
		free((void *)Buffer);
    
    return ret;
} // WiPN_ResetBadgeByServer
#endif // #if SUBSCRIBE_FLAG

#if POST_FLAG
//// WiPN_Post 
INT32 WiPN_Post(const CHAR *pPostServerString, 
				const CHAR *pCmd, 
				CHAR *pRETString, 
				UINT32 SizeOfRETString, 
				CHAR *pUTCTString, 
				UINT32 SizeOfUTCTString)
{
    if (!pPostServerString || !pCmd || !pRETString || !pUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
	if (   0 == strlen(pPostServerString) 
		|| 0 == strlen(pCmd)
		|| 0 == strlen(pUTCTString) 
		|| 0 == SizeOfRETString 
		|| 0 == SizeOfUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
		
	//// -------------------------- Get Server DID -------------------------	
    //// ServerDIDString: "NN,DID,DID,DID,...,DID"
    //// sample: "03,PPCS-123456-ETLRN,PPCS-123456-HYEXE,PPCS-123456-RFVMV"
	//// Get Number From ServerDIDString 
    unsigned short NumberOfServer = 0;
	unsigned short SizeOfDID = 0;
    if (0 > pharse_number(pPostServerString, &NumberOfServer, &SizeOfDID) || 0 == NumberOfServer)
	{
		return WiPN_ERROR_GetNumberFromPostServerStringFailed;
	}
	SizeOfDID = (SizeOfDID/4+1)*4; // Keep enough distance between DID
	
	// Allocate memory space according to the number of PostServerDID
    CHAR *pServerDID = (CHAR *)malloc(SizeOfDID*NumberOfServer);
    if (!pServerDID)
	{
		printf("WiPN_Post - Malloc PostServerDID Buf failed!!\n");
		return WiPN_ERROR_MallocFailed;
	}   
    memset(pServerDID, '\0', SizeOfDID*NumberOfServer);
    
    //// Get DID From ServerDIDString 
    const CHAR *pDID = NULL;
    int i = 0;
    for ( ; i < NumberOfServer; i++)
    {
        pDID = pharse_DID(pPostServerString, i);
        if (!pDID)
		{
			free((void *)pServerDID);
			return WiPN_ERROR_GetDIDFromPostServerStringFailed;
		}   
        memcpy(&pServerDID[SizeOfDID*i], pDID, strlen(pDID));    
        //printf("PostServerDID[%d]=%s\n", i, &pServerDID[SizeOfDID*i]);
		free((void*)pDID);
		pDID = NULL;
    }
	//// -----------------------------------------------------------------------
	
	// Calculate the space required for PostCmd
	UINT32 Length_Cmd = strlen(pCmd)+strlen("UTCT=0x&")+strlen(pUTCTString);
	
	// NDT can only send a maximum of 1280 bytes of data, according to the encryption algorithm, encryption data can not be more than 630 bytes
	int MaxSizeOfCmd = 630;
	if (MaxSizeOfCmd < Length_Cmd) 
	{
		printf("WiPN_Post - Length Of PostCmd is Exceed %d bytes!!\n", MaxSizeOfCmd);
		free((void *)pServerDID);
		return WiPN_ERROR_ExceedMaxSize;
	}
	
	// Save: 1.Cmd (Encryption before)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOf_CR_Buffer = (Length_Cmd/4+1)*4;
	char *CR_Buffer = (char *)malloc(SizeOf_CR_Buffer);
	if (!CR_Buffer)
	{
		printf("WiPN_Post - Malloc PostCmd Buf failed!!\n");
		free((void *)pServerDID);
		return WiPN_ERROR_MallocFailed;
	}   
	
	// Save: 1.Cmd (After encryption)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOfBuffer = SizeOf_CR_Buffer*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("WiPN_Post - Malloc Buffer failed!!\n");
		free((void *)pServerDID);
		free((void *)CR_Buffer);
		return WiPN_ERROR_MallocFailed;
	}  
	
	int ret = 0;
    int PostServerHandle = -1;
    short retryCount_RecvFrom = 0;
	unsigned short PrintfFlag = 0;
	
	//srand((UINT32)time(NULL));
	//unsigned short j = abs(rand() % NumberOfServer);
	short index = 0;
    while (1)
    {
        if (0 > PostServerHandle)
        {
            for (i = 0; i < NumberOfServer; i++)
            {             
				//j = (j + 1) % NumberOfServer;
				//index = SizeOfDID * j;
				index = SizeOfDID * i;
				
				// Calculate UTCT time
				UINT32 UTCT_Post = time(NULL) - g_Time_ServerRet + strtol(pUTCTString, NULL, 16);
				
				// Format PostCmd, Save in CR_Buffer
				memset(CR_Buffer, '\0', SizeOf_CR_Buffer);
				SNPRINTF(CR_Buffer, SizeOf_CR_Buffer, "UTCT=0x%X&%s", (UINT32)UTCT_Post, pCmd);
				if (0 == PrintfFlag)
				{
					PrintfFlag = 1;
					printf("PostCmd= %s\nPostCmdSize= %u byte (Not Encrypted Size)\n\n", CR_Buffer, (UINT32)strlen(CR_Buffer));
				}
					
				// encryption PostCmd, Save in Buffer 
//				if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, CR_Buffer, Buffer, SizeOfBuffer))
                if (0 > iPN_StringEnc(gEncDecKey, CR_Buffer, Buffer, SizeOfBuffer))
				{
					printf("WiPN_Post - iPN_StringEnc failed!\n");
					free((void *)pServerDID);
					free((void *)CR_Buffer);
					free((void *)Buffer);
					return WiPN_ERROR_iPNStringEncFailed;
				}
				
				UINT32 SizeOfPostCmd = strlen(Buffer);
				printf("send cmd to PostServer, PostServerDID[%d]= %s. SizeOfPostCmd= %d byte (Encrypted Size). sending...\n", i, &pServerDID[index], SizeOfPostCmd);

				//unsigned int tick = MyGetTickCount();	
				
				// Send Cmd to PostServer
				ret = NDT_PPCS_SendTo(	&pServerDID[index], 
										Buffer, 
										SizeOfPostCmd, 
										g_SendToMode);
				//debug_log(__FILE__, "NDT_PPCS_SendTo done! TimeUsed: %ld ms\n", MyGetTickCount() - tick);
				
				if (0 > ret)
                {
                    printf("send cmd to PostServer failed!! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
                    printf("retry to send ...\n\n");
					continue;
                }
                else
                {
					PostServerHandle = ret;
                    printf("send cmd to PostServer success!! \n");
                    break;
                }
            }
            if (0 > PostServerHandle)
            {
                printf("WiPN_Post - Get PostServerHandle failed! PostServerDID[%d]= %s. ret= %d [%s]\n", i, &pServerDID[index], ret, getErrorCodeInfo(ret));
                break;
            }
        }
        else
        {
            printf("Waiting for PostServer response, NDT_PPCS_RecvFrom ...\n");      
			UINT16 SizeToRead = SizeOfBuffer;
			memset(Buffer, 0, SizeOfBuffer); 	
			
			ret = NDT_PPCS_RecvFrom(PostServerHandle, Buffer, &SizeToRead, 3000);
			
			// Record RecvFrom return time
			time_t Time_ServerRet = time(NULL); 
			struct tm *ptm = localtime((const time_t *)&Time_ServerRet);
            if (0 > ret)
            {
				printf("WiPN_Post - NDT_PPCS_RecvFrom: PostServerDID[%d]= %s. ret= %d. [%s]\n", i, &pServerDID[index], ret, getErrorCodeInfo(ret));
                if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}  
        				             				
                // -26 -27 -3
                if ((NDT_ERROR_NoAckFromPushServer == ret || NDT_ERROR_NoAckFromDevice == ret || NDT_ERROR_TimeOut == ret) && 3 > retryCount_RecvFrom)
                {
                    retryCount_RecvFrom++;
                    continue;
                }
                else if (NDT_ERROR_RemoteHandleClosed == ret) // -36
				{
					printf("WiPN_Post - PostServer already call CloseHandle(), PostServerDID[%d]= %s.\n", i, &pServerDID[index]);
				}  
                break;
            }
            else
            {
				// Decrypt received data                         
//                if (0 > iPN_StringDnc(STRING_ENC_DEC_KEY, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                if (0 > iPN_StringDnc(gEncDecKey, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                {
                    printf("WiPN_Post - NDT_PPCS_RecvFrom - iPN_StringDnc: RecvFrom Data Dec failed! PostServerDID[%d]: %s\n", i, &pServerDID[index]);
                    ret = WiPN_ERROR_iPNStringDncFailed;
                    break ;
                } 
				
				printf("\nFrom PostServer(Handle:%d, DID[%d]=%s): \n", PostServerHandle, i, &pServerDID[index]);
				printf("Data: %s\n", CR_Buffer);
				//debug_log(__FILE__, "Size: %u byte\n", (UINT32)strlen(CR_Buffer)); 
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}				
 			
				// Get RET 
                if (0 > GetStringItem(CR_Buffer, "RET", '&', pRETString, SizeOfRETString))
                {
                    printf("WiPN_Post - Get RETString failed!\n");
                    ret = WiPN_ERROR_GetRETStringItemFailed;
                    break;
                }
				// Get UTCT 
                if (0 > GetStringItem(CR_Buffer, "UTCT", '&', pUTCTString, SizeOfUTCTString))
                {
                    printf("WiPN_Post - Get UTCTString failed!\n");
                    ret =  WiPN_ERROR_GetUTCTStringItemFailed;
                    break;
                }
				else // g_Time_ServerRet Must be synchronized with the UTCTString time update!!!
				{
					g_Time_ServerRet = Time_ServerRet;
				}
                break;
            } // ret > 0
        } // Handle > 0
    } // while (1)
    if (0 <= PostServerHandle)
	{		
		NDT_PPCS_CloseHandle(PostServerHandle);	
		printf("NDT_PPCS_CloseHandle(%d) done!\n\n", PostServerHandle);
	}
	if (pServerDID)
		free((void *)pServerDID);
	if (CR_Buffer)
		free((void *)CR_Buffer);
    if (Buffer)
		free((void *)Buffer);	
	
    return ret;
}

//// WiPN_PostByServer 
INT32 WiPN_PostByServer(const CHAR *pServerString,
						const CHAR *pAES128Key,
						const CHAR *pPostServerString, 
						const CHAR *pCmd, 
						CHAR *pRETString, 
						UINT32 SizeOfRETString, 
						CHAR *pUTCTString, 
						UINT32 SizeOfUTCTString)
{
    if (   !pServerString 
		|| !pAES128Key 
		|| !pPostServerString 
		|| !pCmd 
		|| !pRETString 
		|| !pUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
	if (   0 == strlen(pServerString)
		|| 0 == strlen(pAES128Key)
		|| 0 == strlen(pPostServerString) 
		|| 0 == strlen(pCmd)
		|| 0 == strlen(pUTCTString) 
		|| 0 == SizeOfRETString 
		|| 0 == SizeOfUTCTString)
	{
		return WiPN_ERROR_InvalidParameter;
	}
		
	//// -------------------------- Get Server DID -------------------------	
    //// ServerDIDString: "NN,DID,DID,DID,...,DID"
    //// sample: "03,PPCS-123456-ETLRN,PPCS-123456-HYEXE,PPCS-123456-RFVMV"
	//// Get Number From ServerDIDString 
    unsigned short NumberOfServer = 0;
	unsigned short SizeOfDID = 0;
    if (0 > pharse_number(pPostServerString, &NumberOfServer, &SizeOfDID) || 0 == NumberOfServer)
	{
		return WiPN_ERROR_GetNumberFromPostServerStringFailed;
	}
	SizeOfDID = (SizeOfDID/4+1)*4; // Keep enough distance between DID
	
	// Allocate memory space according to the number of PostServerDID
    CHAR *pServerDID = (CHAR *)malloc(SizeOfDID*NumberOfServer);
    if (!pServerDID)
	{
		printf("WiPN_PostByServer - Malloc PostServerDID Buf failed!!\n");
		return WiPN_ERROR_MallocFailed;
	}   
    memset(pServerDID, '\0', SizeOfDID*NumberOfServer);
    
    //// Get DID From ServerDIDString 
    const CHAR *pDID = NULL;
    int i = 0;
    for ( ; i < NumberOfServer; i++)
    {
        pDID = pharse_DID(pPostServerString, i);
        if (!pDID)
		{
			free((void *)pServerDID);
			return WiPN_ERROR_GetDIDFromPostServerStringFailed;
		}   
        memcpy(&pServerDID[SizeOfDID*i], pDID, strlen(pDID));    
        //printf("PostServerDID[%d]=%s\n", i, &pServerDID[SizeOfDID*i]);
		free((void*)pDID);
		pDID = NULL;
    }
	//// -----------------------------------------------------------------------

	// Calculate the space required for PostCmd
	UINT32 Length_Cmd = strlen(pCmd)+strlen("UTCT=0x&")+strlen(pUTCTString);
	
	// NDT can only send a maximum of 1280 bytes of data, according to the encryption algorithm, encryption data can not be more than 630 bytes
	int MaxSizeOfCmd = 630;
	if (MaxSizeOfCmd < Length_Cmd) 
	{
		printf("WiPN_PostByServer - Length Of PostCmd is Exceed %d bytes!!\n", MaxSizeOfCmd);
		free((void *)pServerDID);
		return WiPN_ERROR_ExceedMaxSize;
	}
	
	// Save: 1.Cmd (Encryption before)
	//		2.Response: Server response data (After decryption)
	UINT32 SizeOf_CR_Buffer = (Length_Cmd/4+1)*4;
	CHAR *CR_Buffer = (CHAR *)malloc(SizeOf_CR_Buffer);
	if (!CR_Buffer)
	{
		printf("WiPN_PostByServer - Malloc PostCmd Buf failed!!\n");
		free((void *)pServerDID);
		return WiPN_ERROR_MallocFailed;
	}   
		
	
	// Save: 1.Cmd (After encryption)
	//		2.Response: Server response data (After decryption)
	unsigned short SizeOfBuffer = SizeOf_CR_Buffer*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("WiPN_PostByServer - Malloc Buffer failed!!\n");
		free((void *)pServerDID);
		free((void *)CR_Buffer);
		return WiPN_ERROR_MallocFailed;
	}
	
	int ret = 0;
    int PostServerHandle = -1;
    short retryCount_RecvFrom = 0;
	unsigned short PrintfFlag = 0;	
	
	//srand((UINT32)time(NULL));
	//unsigned short j = abs(rand() % NumberOfServer);
	short index = 0;
    while (1)
    {
        if (0 > PostServerHandle)
        {
            for (i = 0; i < NumberOfServer; i++)
            {             
				//j = (j + 1) % NumberOfServer;
				//index = SizeOfDID * j;
				index = SizeOfDID * i;
				
				// Calculate UTCT time
				UINT32 UTCT_Post = time(NULL) - g_Time_ServerRet + strtol(pUTCTString, NULL, 16);
				
				// Format PostCmd, Save in CR_Buffer
				memset(CR_Buffer, '\0', SizeOf_CR_Buffer);
				SNPRINTF(CR_Buffer, SizeOf_CR_Buffer, "UTCT=0x%X&%s", (UINT32)UTCT_Post, pCmd);
				
				if (0 == PrintfFlag)
				{
					PrintfFlag = 1;
					printf("PostCmd= %s\nPostCmdSize= %u byte (Not Encrypted Size)\n\n", CR_Buffer, (UINT32)strlen(CR_Buffer));
				}
					
				// encryption PostCmd, Save in Buffer				
//				if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, CR_Buffer, Buffer, SizeOfBuffer))
                if (0 > iPN_StringEnc(gEncDecKey, CR_Buffer, Buffer, SizeOfBuffer))
				{
					printf("WiPN_PostByServer - iPN_StringEnc: PostCmd Enc failed!\n");
					free((void *)pServerDID);
					free((void *)CR_Buffer);
					free((void *)Buffer);
					return WiPN_ERROR_iPNStringEncFailed;
				}
				
				UINT32 SizeOfPostCmd = strlen(Buffer);
				printf("send cmd to PostServer, PostServerDID[%d]= %s. SizeOfPostCmd= %d byte (Encrypted Size). sending...\n", i, &pServerDID[index], SizeOfPostCmd);

				//unsigned int tick = MyGetTickCount();		
				
				// Send Cmd to PostServer
				ret = NDT_PPCS_SendToByServer(	&pServerDID[index], 
												Buffer, 
												SizeOfPostCmd, 
												g_SendToMode, 
												pServerString, 
												pAES128Key);
				//debug_log(__FILE__, "NDT_PPCS_SendToByServer done! TimeUsed: %ld ms\n", MyGetTickCount() - tick);
				
				if (0 > ret)
                {
                    printf("send cmd to PostServer failed!! ret= %d [%s]\n", ret, getErrorCodeInfo(ret));
                    printf("retry to send ...\n\n");
					continue;
                }
                else
                {
					PostServerHandle = ret;
                    printf("send cmd to PostServer success!! \n");
                    break;
                }
            } // for
            if (0 > PostServerHandle)
            {
                printf("WiPN_PostByServer - Get PostServerHandle failed! PostServerDID[%d]= %s. ret= %d [%s]\n", i, &pServerDID[index], ret, getErrorCodeInfo(ret));
                break;
            }
        }
        else
        {
            printf("Waiting for PostServer response, NDT_PPCS_RecvFrom ...\n");      
			memset(Buffer, 0, SizeOfBuffer); 
			UINT16 SizeToRead = SizeOfBuffer;		
			
			ret = NDT_PPCS_RecvFrom(PostServerHandle, Buffer, &SizeToRead, 3000);
		
			// Record RecvFrom return time
			time_t Time_ServerRet = time(NULL); 
			struct tm *ptm = localtime((const time_t *)&Time_ServerRet);			
            if (0 > ret)
            {
				printf("WiPN_PostByServer - NDT_PPCS_RecvFrom: PostServerDID[%d]= %s. ret= %d. [%s]\n", i, &pServerDID[index], ret, getErrorCodeInfo(ret));
				if (ptm)
				{
					printf("LocalTime: %d-%d-%d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}	
				
                // -26 -27 -3
                if ((NDT_ERROR_NoAckFromPushServer == ret || NDT_ERROR_NoAckFromDevice == ret || NDT_ERROR_TimeOut == ret) && 3 > retryCount_RecvFrom)
                {
                    retryCount_RecvFrom++;
                    continue;
                }
                else if (NDT_ERROR_RemoteHandleClosed == ret) // -36
				{
					printf("WiPN_PostByServer - PostServer already call CloseHandle().\n");
				}  
                break;
            }
            else
            {
				// Decrypt received data, Save in CR_Buffer                         
//                if (0 > iPN_StringDnc(STRING_ENC_DEC_KEY, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                if (0 > iPN_StringDnc(gEncDecKey, Buffer, CR_Buffer, SizeOf_CR_Buffer))
                {
                    printf("WiPN_PostByServer - NDT_PPCS_RecvFrom - iPN_StringDnc:  RecvFromData Dec failed! PostServerDID[%d]: %s\n", i, &pServerDID[index]);
                    ret = WiPN_ERROR_iPNStringDncFailed;
                    break ;
                }
				
				printf("\nFrom PostServer(Handle:%d, DID[%d]=%s): \n", PostServerHandle, i, &pServerDID[index]);
				printf("Data: %s\n", CR_Buffer);
				//debug_log(__FILE__, "Size: %u byte\n", (UINT32)strlen(CR_Buffer)); 
				if (ptm)
				{
					printf("LocalTime: %d-%02d-%02d %02d:%02d:%02d\n", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				}
				
				// Get RET
                if (0 > GetStringItem(CR_Buffer, "RET", '&', pRETString, SizeOfRETString))
                {
                    printf("WiPN_PostByServer - Get RETString failed!\n");
                    ret = WiPN_ERROR_GetRETStringItemFailed;
                    break;
                }
				// Get UTCT 
                if (0 > GetStringItem(CR_Buffer, "UTCT", '&', pUTCTString, SizeOfUTCTString))
                {
                    printf("WiPN_PostByServer - Get UTCTString failed!\n");
                    ret =  WiPN_ERROR_GetUTCTStringItemFailed;
                    break;
                }
				else // g_Time_ServerRet Must be synchronized with the UTCTString time update!!!
				{
					g_Time_ServerRet = Time_ServerRet;
				}
                break;
            } // ret > 0
        } // Handle > 0
    } // while (1)
    if (0 <= PostServerHandle)
	{			
		NDT_PPCS_CloseHandle(PostServerHandle);
		printf("NDT_PPCS_CloseHandle(%d) done!\n\n", PostServerHandle);
	}
	if (pServerDID)
		free((void *)pServerDID);
    if (CR_Buffer)
		free((void *)CR_Buffer);
	if (Buffer)
		free((void *)Buffer);
	
    return ret;
}
#endif // #if POST_FLAG


INT32 WiPN_SortForServerDID(CHAR *pServerDIDString) 
{
    if (!pServerDIDString || 0 == strlen(pServerDIDString))
	{
		return WiPN_ERROR_InvalidParameter;
	}
	
	//// -------------------------- Get Server DID -------------------------
	//// ServerDIDString: "NN,DID,DID,DID,...,DID"
    //// sample: "03,PPCS-123456-ETLRN,PPCS-123456-HYEXE,PPCS-123456-RFVMV"
	//// Get Number From ServerDIDString 
    unsigned short NumberOfServer = 0;
	unsigned short SizeOfDID = 0;
    if (0 > pharse_number(pServerDIDString, &NumberOfServer, &SizeOfDID) || 0 == NumberOfServer)
	{
		return WiPN_ERROR_GetNumberFromPostServerStringFailed;
	}
	SizeOfDID = (SizeOfDID/4+1)*4; // Keep enough distance between DID
	
	// Allocate memory space according to the number of ServerDID
    CHAR *pServerDID = (CHAR *)malloc(SizeOfDID*NumberOfServer);
    if (!pServerDID)
	{
		printf("WiPN_SortForServerDID - Malloc ServerDID Buf failed!!\n");
		return WiPN_ERROR_MallocFailed;
	}   
    memset(pServerDID, '\0', SizeOfDID*NumberOfServer);
	
	st_DID *pST_DID = (st_DID *)malloc(sizeof(st_DID)*NumberOfServer);
	if (!pST_DID)
	{
		printf("WiPN_SortForServerDID - Malloc DID Buf failed!!\n");
		return WiPN_ERROR_MallocFailed;
	}
	memset(pST_DID, 0, sizeof(pST_DID)*NumberOfServer);
	
    //// Get DID From ServerDIDString 
	const char *pDID = NULL;
    for (int i = 0; i < NumberOfServer; i++)
    {
        pDID = pharse_DID(pServerDIDString, i);
        if (!pDID)
		{
			free((void *)pServerDID);
			free((void *)pST_DID);
			return WiPN_ERROR_GetDIDFromPostServerStringFailed;
		}   
        memcpy(&pServerDID[SizeOfDID*i], pDID, strlen(pDID));    
		pST_DID[i].pDID = &pServerDID[SizeOfDID*i];
		//printf("ServerDID[%d]=%s\n", i, &pServerDID[SizeOfDID*i]);
        //printf("pST_DID[%d]=%s\n", i, pST_DID[i].pDID);
		free((void*)pDID);
		pDID = NULL;
    }
	//// -----------------------------------------------------------------------
	
	unsigned short SizeOf_UTCT_Buffer = (strlen("UTCT=0&")/4+1)*4;
	char *UTCT_Buffer = (char *)malloc(SizeOf_UTCT_Buffer);
	if (!UTCT_Buffer)
	{
		printf("WiPN_SortForServerDID - Malloc Cmd Buf failed!!\n");
		free((void *)pServerDID);
		free((void *)pST_DID);
		return WiPN_ERROR_MallocFailed;
	}   
	
	memset(UTCT_Buffer, '\0', SizeOf_UTCT_Buffer);
	SNPRINTF(UTCT_Buffer, SizeOf_UTCT_Buffer, "UTCT=0&");
	
	unsigned short SizeOfBuffer = (SizeOf_UTCT_Buffer+2)*2;
	char *Buffer = (char *)malloc(SizeOfBuffer);
	if (!Buffer)
	{
		printf("WiPN_SortForServerDID - Malloc Buffer failed!!\n");
		free((void *)pServerDID);
		free((void *)pST_DID);
		free((void *)UTCT_Buffer);
		return WiPN_ERROR_MallocFailed;
	}  
	// encryption Cmd, Save in Buffer 
	if (0 > iPN_StringEnc(STRING_ENC_DEC_KEY, UTCT_Buffer, Buffer, SizeOfBuffer))
	{
		printf("WiPN_SortForServerDID - iPN_StringEnc failed!\n");
		free((void *)pServerDID);
		free((void *)pST_DID);
		free((void *)UTCT_Buffer);
		free((void *)Buffer);
		return WiPN_ERROR_iPNStringEncFailed;
	}
	short index = 0;
	int ret = -1;
	for (int i = 0; i < NumberOfServer; i++)
	{
		index = SizeOfDID*i;
		unsigned int tick = MyGetTickCount();	
				
		ret = NDT_PPCS_SendTo(&pServerDID[index], 
							Buffer, 
							strlen(Buffer), 
							g_SendToMode);
		pST_DID[i].Time = MyGetTickCount() - tick;
		pST_DID[i].Handle = ret;
		printf("%d-NDT_PPCS_SendTo(%s) done! ret=%d, TimeUsed: %d ms\n", i, pST_DID[i].pDID, ret, pST_DID[i].Time);
		if (0 <= ret)
			NDT_PPCS_CloseHandle(ret);
	}
	int count = 0;
	for (int i = 0; i < NumberOfServer; i++)
	{
		if (0 <= pST_DID[i].Handle)
		{
			count++;
		}
	}
	if (0 < count)
	{
		// Sort the Server DID by time
		st_selectSort(pST_DID, NumberOfServer);
		for (int i = 0; i < NumberOfServer; i++)
		{
			printf("DID[%d]=%s, TimeUsed: %d ms\n", i, pST_DID[i].pDID, pST_DID[i].Time);
		}
		memset(pServerDIDString, 0, SIZE_PostServerString);
		SNPRINTF(pServerDIDString, SIZE_PostServerString, "%02d", NumberOfServer);
		for (int i = 0; i < NumberOfServer; i++)
		{
			char DID[SIZE_DID];
			memset(DID, 0, SIZE_DID);
			SNPRINTF(DID, SIZE_DID, ",%s", pST_DID[i].pDID);
			strcat(pServerDIDString, DID);
		}
	}	
	
	if (pServerDID)
		free((void *)pServerDID);
	if (pST_DID)
		free(pST_DID);
	if (UTCT_Buffer)
		free((void *)UTCT_Buffer);
    if (Buffer)
		free((void *)Buffer);	
	
	if (0 < count)
		return 0;
	else
		return ret;
}
