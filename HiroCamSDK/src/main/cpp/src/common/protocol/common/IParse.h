/*
* Copyright (c) 2014,foscam �з���ϵ
* All rights reserved.
*
* �ļ����ƣ�IParse.h
* �ļ���ʶ��
* ժ Ҫ��Э�����������
*
* ��ǰ�汾��1.0
* �� �ߣ�luoc
* ������ڣ�2014��8��7��
*
* ȡ���汾��
* ԭ���� ��
* ������ڣ�
*/

#ifndef __HIROCAMSDK_SRC_COMMON_COMPONENT_PROTOCOL_COMMON_INTERFACE_IPARSE__
#define __HIROCAMSDK_SRC_COMMON_COMPONENT_PROTOCOL_COMMON_INTERFACE_IPARSE__
#include "BaseDataType.h"
#include "ISock.h"
namespace HiroCamSDK {
	namespace Protocol{
enum DATACHECK
{
	DATAERROR,
	DATAOK,
	DATANOCOMP,
	HEADNOCOMP,
};

class IParse {
public:
	virtual ~IParse(){};
public:
	virtual HR_S32 onParseMsg(ISock *pSock, HR_S8* pHead) = 0;
	virtual HR_S32 onCheckData(HR_S8* pHead, HR_U32 len) = 0;
	virtual HR_S32 onDataNoComp(HR_S8* pHead, HR_U32 Maxlen, HR_U32* RecvBufLen) = 0;
};
	}
} 
#endif /* __FOSSDK_SRC_COMMON_COMPONENT_PROTOCOL_COMMON_INTERFACE_IPARSE__ */


