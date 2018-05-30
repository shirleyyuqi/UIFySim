/*
* Copyright (c) 2014,foscam 研发体系
* All rights reserved.
*
* 文件名称：FosTime.h
* 文件标识：
* 摘 要：系统时间相关
*		
*
* 当前版本：1.0
* 作 者：luoc
* 完成日期：2014年7月29日
*
* 取代版本：
* 原作者 ：
* 完成日期：
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
		static HR_U64 GetUTCTime_Ms();//UTC time, 单位 ms
		static HR_U64 GetLocalTime_Ms();//UTC time, 单位 ms

		static HR_U32 GetUTCTime_S(); //UTC time, 单位 s
		static HR_U32 GetLocalTime_S(); //本地时区时间, 单位 s
		static HR_U32 UTC2Localtime(HR_U32 t);
		static HR_U32 Localtime2UTC(HR_U32 t);

		static HR_U32 GetTimeZone();
	};
}

#endif
