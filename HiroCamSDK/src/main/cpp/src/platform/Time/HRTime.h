/*
* Copyright (c) 2014,foscam �з���ϵ
* All rights reserved.
*
* �ļ����ƣ�FosTime.h
* �ļ���ʶ��
* ժ Ҫ��ϵͳʱ�����
*		
*
* ��ǰ�汾��1.0
* �� �ߣ�luoc
* ������ڣ�2014��7��29��
*
* ȡ���汾��
* ԭ���� ��
* ������ڣ�
*/
#ifndef __HIROCAMSDK_SRC_PLATFORM_TIME__
#define __HIROCAMSDK_SRC_PLATFORM_TIME__
#include "BaseDataType.h"
namespace HiroCamSDK
{
	class CTime
	{
	private:
		CTime(){};
		~CTime(){};
	public:
		static HR_U64 GetUTCTime_Ms();//UTC time, ��λ ms
		static HR_U64 GetLocalTime_Ms();//UTC time, ��λ ms

		static HR_U32 GetUTCTime_S(); //UTC time, ��λ s
		static HR_U32 GetLocalTime_S(); //����ʱ��ʱ��, ��λ s
		static HR_U32 UTC2Localtime(HR_U32 t);
		static HR_U32 Localtime2UTC(HR_U32 t);

		static HR_U32 GetTimeZone();
	};
}

#endif
