#pragma once
#ifdef _USE_RECORD
#include "IRecord.h"
#include "RecordFactory.h"
#include "CStreamBuffer.h"
#include "HiroCamDef.h"
#include "AudioCodec.h"
#include "Thread.h"
#include "BaseFunc.h"
#include "AutoLock.h"

#define VIDEOBUFSIZE		(1024*1024)
#define AUDIOBUFSIZE		(128*1024)
#define SIGLEAUDIOFRAMETM	(960/16)
#define BITOFPERMS				16
#define MAXEMPUTYTIME			3000
typedef HR_S32 (*CallBackFunc)(HRSDK_CMDRESULT errorcode, HR_VOID *pContext);

class CRecord
{
public:
	CRecord(void);
	~CRecord(void);
	int Init(const HR_CHAR *VideoStreambufName, HR_S32 videoStreambufSize, HR_S32 videoFrameSize, const HR_CHAR *AudioStreambufName, HR_S32 audioStreambufSize, HR_S32 audioFrameSize, CallBackFunc pFunc, HR_VOID *pContext);
	HRSDK_CMDRESULT StartRecord(HRRECORDTYPE filetype, const HR_CHAR *filename);
	HRSDK_CMDRESULT StopRecord();

	HR_BOOL IsRecord();
	HR_VOID SetRecordAudio(HR_BOOL bAudio);
	HR_BOOL IsWillSetAudio();
	HR_VOID WillSetAudioState(HR_BOOL bWillAudio);
protected:
	HR_THREADMETHOD_DEC(RecordThread,lpParam);
	HR_S32 RecordThreadRun();
protected:
	void fillEmputyAudio(HRSDK_DecFrame* pAudioDataInfo, int index, int audioSize);
protected:
	CStreamBuff*		m_pVideoReader;	
	CStreamBuff*		m_pAudioReader;	
	CallBackFunc		m_pCallBackFunc;
	HR_VOID	*		m_pContext;
	IRecord*			m_pRecord;	

	HR_BOOL			m_IsThreadRun;
	HR_BOOL			m_bHasAudio;
	HR_BOOL			m_bWillAudio;
	HR_BOOL			m_IsRecord;
	HR_BOOL			m_bWaitQuit;
	HR_BOOL			m_bFristFrame;
	CThread			m_hRecrodThread;

	MUTEX			m_mutex;
};
#endif
