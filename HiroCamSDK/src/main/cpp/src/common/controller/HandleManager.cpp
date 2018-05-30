#include "HandleManager.h"
#ifdef _TUTK
#include "RDTAPIs.h"
#include "IOTCAPIs.h"
#endif
namespace HiroCamSDK
{
	static CHandleManager gHandleManager;
	CHandleManager::CHandleManager()
	{
		m_sIndexHandle = 0;
		m_bInit = HR_FALSE;
		INIT_MUTEX(&m_mMutex);
	}

	CHandleManager::~CHandleManager()
	{
		RELEASE_MUTEX(&m_mMutex);
	}

	void CHandleManager::ReleaseInstance()
	{
		while(HR_TRUE)
		{
			{
				CAutoLock lock(gHandleManager.m_mMutex);
				std::map<HR_U32, CApiImpl*>::iterator it =  gHandleManager.m_hApi.begin();
				for (; it != gHandleManager.m_hApi.end();++it)
				{
					CApiImpl* pApi = (CApiImpl*)it->second;
					pApi->WillBeRelease();
				}
				if (gHandleManager.m_hApi.empty() && gHandleManager.m_iReleaseCount == 0)
				{
					break;
				}
			}
			
			SLEEP(40);
		}
	}

	CApiImpl* CHandleManager::GetApiManager(HR_U32& handle)
	{
		CAutoLock lock(gHandleManager.m_mMutex);
		if (handle <= gHandleManager.m_sIndexHandle && handle > 0)
		{
			std::map<HR_U32, CApiImpl*>::iterator it = gHandleManager.m_hApi.find(handle);
			if (it != gHandleManager.m_hApi.end())
			{
				if(it->second->GetARef() == 0) return NULL;

				it->second->Ref();
				return (CApiImpl*)(it->second);
			}
		}
		HRLOG_E("GetApiManager failed!! handle error!\n");
		return NULL;
	}

	CApiImpl* CHandleManager::CreateApiManager(HR_U32& handle)
	{
		CAutoLock lock(gHandleManager.m_mMutex);
		handle = ++gHandleManager.m_sIndexHandle;
		CApiImpl* pApi = new CApiImpl;
		gHandleManager.m_hApi[gHandleManager.m_sIndexHandle] = pApi;
		pApi->ARef();
		//HRLOG_I("Handel : %d, retainCount:%d ++ \n", handle, api);
		//++gHandleManager.m_iRef;
		return pApi;
	}

	void CHandleManager::DeleteApiManager(HR_U32 handle)
	{
		CApiImpl* pReleaseApi = NULL;
		{
			CAutoLock lock(gHandleManager.m_mMutex);
			std::map<HR_U32, CApiImpl*>::iterator it = gHandleManager.m_hApi.find(handle);
			if (it != gHandleManager.m_hApi.end())
			{
				CApiImpl* pApi = (CApiImpl*)it->second;
				//HRLOG_I("Handel : %d, retainA:%d, retain:%d +\n", handle, pApi->GetRef(), pApi->GetARef());
				if ( pApi->UnARef() == 0 && pApi->GetRef() == 0)
				{
					gHandleManager.m_hApi.erase(it);
					pReleaseApi = pApi;
					gHandleManager.m_iReleaseCount ++;
				}else if (pApi->GetARef() == 0){
					pApi->WillBeRelease();
				}
			}
		}

		if (pReleaseApi)
		{
			pReleaseApi->Release();
			delete pReleaseApi;

			CAutoLock lock(gHandleManager.m_mMutex);
			gHandleManager.m_iReleaseCount --;
		}
	}

	HR_BOOL CHandleManager::ReleaseApiManager(HR_U32 handle)
	{
		CApiImpl* pReleaseApi = NULL;
		{
			CAutoLock lock(gHandleManager.m_mMutex);
			std::map<HR_U32, CApiImpl*>::iterator it = gHandleManager.m_hApi.find(handle);
			if (it != gHandleManager.m_hApi.end())
			{
				CApiImpl* pApi = (CApiImpl*)it->second;
				//HRLOG_I("Handel : %d, retainA:%d, retain:%d +\n", handle, pApi->GetRef(), pApi->GetARef());
				if ( pApi->UnRef() == 0 && pApi->GetARef() == 0)
				{
					gHandleManager.m_hApi.erase(it);
					pReleaseApi = pApi;
					gHandleManager.m_iReleaseCount ++;
				}			
			}
		}
		if (pReleaseApi)
		{
			pReleaseApi->Release();
			delete pReleaseApi;

			CAutoLock lock(gHandleManager.m_mMutex);
			gHandleManager.m_iReleaseCount --;
			return HR_TRUE;
		}		
		return HR_FALSE;
	}
}


