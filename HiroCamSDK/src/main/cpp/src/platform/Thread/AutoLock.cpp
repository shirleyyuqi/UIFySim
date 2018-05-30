#include "AutoLock.h"

namespace HiroCamSDK
{
	CAutoLock::CAutoLock(MUTEX& mutex)
		: mMutex(mutex)
	{
		LOCK_MUTEX(&mutex);
	}

	CAutoLock::~CAutoLock()
	{
		UNLOCK_MUTEX(&mMutex);
	}
}

