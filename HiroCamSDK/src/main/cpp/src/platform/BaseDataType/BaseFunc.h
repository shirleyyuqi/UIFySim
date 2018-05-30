/*
* Copyright (c) 2014,foscam �з���ϵ
* All rights reserved.
*
* �ļ����ƣ�BaseDataType.h
* �ļ���ʶ��
* ժ Ҫ������������
*
* ��ǰ�汾��1.0
* �� �ߣ�luoc
* ������ڣ�2014��7��23��
*
* ȡ���汾��
* ԭ���� ��
* ������ڣ�
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
