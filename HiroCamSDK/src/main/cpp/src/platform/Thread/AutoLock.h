/*
* Copyright (c) 2014,foscam �з���ϵ
* All rights reserved.
*
* �ļ����ƣ�AutoLock.h
* �ļ���ʶ��
* ժ Ҫ���Զ����������
*
* ��ǰ�汾��1.0
* �� �ߣ�luoc
* ������ڣ�2014��7��23��
*
* ȡ���汾��
* ԭ���� ��
* ������ڣ�
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
