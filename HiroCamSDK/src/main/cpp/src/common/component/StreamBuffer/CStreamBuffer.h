/*
 * CStreamBuff.h
 *
 *  Created on: Nov 15, 2011
 *      Author: foscam
 */

#ifndef CSTREAMBUFF_H_
#define CSTREAMBUFF_H_

#define MAX_CONSUMER		10

#include "BaseDataType.h"
#include "AutoLock.h"
#include <stdlib.h>  //linux下编译需要
using namespace std;
using namespace HiroCamSDK;
enum ShmAccessMode
{
	SHM_ACCESS_READ,
	SHM_ACCESS_WRITE_NONBLOCK, //非阻塞写
	SHM_ACCESS_WRITE_BLOCK,
};

class CStreamBuff
{
protected:
	class CBuffer
	{
	public:
		CBuffer(HR_S32 size, HR_S32 maxFrames);
		~CBuffer();
		HR_CHAR* GetBuffer();
		CBuffer* AddRefren(); 
		HR_S32 DelRefren();
		HR_S32 GetMaxFrame();
		HR_S32	GetSize();
		HiroCamSDK::MUTEX* GetMutex(){return &m_Mutex;};  //互斥锁，用来保证在任一时刻，只能有一个线程访问该对象。
		HiroCamSDK::MUTEX	m_Mutex;
	private:
		HR_CHAR*	m_cBuffer;
		HR_S32		m_iReference;
		HR_S32		m_iMaxFrame;
		HR_S32		m_iSize;
	//	HiroCamSDK::MUTEX	m_Mutex;
	};

public:
	CStreamBuff(HR_CHAR* name, HR_S32 accessMode, HR_S32 size, HR_S32 maxFrames);
	virtual ~CStreamBuff();
	
public:
	HR_S32 putFrame(const HR_CHAR* pFrameData, HR_S32 frameLen, HR_S32 writeMode, HR_S32 isKeyFrame); //写缓冲区
	HR_S32 getFrame(HR_CHAR* pFrameData, HR_S32 frameLen, HR_S32 *isKeyFrame);  //读缓冲区
	HR_S32 rPosUpdate();
	HR_S32 clearBuff(); //清空缓冲区
	HR_S32 GetLatestIFrame(HR_S32 userIdx, HR_S32 frameLen, HR_U32 oldWritePos, HR_BOOL bJumpEndFrame);
	HR_S32 GetUnreadCount();
	HR_S32 GetSavedMemSize();

	static HR_VOID InitalMutex();
	static HR_VOID ReleasMutest();
protected:
	HR_BOOL isWriteable(HR_S32 accessMode, HR_S32 frameLen);  //是否可写
	HR_BOOL adjustWritePtrBlockMode(HR_S32 frameLen);  
	HR_BOOL adjustRWPtrNoBlockMode(HR_S32 frameLen);
private:
	static  map<string, CBuffer*>  m_vBuffer;  
	string		m_sName;	 
	HR_CHAR*		mpShareMem;  //缓冲区
	MUTEX*			m_pMutex;
	HR_S32			mShmSize;  //缓冲区大小
	HR_S32			mMaxFrames;  //最大帧数
	HR_S32			mAccessMode;
	HR_S32			mUsrIndex;
public:
	static MUTEX	m_vMutex;
};

#endif /* CSTREAMBUFF_H_ */
