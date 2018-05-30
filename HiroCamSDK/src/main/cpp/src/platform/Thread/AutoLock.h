/*
* Copyright (c) 2014,foscam 研发体系
* All rights reserved.
*
* 文件名称：AutoLock.h
* 文件标识：
* 摘 要：自动加锁与解锁
*
* 当前版本：1.0
* 作 者：luoc
* 完成日期：2014年7月23日
*
* 取代版本：
* 原作者 ：
* 完成日期：
*/
#ifndef __HIROCAMSDK_SRC_PLATFORM_AUTOLOCK__
#define __HIROCAMSDK_SRC_PLATFORM_AUTOLOCK__
#include "BaseDataType.h"
typedef enum{
    SDKFUN_INIT,
    SDKFUN_ENTERFUN,
    SDKFUN_QUITFUN,
}FunState;

namespace HiroCamSDK
{
	class CAutoLock
	{
	public:
		CAutoLock(MUTEX& mutex);
		~CAutoLock();
	protected:
		MUTEX&	mMutex;
	};
}

class CAutoState{
public:
    CAutoState(FunState& state)
    : mState(state)
    {
        mState = SDKFUN_ENTERFUN;
    }
    
    ~CAutoState(){
        mState = SDKFUN_QUITFUN;
    }
private:
    FunState&   mState;
};

#endif
