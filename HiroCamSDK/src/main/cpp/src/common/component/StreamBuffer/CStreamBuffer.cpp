#include "CStreamBuffer.h"
#include <string>
#include <assert.h>
#include <iostream>
using namespace HiroCamSDK;

typedef enum{
	OFFSETFRAME_NONE = 0x0,
	OFFSETFRAME_IFRAME = 0x1,
	OFFSETFRAME_PFRAME = 0x2,
}OFFSETFRAME;

typedef struct tagShmProductorInfo
{
	HR_S32			offset;
	HR_U32			iFrameNO;
	HR_U32         wCount;
}ShmProductorInfo;    //生产者信息

typedef struct tagShmConsumerInfo
{
	HR_BOOL		bUse;
	OFFSETFRAME		offsetFrame;
	HR_S32			offset;
	HR_U32			iFrameNO;
	HR_U32         rCount;
}ShmConsumerInfo;  //消费者信息

typedef struct tagShmUsrInfo
{
	ShmProductorInfo productor;
	ShmConsumerInfo consumer[MAX_CONSUMER];
}ShmUsrInfo;    //使用者信息

typedef struct tagShmDataHead
{
	HR_S32			position;
	HR_S32			length;
	HR_S32			isKeyFrame;
	HR_U32			iFrameNO;
} ShmDataHead;  //数据头，每帧数据存一个头部

//申请存储使用者和数据帧头部信息的空间
#define SHARE_MEM_HEAD_SIZE(n) (sizeof(ShmUsrInfo) + n*sizeof(ShmDataHead))   
map<string, CStreamBuff::CBuffer*>  CStreamBuff::m_vBuffer;
MUTEX CStreamBuff::m_vMutex; 

CStreamBuff::CBuffer::CBuffer(HR_S32 size, HR_S32 maxFrames)
: m_iReference(0)
{
	INIT_MUTEX(&m_Mutex);//初始化锁
	m_iMaxFrame = maxFrames;
	m_iSize = size;
	m_cBuffer = (HR_CHAR*)malloc(SHARE_MEM_HEAD_SIZE(m_iMaxFrame)+size);
	memset(m_cBuffer, 0x00, SHARE_MEM_HEAD_SIZE(m_iMaxFrame));
}

CStreamBuff::CBuffer::~CBuffer()
{
	RELEASE_MUTEX(&m_Mutex);//释放锁
	if (m_cBuffer)
	{
		free(m_cBuffer);
		m_cBuffer = NULL;
	}
}

HR_CHAR* CStreamBuff::CBuffer::GetBuffer()
{
	return m_cBuffer;
}

//访问计数
CStreamBuff::CBuffer* CStreamBuff::CBuffer::AddRefren()
{
	LOCK_MUTEX(&m_Mutex);
	m_iReference ++;
	UNLOCK_MUTEX(&m_Mutex);
	return this;
}

HR_S32 CStreamBuff::CBuffer::DelRefren()
{
	LOCK_MUTEX(&m_Mutex);
	--m_iReference;
	UNLOCK_MUTEX(&m_Mutex);
	return m_iReference;
}

//获取缓冲区最大帧数
HR_S32 CStreamBuff::CBuffer::GetMaxFrame()   
{
	return m_iMaxFrame;
}

//获取缓冲区长度
HR_S32	CStreamBuff::CBuffer::GetSize() 
{
	return m_iSize;
}

HR_VOID CStreamBuff::InitalMutex()
{
	INIT_MUTEX(&m_vMutex);
}

HR_VOID CStreamBuff::ReleasMutest()
{
	RELEASE_MUTEX(&m_vMutex);
}

/*构造函数
  使用 name 表示缓冲区，同一name的缓冲区相同
*/
CStreamBuff::CStreamBuff(HR_CHAR* name, HR_S32 accessMode, HR_S32 size, HR_S32 maxFrames)
: m_sName(name)
, mpShareMem(NULL)
{
	CAutoLock CAutoLock(m_vMutex);
	
	if (m_vBuffer[name] == NULL)
	{
		mMaxFrames = maxFrames;
		m_vBuffer[name] = new CBuffer(size, mMaxFrames);
	}
	
	CBuffer* pBuf = m_vBuffer[name]->AddRefren();
	mMaxFrames = pBuf->GetMaxFrame();
	mShmSize = pBuf->GetSize();
	mpShareMem = pBuf->GetBuffer();
	m_pMutex = pBuf->GetMutex();
	mUsrIndex = 0;
    HRLOG_W("mpShardMem: %p\n", mpShareMem);
#if 1 
	mAccessMode = accessMode;
	ShmUsrInfo *pUsrInfo = (ShmUsrInfo* )mpShareMem;
	
	if( (accessMode == SHM_ACCESS_WRITE_NONBLOCK) ||
		(accessMode == SHM_ACCESS_WRITE_BLOCK) )  // write mode
	{
		pUsrInfo->productor.offset = 0;
		pUsrInfo->productor.wCount = 0;
		// Set all consumers' offset to 0
		for( HR_S32 i = 0; i < MAX_CONSUMER; i++ )
		{
			if( !pUsrInfo->consumer[i].bUse )
			{
				pUsrInfo->consumer[i].offset = 0;
				pUsrInfo->consumer[i].rCount = 0;
			}
		}

		memset(mpShareMem, 0x00, SHARE_MEM_HEAD_SIZE(mMaxFrames));
	}
	else if( accessMode == SHM_ACCESS_READ )
	{
		HR_BOOL isUsrExist = HR_FALSE;
		// Check if already in consumers list
		for( HR_S32 i = 0; i < MAX_CONSUMER; i++ )
		{
			if( !pUsrInfo->consumer[i].bUse )
			{
				pUsrInfo->consumer[i].offset = pUsrInfo->productor.offset;
				isUsrExist = HR_TRUE;
				mUsrIndex = i;
				pUsrInfo->consumer[i].bUse = HR_TRUE;
				pUsrInfo->consumer[i].rCount = pUsrInfo->productor.wCount;
				break;
			}
		}
		// Create a user
		if( !isUsrExist )
		{
			for( HR_S32 i = 0; i < MAX_CONSUMER; i++ )
			{
				if( pUsrInfo->consumer[i].bUse )
				{
					pUsrInfo->consumer[i].offset = pUsrInfo->productor.offset;
					pUsrInfo->consumer[i].rCount = pUsrInfo->productor.wCount;
					mUsrIndex = i;
					break;
				}
			}
		}
	}
#endif
}

CStreamBuff::~CStreamBuff()
{
	try{
	CAutoLock CAutoLock(m_vMutex);
	if (m_vBuffer[m_sName.c_str()])
	{
		if ( m_vBuffer[m_sName.c_str()]->DelRefren() == 0)
		{
			delete m_vBuffer[m_sName.c_str()];
			m_vBuffer.erase(m_sName.c_str());
		}
		else{
			if( (mAccessMode == SHM_ACCESS_WRITE_NONBLOCK) ||
				(mAccessMode == SHM_ACCESS_WRITE_BLOCK) )
			{

			}
			else
			{
				if(MAX_CONSUMER>mUsrIndex && 0<=mUsrIndex)
				{
					ShmUsrInfo *pUsrInfo = (ShmUsrInfo* )mpShareMem;
					pUsrInfo->consumer[mUsrIndex].bUse = HR_FALSE;
					pUsrInfo->consumer[mUsrIndex].offset = 0;
					pUsrInfo->consumer[mUsrIndex].rCount = 0;
				}
			}

		}
	}
	}catch(...){}
}

//写数据
HR_S32 CStreamBuff::putFrame(const HR_CHAR* pFrameData, HR_S32 frameLen, HR_S32 writeMode, HR_S32 isKeyFrame)

{

	HR_S32 writeBytes = 0;

	if( (pFrameData == NULL) || (frameLen <= 0) )	
	{
		HRLOG_W("Write data is null\n");
		return 0;
	}

	if ( NULL == mpShareMem ) 
	{
		HRLOG_E("mpShareMem is null\n");
		return -1; 
	}

	CAutoLock CAutoLock(*m_pMutex);
	HR_S32 head = 0, position = 0;
	ShmUsrInfo *pUsrInfo = (ShmUsrInfo* )mpShareMem;
	ShmDataHead* pDataHead = (ShmDataHead* )( mpShareMem + sizeof(ShmUsrInfo) );
	head = pUsrInfo->productor.offset % mMaxFrames;
	position = pDataHead[head].position; //数据写入位置
	
	if( (isWriteable(writeMode, frameLen)) && (frameLen < mShmSize/2) )
	{
		position = pDataHead[head].position;
		memcpy( mpShareMem+position, pFrameData, frameLen );
		pDataHead[head].length = frameLen;
		pDataHead[head].isKeyFrame = isKeyFrame;
		pDataHead[head].iFrameNO = ++pUsrInfo->productor.iFrameNO;
		pDataHead[ (head+1) % mMaxFrames].position = position + frameLen;
		pUsrInfo->productor.offset = (pUsrInfo->productor.offset + 1) % mMaxFrames;
		writeBytes = frameLen;
		pUsrInfo->productor.wCount++;
	}else{
		HRLOG_W("isWriteable(writeMode, frameLen):%d  (frameLen < mShmSize/2)%d  frameLen%d\n",isWriteable(writeMode, frameLen),(frameLen < mShmSize/2),frameLen);
	}
	return writeBytes;
}

//读数据
HR_S32 CStreamBuff::getFrame(HR_CHAR* pFrameData, HR_S32 frameLen, HR_S32 *isKeyFrame)
{
	HR_S32 ret = 0;

	if ( NULL == mpShareMem ) 
	{
		HRLOG_E("mpShareMem is null\n");
		return -1; 
	}
	
	CAutoLock CAutoLock(*m_pMutex);
	HR_S32 head = 0, tail = 0, position = 0, len = 0;
	ShmUsrInfo *pUsrInfo = (ShmUsrInfo* )mpShareMem;
	ShmDataHead* pDataHead = (ShmDataHead* )( mpShareMem + sizeof(ShmUsrInfo) );
	head = pUsrInfo->productor.offset % mMaxFrames;
	tail = pUsrInfo->consumer[mUsrIndex].offset % mMaxFrames; 
	OFFSETFRAME& offsetFrame = pUsrInfo->consumer[mUsrIndex].offsetFrame;
	if( head != tail )
	{
		len = pDataHead[tail].length;
		position = pDataHead[tail].position;
		*isKeyFrame = pDataHead[tail].isKeyFrame;
		if (frameLen >= len)
		{
			if (*isKeyFrame || offsetFrame == OFFSETFRAME_IFRAME)
			{
				offsetFrame = OFFSETFRAME_IFRAME;
			}
			else
			{
				pUsrInfo->consumer[mUsrIndex].iFrameNO = pDataHead[tail].iFrameNO;
				pUsrInfo->consumer[mUsrIndex].offset = (tail + 1 ) % mMaxFrames;
				pUsrInfo->consumer[mUsrIndex].rCount++;
				return 0;
			}

			memcpy( pFrameData, mpShareMem + position, len );
			ret = len;
			
			pUsrInfo->consumer[mUsrIndex].iFrameNO = pDataHead[tail].iFrameNO;
			pUsrInfo->consumer[mUsrIndex].offset = (tail + 1 ) % mMaxFrames;
			pUsrInfo->consumer[mUsrIndex].rCount++;
		}
		else
		{
			HRLOG_W("frameLen < len frameLen%d   len:%d\n",frameLen,len);
			return 0;
		}		
	}
	return ret;
}

//调整读者位置到写者当前位置
HR_S32 CStreamBuff::rPosUpdate() 
{
	HRLOG_I("rPosUpdate");
	if ( NULL == mpShareMem ) 
	{
		HRLOG_E("mpShareMem is null");
		return -1; 
	}
	
	if ( mAccessMode == SHM_ACCESS_READ ) 
	{
		CAutoLock CAutoLock(*m_pMutex);
		ShmUsrInfo *pUsrInfo = (ShmUsrInfo* )mpShareMem;
		pUsrInfo->consumer[mUsrIndex].offset = pUsrInfo->productor.offset;
		pUsrInfo->consumer[mUsrIndex].offsetFrame = OFFSETFRAME_NONE;
		pUsrInfo->consumer[mUsrIndex].rCount = pUsrInfo->productor.wCount;
		pUsrInfo->consumer[mUsrIndex].iFrameNO = pUsrInfo->productor.iFrameNO;

	}

	return 0;
}

//检查是否可写入
HR_BOOL CStreamBuff::isWriteable(HR_S32 accessMode, HR_S32 frameLen)
{
	if( accessMode == SHM_ACCESS_WRITE_BLOCK )
	{
		return adjustWritePtrBlockMode( frameLen );
	}
	else
	{
		return adjustRWPtrNoBlockMode( frameLen );
	}
}

HR_BOOL CStreamBuff::adjustWritePtrBlockMode(HR_S32 frameLen)
{
	HR_S32 i;
	HR_S32 head, tail;
	HR_S32 writePosition;
	HR_S32  readPosition;
	if ( NULL == mpShareMem ) 
	{
		HRLOG_E("mpShareMem is null\n");
		return HR_FALSE; 
	}

	ShmUsrInfo *pUsrInfo = (ShmUsrInfo* )mpShareMem;
	ShmDataHead* pDataHead = (ShmDataHead* )( mpShareMem + sizeof(ShmUsrInfo) );

	head = pUsrInfo->productor.offset % mMaxFrames; //写偏移
	writePosition = pDataHead[head].position; //写位置

	for( i = 0; i < MAX_CONSUMER; i++ )
	{
		if( !pUsrInfo->consumer[i].bUse )
		{
			continue;
		}
		
		tail = pUsrInfo->consumer[i].offset % mMaxFrames;
		if( head == tail )
		{
			continue;
		}
		
		if( (head + 1 ) % mMaxFrames == tail )
		{
			return HR_FALSE;
		}
		readPosition = pDataHead[tail].position;
		if( (writePosition <= readPosition) && (writePosition + frameLen >= readPosition) )
		{
			return HR_FALSE;
		}
		
		if( ( (writePosition + frameLen ) >= mShmSize ) )
		{
			if( (SHARE_MEM_HEAD_SIZE(mMaxFrames) + frameLen) > (HR_U32)readPosition )
			{
				return HR_FALSE;
			}
		}
	}

	if( ( (writePosition + frameLen) >= mShmSize ) ||
		( (HR_U32)writePosition < SHARE_MEM_HEAD_SIZE(mMaxFrames) ) )
	{
		pDataHead[head].position = SHARE_MEM_HEAD_SIZE(mMaxFrames);
	}

	return HR_TRUE;
}

HR_BOOL CStreamBuff::adjustRWPtrNoBlockMode(HR_S32 frameLen)
{
	HR_S32 i;
	HR_S32 head, tail, iFrame;
	HR_S32 writePosition = 0;
	HR_S32 writePosition_old = 0;
	//HR_S32 readPosition = 0;
	//HR_BOOL bPtrMoved = HR_FALSE;
	//HRLOG_I("adjustRWPtrNoBlockMode");
	if ( NULL == mpShareMem ) 
	{
		HRLOG_E("mpShareMem is null");
		return HR_FALSE; 
	}
	
	ShmUsrInfo *pUsrInfo = (ShmUsrInfo* )mpShareMem;
	ShmDataHead* pDataHead = (ShmDataHead* )( mpShareMem + sizeof(ShmUsrInfo) );
	head = pUsrInfo->productor.offset % mMaxFrames;  //当前写者偏移
	writePosition = pDataHead[head].position;	 //当前写入位置
	//一次性写入数据长度大于缓冲区长度或写入位置不合法
	if ( ((writePosition + frameLen) >= mShmSize) ||   
		((HR_U32)writePosition < SHARE_MEM_HEAD_SIZE(mMaxFrames)) )
	{
		writePosition_old = writePosition;
		writePosition = SHARE_MEM_HEAD_SIZE(mMaxFrames); //从缓冲区起始位置开始写
		pDataHead[head].position = writePosition;
	}
	
	for( i = 0; i < MAX_CONSUMER; i++ )
	{
		//bPtrMoved = HR_FALSE;
		if( !pUsrInfo->consumer[i].bUse )
		{
			continue;
		}

		tail = pUsrInfo->consumer[i].offset % mMaxFrames; //当前读者偏移

		/*
		    ________________________________________________________
			| 前                 缓冲区                        后  |
			--------------------------------------------------------
			iFrameNo 表示帧序号，右productor自增.
			1)head == tail 时, consumer[i].iFrameNo != productor.iFrameNo 则需要重新找帧.
			2)head != tail时
			  1. 写指针接近缓冲区的末尾 且 不够存放要写入的帧，需要写指针绕回头写，如果缓存区末尾有帧，则需重新找I帧。
			  2. 读指针在写指针后面 且 写指针写的长度覆盖到读指针，则需要重新找I帧
			  3. 读指针等于写指针 且 consumer[i].iFrameNo != productor.iFrameNo 则需要重新找帧.
		*/
		if ( ((head == tail) && pUsrInfo->consumer[i].iFrameNO != pUsrInfo->productor.iFrameNO) 
			|| ((head != tail) 
			   && (
			          (writePosition_old != 0 && writePosition_old <= pDataHead[tail].position)
			       || ((writePosition < pDataHead[tail].position) && (writePosition + frameLen >= pDataHead[tail].position))   
			       || (writePosition == pDataHead[tail].position && pUsrInfo->consumer[i].iFrameNO != pUsrInfo->productor.iFrameNO)
			      )
			   )
			)
		{
			HR_BOOL bJumpEndFrame = (HR_BOOL)(head != tail && writePosition_old != 0 && writePosition_old <= pDataHead[tail].position);
			iFrame = GetLatestIFrame( i, frameLen, writePosition_old, bJumpEndFrame);

			pUsrInfo->consumer[i].rCount = pUsrInfo->consumer[i].rCount+ pDataHead[iFrame].iFrameNO - pUsrInfo->consumer[i].iFrameNO;  //更新count

			pUsrInfo->consumer[i].iFrameNO = pDataHead[iFrame].iFrameNO;

			pUsrInfo->consumer[i].offset = iFrame;

		}
	}

	return HR_TRUE;
}
//获取下一个I 帧（发生写覆盖或跳帧时，读者读取的下一帧必须为I帧）
HR_S32 CStreamBuff::GetLatestIFrame(HR_S32 userIdx, HR_S32 frameLen, HR_U32 oldWritePos, HR_BOOL bJumpEndFrame)
{
	HR_S32 i,head, tail, iFrame;
	HR_U32 writePosition;
	HR_U32 readPosition;

	if ( NULL == mpShareMem ) 
	{
		HRLOG_E("mpShareMem is null\n");
		return -1; 
	}

	ShmUsrInfo *pUsrInfo = (ShmUsrInfo* )mpShareMem;
	ShmDataHead* pDataHead = (ShmDataHead* )( mpShareMem + sizeof(ShmUsrInfo) );
    pUsrInfo->consumer[userIdx].offsetFrame = OFFSETFRAME_PFRAME;
	tail = pUsrInfo->consumer[userIdx].offset + 1;  //当前读者偏移
	head = pUsrInfo->productor.offset;  //当前写者偏移
	writePosition = pDataHead[head].position; //当前写入位置
	iFrame = head; 
	
	for( i = 0; i < mMaxFrames-1; i++ )
	{
		if( tail  == head )
		{
			iFrame = head;  //原操作
			if( pDataHead[tail].isKeyFrame == 1 )
			{
				pUsrInfo->consumer[userIdx].offsetFrame = OFFSETFRAME_IFRAME;
			}
			else
			{
				pUsrInfo->consumer[userIdx].offsetFrame = OFFSETFRAME_PFRAME;
			}
			return iFrame;
		}
		if( pDataHead[tail].isKeyFrame == 1 && (head+1) != tail)  //当前读到的帧为 I 帧, 写者下一帧不为读者位置，解决连续I帧读不到情况
		{
			readPosition = pDataHead[tail].position; //当前读数据位置
			if (writePosition == readPosition)
			{ 
				pUsrInfo->consumer[userIdx].offsetFrame = OFFSETFRAME_PFRAME;
				return head;
			}
			else if( (writePosition < readPosition) &&
				(writePosition + frameLen >= readPosition) )
			{
				tail = (tail + 1) % mMaxFrames;
				continue;
			}
			else if (bJumpEndFrame && oldWritePos < readPosition)
			{
				tail = (tail + 1) % mMaxFrames;
				continue;
			}
			
			iFrame = tail;
			pUsrInfo->consumer[userIdx].offsetFrame = OFFSETFRAME_IFRAME;
			break;
		}
		tail = (tail + 1) % mMaxFrames;

	}

	return iFrame;
}
//写者重置所有读者读取位置为当前写者的位置
HR_S32 CStreamBuff::clearBuff()
{
	if ( NULL == mpShareMem ) 
	{
		HRLOG_E("mpShareMem is null\n");
		return -1; 
	}

	if ( mAccessMode == SHM_ACCESS_WRITE_NONBLOCK || mAccessMode == SHM_ACCESS_WRITE_BLOCK ) 
	{
		CAutoLock CAutoLock(*m_pMutex);
	//	memset(mpShareMem + sizeof(ShmUsrInfo) , 0, mMaxFrames * sizeof(ShmDataHead));

		ShmUsrInfo *pUsrInfo = (ShmUsrInfo* )mpShareMem;
		for ( HR_S32 i = 0; i < MAX_CONSUMER; i++ )
		{
			if (pUsrInfo->consumer[i].bUse)
			{
				pUsrInfo->consumer[i].offset = pUsrInfo->productor.offset;
				pUsrInfo->consumer[i].offsetFrame = OFFSETFRAME_NONE;
				pUsrInfo->consumer[i].iFrameNO = pUsrInfo->productor.iFrameNO;
				pUsrInfo->consumer[i].rCount = pUsrInfo->productor.wCount;
			}
		}
	}

	return 0;
}

//获取未读的帧数，做缓冲计数
HR_S32 CStreamBuff::GetUnreadCount()
{
	
	if ( NULL == mpShareMem ) 
	{
		HRLOG_E("mpShareMem is null\n");
		return -1; 
	}
	HR_S32 wcnt = 0, rcnt = 0;
	ShmUsrInfo *pUsrInfo = (ShmUsrInfo* )mpShareMem;

	wcnt = pUsrInfo->productor.wCount;
	rcnt = pUsrInfo->consumer[mUsrIndex].rCount; 
	return (wcnt- rcnt);
}

//获取缓冲区已存储的大小
HR_S32 CStreamBuff::GetSavedMemSize()
{

	if ( NULL == mpShareMem ) 
	{
		HRLOG_E("mpShareMem is null\n");
		return -1; 
	}
	CAutoLock CAutoLock(*m_pMutex);
	ShmUsrInfo *pUsrInfo = (ShmUsrInfo* )mpShareMem;
	ShmDataHead* pDataHead = (ShmDataHead* )( mpShareMem + sizeof(ShmUsrInfo) );
	HR_S32 head = pUsrInfo->productor.offset % mMaxFrames;  //当前写者偏移
	HR_S32 writePosition = pDataHead[head].position;	 //当前写入位置
	HR_S32 tail = pUsrInfo->consumer[mUsrIndex].offset % mMaxFrames; //当前写者偏移
	HR_S32 ReadPosition = pDataHead[tail].position;

	if ((pUsrInfo->productor.iFrameNO > pUsrInfo->consumer[mUsrIndex].iFrameNO))
	{
	//	std::cout << "wpos--" << writePosition << "rpos"<< ReadPosition << "head = "<<head<< "tail=" << tail <<endl; 
		if (head >= tail)
		{
			if(writePosition > ReadPosition)
			{
				return (writePosition - ReadPosition);
			}
			else if (writePosition <= ReadPosition)
			{
				return (mShmSize + writePosition - ReadPosition);
			}
		}
		if (head < tail)
		{
			if (writePosition <= ReadPosition)
			{
				return (mShmSize + writePosition - ReadPosition);
			}	
		}	
	}
	return 0;
}
