/*
* Copyright (c) 2014,foscam 研发体系
* All rights reserved.
*
* 文件名称：MsgThread.h
* 文件标识：
* 摘 要：消息
*
* 当前版本：1.0
* 作 者：luoc
* 完成日期：2014年8月5日
*
* 取代版本：
* 原作者 ：
* 完成日期：
*/
#ifndef __HIROCAMSDK_SRC_COMMON_CORE_MSGQUE_MSGQUE__
#define __HIROCAMSDK_SRC_COMMON_CORE_MSGQUE_MSGQUE__
#include "BaseDataType.h"
#include "Thread.h"
#include "AutoLock.h"
namespace HiroCamSDK{
	template<typename T, typename V>
	class CMsgQue
	{
	public:
		CMsgQue();
		~CMsgQue();
	public:
		HR_VOID ClearMsg();
		HR_VOID PostMsg(T& msg);
		HR_VOID PopEnd();
		HR_BOOL PeekMsg(T& msg);
		HR_BOOL PeekMsgNoLock(T& msg);
		HR_BOOL PeekEqMsg(T& msg);
		HR_VOID PeekEndMsg(T& msg);
		HR_BOOL FindMsg(T& msg, T **dstMsg);
		HR_VOID Lock();
		HR_VOID UnLock();
	private:
		V       	    m_Msg;
		MUTEX			m_Mutex;
	};

	template<typename T, typename V>
	CMsgQue<T, V>::CMsgQue()
	{
		INIT_MUTEX(&m_Mutex);
	}

	template<typename T, typename V>
	CMsgQue<T, V>::~CMsgQue()
	{
		RELEASE_MUTEX(&m_Mutex);
	}

	template<typename T, typename V>
	HR_VOID CMsgQue<T, V>::Lock()
	{
		LOCK_MUTEX(&m_Mutex);
	}

	template<typename T, typename V>
	HR_VOID CMsgQue<T, V>::UnLock()
	{
		UNLOCK_MUTEX(&m_Mutex);
	}

	template<typename T, typename V>
	HR_VOID CMsgQue<T, V>::PostMsg(T& msg)
	{
		CAutoLock lock(m_Mutex);
		m_Msg.push_back(msg);
	}

	template<typename T, typename V>
	HR_VOID CMsgQue<T, V>::PopEnd()
	{
		CAutoLock lock(m_Mutex);
		if(!m_Msg.empty())
		{
			m_Msg.pop_back();
		}
	}

	template<typename T, typename V>
	HR_VOID CMsgQue<T, V>::ClearMsg()
	{
		CAutoLock lock(m_Mutex);
		m_Msg.clear();
	}

	template<typename T, typename V>
	HR_BOOL CMsgQue<T, V>::PeekMsg(T& msg)
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
	HR_BOOL CMsgQue<T, V>::PeekMsgNoLock(T& msg)
	{
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
	HR_BOOL CMsgQue<T, V>::FindMsg(T& msg, T **dstMsg)
	{
		typename V::iterator it;
		CAutoLock lock(m_Mutex);
		for(it = m_Msg.begin(); it < m_Msg.end(); )
		{
			if (msg == *it)
			{
				*dstMsg = &(*it);
				return HR_TRUE;
			}
			else
			{
				++it;
			}
		}
		return HR_FALSE;
	}

	template<typename T, typename V>
	HR_BOOL CMsgQue<T, V>::PeekEqMsg(T& msg)
	{
		typename V::iterator it;
		CAutoLock lock(m_Mutex);
		for(it = m_Msg.begin(); it != m_Msg.end(); )
		{
			if (msg == *it)
			{
				msg = *it;
				it = m_Msg.erase(it);
				return HR_TRUE;
			}
			else
			{
				++it;
			}
		}
		return HR_FALSE;
	}

	template<typename T, typename V>
	HR_VOID CMsgQue<T, V>::PeekEndMsg(T& msg)
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
