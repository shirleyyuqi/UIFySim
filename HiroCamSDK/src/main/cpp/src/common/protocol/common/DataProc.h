/*
* Copyright (c) 2018,hiro �з���ϵ
* All rights reserved.
*
* �ļ����ƣ�DataProc.h
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
