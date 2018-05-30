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
}ShmProductorInfo;    //��������Ϣ

typedef struct tagShmConsumerInfo
{
	HR_BOOL		bUse;
	OFFSETFRAME		offsetFrame;
	HR_S32			offset;
	HR_U32			iFrameNO;
	HR_U32         rCount;
}ShmConsumerInfo;  //��������Ϣ

typedef struct tagShmUsrInfo
{
	ShmProductorInfo productor;
	ShmConsumerInfo consumer[MAX_CONSUMER];
}ShmUsrInfo;    //ʹ������Ϣ

typedef struct tagShmDataHead
{
	HR_S32			position;
	HR_S32			length;
	HR_S32			isKeyFrame;
	HR_U32			iFrameNO;
} ShmDataHead;  //����ͷ��ÿ֡���ݴ�һ��ͷ��

//����洢ʹ���ߺ�����֡ͷ����Ϣ�Ŀռ�
#define SHARE_MEM_HEAD_SIZE(n) (sizeof(ShmUsrInfo) + n*sizeof(ShmDataHead))   
map<string, CStreamBuff::CBuffer*>  CStreamBuff::m_vBuffer;
MUTEX CStreamBuff::m_vMutex; 

CStreamBuff::CBuffer::CBuffer(HR_S32 size, HR_S32 maxFrames)
: m_iReference(0)
{
	INIT_MUTEX(&m_Mutex);//��ʼ����
	m_iMaxFrame = maxFrames;
	m_iSize = size;
	m_cBuffer = (HR_CHAR*)malloc(SHARE_MEM_HEAD_SIZE(m_iMaxFrame)+size);
	memset(m_cBuffer, 0x00, SHARE_MEM_HEAD_SIZE(m_iMaxFrame));
}

CStreamBuff::CBuffer::~CBuffer()
{
	RELEASE_MUTEX(&m_Mutex);//�ͷ���
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

//���ʼ���
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

//��ȡ���������֡��
HR_S32 CStreamBuff::CBuffer::GetMaxFrame()   
{
	return m_iMaxFrame;
}

//��ȡ����������
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

/*���캯��
  ʹ�� name ��ʾ��������ͬһname�Ļ�������ͬ
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

//д����
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
	position = pDataHead[head].position; //����д��λ��
	
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

//������
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

//��������λ�õ�д�ߵ�ǰλ��
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

//����Ƿ��д��
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

	head = pUsrInfo->productor.offset % mMaxFrames; //дƫ��
	writePosition = pDataHead[head].position; //дλ��

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
	head = pUsrInfo->productor.offset % mMaxFrames;  //��ǰд��ƫ��
	writePosition = pDataHead[head].position;	 //��ǰд��λ��
	//һ����д�����ݳ��ȴ��ڻ��������Ȼ�д��λ�ò��Ϸ�
	if ( ((writePosition + frameLen) >= mShmSize) ||   
		((HR_U32)writePosition < SHARE_MEM_HEAD_SIZE(mMaxFrames)) )
	{
		writePosition_old = writePosition;
		writePosition = SHARE_MEM_HEAD_SIZE(mMaxFrames); //�ӻ�������ʼλ�ÿ�ʼд
		pDataHead[head].position = writePosition;
	}
	
	for( i = 0; i < MAX_CONSUMER; i++ )
	{
		//bPtrMoved = HR_FALSE;
		if( !pUsrInfo->consumer[i].bUse )
		{
			continue;
		}

		tail = pUsrInfo->consumer[i].offset % mMaxFrames; //��ǰ����ƫ��

		/*
		    ________________________________________________________
			| ǰ                 ������                        ��  |
			--------------------------------------------------------
			iFrameNo ��ʾ֡��ţ���productor����.
			1)head == tail ʱ, consumer[i].iFrameNo != productor.iFrameNo ����Ҫ������֡.
			2)head != tailʱ
			  1. дָ��ӽ���������ĩβ �� �������Ҫд���֡����Ҫдָ���ƻ�ͷд�����������ĩβ��֡������������I֡��
			  2. ��ָ����дָ����� �� дָ��д�ĳ��ȸ��ǵ���ָ�룬����Ҫ������I֡
			  3. ��ָ�����дָ�� �� consumer[i].iFrameNo != productor.iFrameNo ����Ҫ������֡.
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

			pUsrInfo->consumer[i].rCount = pUsrInfo->consumer[i].rCount+ pDataHead[iFrame].iFrameNO - pUsrInfo->consumer[i].iFrameNO;  //����count

			pUsrInfo->consumer[i].iFrameNO = pDataHead[iFrame].iFrameNO;

			pUsrInfo->consumer[i].offset = iFrame;

		}
	}

	return HR_TRUE;
}
//��ȡ��һ��I ֡������д���ǻ���֡ʱ�����߶�ȡ����һ֡����ΪI֡��
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
	tail = pUsrInfo->consumer[userIdx].offset + 1;  //��ǰ����ƫ��
	head = pUsrInfo->productor.offset;  //��ǰд��ƫ��
	writePosition = pDataHead[head].position; //��ǰд��λ��
	iFrame = head; 
	
	for( i = 0; i < mMaxFrames-1; i++ )
	{
		if( tail  == head )
		{
			iFrame = head;  //ԭ����
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
		if( pDataHead[tail].isKeyFrame == 1 && (head+1) != tail)  //��ǰ������֡Ϊ I ֡, д����һ֡��Ϊ����λ�ã��������I֡���������
		{
			readPosition = pDataHead[tail].position; //��ǰ������λ��
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
//д���������ж��߶�ȡλ��Ϊ��ǰд�ߵ�λ��
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

//��ȡδ����֡�������������
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

//��ȡ�������Ѵ洢�Ĵ�С
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
	HR_S32 head = pUsrInfo->productor.offset % mMaxFrames;  //��ǰд��ƫ��
	HR_S32 writePosition = pDataHead[head].position;	 //��ǰд��λ��
	HR_S32 tail = pUsrInfo->consumer[mUsrIndex].offset % mMaxFrames; //��ǰд��ƫ��
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
