/*
* Copyright (c) 2014,foscam 研发体系
* All rights reserved.
*
* 文件名称：BaseDataType.h
* 文件标识：
* 摘 要：基础函数库
*
* 当前版本：1.0
* 作 者：luoc
* 完成日期：2014年7月23日
*
* 取代版本：
* 原作者 ：
* 完成日期：
*/

#ifndef __HIROCAMSDK_SRC_PLATFORM_BASEFUNC__
#define __HIROCAMSDK_SRC_PLATFORM_BASEFUNC__
#include "BaseDataType.h"

namespace HiroCamSDK
{
	HR_BOOL GetDDnsServer(HR_CHAR *ddns1, HR_CHAR *ddns2);
	HR_BOOL StrNCpy(HR_CHAR *dst, HR_CHAR *src, HR_U32 dstlen);
	HR_S32  URL_Encode( const HR_CHAR * src, HR_S32  src_len,  HR_CHAR * dest, HR_S32  dest_len );
	HR_S32 URL_Decode(HR_CHAR *str, HR_S32 len);
	HR_S32 GetLastErr();
	HR_S32 CheckSpecialSymbol(HR_CHAR *str, HR_U32 len);
}
#endif
