/*
* Copyright (c) 2018,hiro 研发体系
* All rights reserved.
*
* 文件名称：DataProc.h
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

#ifndef __SRC_NETSERVER_DATAPROC_DATAPROC_H__
#define __SRC_NETSERVER_DATAPROC_DATAPROC_H__
#include "BaseDataType.h"
#include "IParse.h"
#include "ISock.h"
namespace HiroCamSDK {
    namespace Protocol{

HR_S32 HRDataProc_Recv(IParse* pParse, ISock* pSock, HR_S32 channel);
HR_S32 HRDataProc_onParseData(IParse* pParse, ISock* pSock, HR_S32 channel, HR_S8* pBuf, HR_U32* pRcvLen, HR_U32 iMaxLen);
        
    }
}
#endif
