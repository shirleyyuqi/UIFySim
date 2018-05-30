/*
* Copyright (c) 2014,foscam 研发体系
* All rights reserved.
*
* 文件名称：Thread.h
* 文件标识：
* 摘 要：线程
*
* 当前版本：1.0
* 作 者：luoc
* 完成日期：2014年7月23日
*
* 取代版本：
* 原作者 ：
* 完成日期：
*/

#ifndef __HIROCAMSDK_SRC_PLATFORM_THREAD__
#define __HIROCAMSDK_SRC_PLATFORM_THREAD__
#include "BaseDataType.h"
namespace HiroCamSDK
{
	typedef enum{
		THREADSTATE_NONE,
		THREADSTATE_CREATE,
		THREADSTATE_RUN,
		THREADSTATE_QUIT,
	}THREAD_STATE;
    
	class CThread
	{
	public:
		CThread();
		~CThread();
	public:
		HR_S32 CreateThread(HR_S32 isDetached, HR_S32 isSopeInSystem, HR_S32 stackSize, THREADFUNC func, HR_VOID *context);
		HR_VOID ExitThead(HR_U32 dwMilliseconds, HR_S32 code);
		HR_BOOL IsThreadRun();
	protected:
		HR_S32	 waitForSingleObject(HR_U32 dwMilliseconds);
		HR_BOOL closeHandle();
		HR_BOOL terminateThread(HR_S32 code);
		HR_THREADMETHOD_DEC(threadRun, lpParam);
	protected:
		HR_THREAD			m_hThread;
		HR_THREADID		    m_hThreadID;
		THREAD_STATE		m_eRelease;
		THREADFUNC			m_pThreadFunc;
		HR_VOID*            m_pContext;
	};
}

#endif
