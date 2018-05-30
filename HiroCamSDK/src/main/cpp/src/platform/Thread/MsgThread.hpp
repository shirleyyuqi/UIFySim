/*
* Copyright (c) 2014,foscam �з���ϵ
* All rights reserved.
*
* �ļ����ƣ�MsgThread.h
* �ļ���ʶ��
* ժ Ҫ����Ϣ�߳�
*
* ��ǰ�汾��1.0
* �� �ߣ�luoc
* ������ڣ�2014��8��5��
*
* ȡ���汾��
* ԭ���� ��
* ������ڣ�
*/
#ifndef __HIROCAMSDK_SRC_PLATFORM_MSGTHREAD__
#define __HIROCAMSDK_SRC_PLATFORM_MSGTHREAD__
#include "BaseDataType.h"
#include "Thread.h"
#include "AutoLock.h"
namespace HiroCamSDK{
	template<typename T, typename V>
	class CMsgThread
	{
	public:
		CMsgThread();
		~CMsgThread();
	public:
		HR_S32 CreateThread(HR_S32 isDetached, HR_S32 isScopeInSystem, HR_S32 stackSize, THREADFUNC func, HR_VOID *context);
		HR_VOID ExitThead(int timeout, int exitcode);
		HR_VOID ClearMsg();
		HR_VOID PostMsg(T& msg);
		HR_BOOL PeekMsg(T& msg);
		HR_VOID PeekEndMsg(T& msg);
		HR_BOOL IsThreadRun();
	private:
		V       	    m_Msg;
		MUTEX			m_Mutex;
		CThread			m_hThread;
	};

	template<typename T, typename V>
	CMsgThread<T, V>::CMsgThread()
	{
		INIT_MUTEX(&m_Mutex);
	}

	template<typename T, typename V>
	CMsgThread<T, V>::~CMsgThread()
	{
		RELEASE_MUTEX(&m_Mutex);
	}

	template<typename T, typename V>
	HR_BOOL CMsgThread<T, V>::IsThreadRun()
	{
		return m_hThread.IsThreadRun();
	}

	template<typename T, typename V>
	HR_S32 CMsgThread<T, V>::CreateThread(HR_S32 isDetached, HR_S32 isScopeInSystem, HR_S32 stackSize, THREADFUNC func, HR_VOID *context)
	{
		{
			CAutoLock lock(m_Mutex);
			m_Msg.clear();
		}
		return m_hThread.CreateThread(isDetached, isScopeInSystem, stackSize, func, context);
	}

	template<typename T, typename V>
	HR_VOID CMsgThread<T, V>::ExitThead(int timeout, int exitcode)
	{
		m_hThread.ExitThead(timeout, exitcode);
	}

	template<typename T, typename V>
	HR_VOID CMsgThread<T, V>::PostMsg(T& msg)
	{
		CAutoLock lock(m_Mutex);
		m_Msg.push_back(msg);
	}

	template<typename T, typename V>
	HR_VOID CMsgThread<T, V>::ClearMsg()
	{
		CAutoLock lock(m_Mutex);
		m_Msg.clear();
	}

	template<typename T, typename V>
	HR_BOOL CMsgThread<T, V>::PeekMsg(T& msg)
	{
		CAutoLock lock(m_Mutex);
		unsigned long size = m_Msg.size();
		if (size > 0)
		{
			msg = m_Msg[0];
			m_Msg.erase(m_Msg.begin());
			return HR_TRUE;
		}
		return HR_FALSE;
	}


	template<typename T, typename V>
	HR_VOID CMsgThread<T, V>::PeekEndMsg(T& msg)
	{
		typename V::iterator it;
		CAutoLock lock(m_Mutex);
		for(it = m_Msg.begin(); it < m_Msg.end(); )
		{
			if (msg == *it)
			{
				msg = *it;
				it = m_Msg.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}
#endif



