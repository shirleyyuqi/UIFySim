/*
* Copyright (c) 2014,foscam 研发体系
* All rights reserved.
*
* 文件名称：IParse.h
* 文件标识：
* 摘 要：协议解析基础类
*
* 当前版本：1.0
* 作 者：luoc
* 完成日期：2014年8月7日
*
* 取代版本：
* 原作者 ：
* 完成日期：
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


