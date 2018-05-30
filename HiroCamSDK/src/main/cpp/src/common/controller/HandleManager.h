/*
* Copyright (c) 2014,foscam ?§Ù????
* All rights reserved.
*
* ????????HandleManager.h
* ????????
* ? ???API Handle ????
*
* ????·Ú??1.0
* ?? ???luoc
* ????????2014??7??30??
*
* ????·Ú??
* ????? ??
* ????????
*/
#ifndef __HIROCAMSDK_SRC_CKONTROLLER_HANDLEMANAGER__
#define __HIROCAMSDK_SRC_CKONTROLLER_HANDLEMANAGER__
#include "BaseDataType.h"
#include "AutoLock.h"
#include "ApiImpl.h"
namespace HiroCamSDK
{
	class CHandleManager
	{
	public:
		CHandleManager();
		~CHandleManager();
	public:
		//???????
		static void ReleaseInstance();
		static CApiImpl *GetApiManager(HR_U32& handle);
        static HR_BOOL ReleaseApiManager(HR_U32 handle);
        
		static CApiImpl *CreateApiManager(HR_U32& handle);
		static void DeleteApiManager(HR_U32 handle);

	private:
		std::map<HR_U32, CApiImpl*>	m_hApi;
		
		HR_BOOL		    m_bInit;
		HR_U32			m_sIndexHandle;
		MUTEX			m_mMutex;
		int				m_iReleaseCount;
	};
}

#endif
