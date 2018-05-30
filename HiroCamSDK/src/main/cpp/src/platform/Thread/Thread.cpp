#include "Thread.h"

namespace HiroCamSDK
{
	CThread::CThread()
#ifndef _WP8
		: m_hThread(0)
#else
		: m_hThread()
#endif
		, m_hThreadID(0)
		, m_eRelease(THREADSTATE_NONE)
		, m_pContext(NULL)
		, m_pThreadFunc(NULL)
	{
	}

	CThread::~CThread()
	{
		m_pContext = NULL;
#ifndef _WP8
		m_hThread = 0;
#endif
	}

	HR_S32 CThread::CreateThread(HR_S32 isDetached, HR_S32 isScopeInSystem, HR_S32 stackSize, THREADFUNC func, HR_VOID *context)
	{
		m_pContext = context;
		m_pThreadFunc = func;
		if (m_pThreadFunc == NULL)
		{
			HRLOG_E("CreateThread failed: m_pThreadFunc is NULL!!\n");
			return HR_FALSE;
		}
#ifndef _WIN32
		int ret = 0;
		pthread_attr_t attr;
		ret = pthread_attr_init( &attr);
		if( ret != 0 )
		{
			//DEBUG_API_ERROR(COMMON_LIB, "pthread_attr_init()");
		}
		if( isDetached )
		{
			ret = pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED);
			if( ret != 0 )
			{
				//DEBUG_API_ERROR(COMMON_LIB, "pthread_attr_setdetachstate()");
			}
		}
		if( isScopeInSystem )
		{
			ret = pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
			if( ret != 0 )
			{
				//DEBUG_API_ERROR(COMMON_LIB, "pthread_attr_setscope()");
			}
		}
#if 0
		ret = pthread_attr_setstacksize( &attr, stackSize );
		if( ret != 0 )
		{
			//DEBUG_API_ERROR(COMMON_LIB, "pthread_attr_setstacksize()");
		}
#endif
		m_eRelease = THREADSTATE_CREATE;
		if ( pthread_create( &m_hThread, &attr, &threadRun, this) != 0 )
		{
			pthread_attr_destroy( &attr);
		}
#else

#if defined(_WP8) && !defined(_WIN_APP)
		m_eRelease = THREADSTATE_CREATE;
		HR_THREAD t(threadRun, this);
		m_hThread.swap(t);
		m_hThread.detach();
#else
		m_eRelease = THREADSTATE_CREATE;
		m_hThread = ::CreateThread(NULL, NULL, threadRun, this, NULL, &m_hThreadID);
#endif
#endif
		SLEEP(10);//释放对cpu的占用
		return 0;
	}

	HR_VOID CThread::ExitThead(HR_U32 dwMilliseconds, HR_S32 code)
	{
		if (waitForSingleObject(dwMilliseconds) == WAIT_TIMEOUT)
		{
			HRLOG_E("ExitThead failed! The error is WAIT_TIMEOUT!\n");
			terminateThread(code);
			closeHandle();
		}
		m_eRelease = THREADSTATE_NONE;
	}

	HR_S32	 CThread::waitForSingleObject(HR_U32 dwMilliseconds)
	{
        while (m_eRelease != THREADSTATE_NONE)
        {
            if (m_eRelease == THREADSTATE_QUIT)
            {
                return 1;
            }
            SLEEP(10);
        }
        return 1;
	}

	HR_BOOL CThread::closeHandle()
	{
#ifdef _WIN32
#ifndef _WP8
		CloseHandle(m_hThread);
		m_hThread = NULL;
		return HR_TRUE;
#else
		return HR_TRUE;
#endif
#else
		m_hThread = NULL;
		return HR_TRUE;
#endif		
	}

	HR_BOOL CThread::terminateThread(HR_S32 code)
	{
#ifdef _WIN32
#ifndef _WP8
		return (HR_BOOL)::TerminateThread(m_hThread, 0);
#else
		return HR_TRUE;
#endif
#else
		return HR_TRUE;
#endif	
	}

	HR_BOOL CThread::IsThreadRun()
	{
		return (HR_BOOL)(m_eRelease != THREADSTATE_NONE && m_eRelease != THREADSTATE_QUIT);
	}

	HR_THREADMETHOD(CThread::threadRun, lpParam)
	{
		CThread* pThis = static_cast<CThread*>(lpParam);
		pThis->m_eRelease = THREADSTATE_RUN;
		pThis->m_pThreadFunc(pThis->m_pContext);
		pThis->m_eRelease = THREADSTATE_QUIT;
		THREAD_RETURN(0);
	}
}


