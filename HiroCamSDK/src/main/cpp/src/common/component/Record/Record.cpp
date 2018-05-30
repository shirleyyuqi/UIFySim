#include "Record.h"
#ifdef _USE_RECORD

CRecord::CRecord()
	: m_bHasAudio(HR_TRUE)
	, m_pRecord(NULL)
	, m_IsRecord(HR_FALSE)
	, m_bWaitQuit(HR_FALSE)
	, m_IsThreadRun(HR_TRUE)
	, m_bFristFrame(HR_TRUE)
	, m_bWillAudio(HR_FALSE)
	, m_pVideoReader(NULL)
	, m_pAudioReader(NULL)
{
	INIT_MUTEX(&m_mutex);
}

CRecord::~CRecord(void)
{
	m_IsThreadRun = HR_FALSE;
	m_hRecrodThread.ExitThead(INFINITE, 0);
	{
		CAutoLock lock(m_mutex);
		if (m_pRecord)
		{
			m_pRecord->CloseFile();
			CRecordFactory::ReleaseRecord(m_pRecord);
			m_pRecord = NULL;
		}
	}
	delete m_pVideoReader;
	delete m_pAudioReader;
	RELEASE_MUTEX(&m_mutex);
}

int CRecord::Init(const HR_CHAR *VideoStreambufName, HR_S32 videoStreambufSize, HR_S32 videoFrameSize, const HR_CHAR *AudioStreambufName, HR_S32 audioStreambufSize, HR_S32 audioFrameSize, CallBackFunc pFunc, HR_VOID *pContext)
{
	m_pCallBackFunc = pFunc;
	m_pContext = pContext;
	m_pVideoReader = new CStreamBuff((HR_CHAR*)VideoStreambufName, SHM_ACCESS_READ, videoStreambufSize, videoFrameSize);
	m_pAudioReader = new CStreamBuff((HR_CHAR*)AudioStreambufName, SHM_ACCESS_READ, audioStreambufSize, audioFrameSize);
	m_hRecrodThread.CreateThread(1, 1, 2 * 1024 * 1024, &RecordThread, this);
	return 0;
}

HR_VOID CRecord::SetRecordAudio(HR_BOOL bAudio)
{
	m_bHasAudio = bAudio;
}

HR_BOOL CRecord::IsWillSetAudio()
{
	return m_bWillAudio;
}

HR_VOID CRecord::WillSetAudioState(HR_BOOL bWillAudio)
{
	m_bWillAudio = bWillAudio;
}

HR_BOOL CRecord::IsRecord()
{
	return m_IsRecord;
}

HRSDK_CMDRESULT CRecord::StartRecord(HRRECORDTYPE filetype, const HR_CHAR *filename)
{
	CAutoLock lock(m_mutex);
	if (m_pRecord)
	{
		return HRSDKCMDRST_SYSBUSY;
	}
	
	m_pRecord = CRecordFactory::CreateRecord(filetype);
	if (m_pRecord)
	{
		HRSDK_CMDRESULT ret = m_pRecord->OpenFile(filename);
		if (HRSDKCMDRST_SUCCESS == ret)
		{
			m_bFristFrame = HR_TRUE;
			m_IsRecord = HR_TRUE;
        }else{
            CRecordFactory::ReleaseRecord(m_pRecord);
            m_pRecord = NULL;
        }
		return ret;
	}	
	return HRSDKCMDRST_ARG_ERR;
}

HRSDK_CMDRESULT CRecord::StopRecord()
{
	CAutoLock lock(m_mutex);
	if (m_pRecord)
	{
		m_bWaitQuit = HR_FALSE;
		m_IsRecord = HR_FALSE;
		while(!m_bWaitQuit){
			SLEEP(20);
		}
		HRSDK_CMDRESULT ret = m_pRecord->CloseFile();
		CRecordFactory::ReleaseRecord(m_pRecord);
		m_pRecord = NULL;
		return ret;
	}
	return HRSDKCMDRST_TIMESYNC_ERR;
}

void CRecord::fillEmputyAudio(HRSDK_DecFrame* pAudioDataInfo, int index, int audioSize)
{
	if (audioSize <= 0)
	{
		return;
	}
	
	char* pTempAudio = (char*)malloc(sizeof(HRSDK_DecFrame)+ audioSize);
	memset(pTempAudio, 0x0, sizeof(HRSDK_DecFrame)+ audioSize);
	memcpy(pTempAudio, pAudioDataInfo, sizeof(HRSDK_DecFrame));
	HRSDK_DecFrame	*pDataInfo = (HRSDK_DecFrame*)pTempAudio;
	pDataInfo->len = audioSize;
	m_pRecord->WriteAudio(pDataInfo);
	HR_SAFACE_FREE(pTempAudio);
}

HR_THREADMETHOD(CRecord::RecordThread, lpParam)
{
	CRecord* pThis = (CRecord*)lpParam;
	pThis->RecordThreadRun();
	THREAD_RETURN(0);
}

HR_S32 CRecord::RecordThreadRun()
{
	m_pVideoReader->rPosUpdate();
	m_pAudioReader->rPosUpdate();

	char* pVideoBuf = (char*)malloc(VIDEOBUFSIZE);
	char* pAudioBuf = (char*)malloc(AUDIOBUFSIZE);
	bool bGetVideo = true;
	bool bGetAudio = true;
	bool bWriteVideo = false;
	bool bWriteAudio = false;
	bool bMakeWriteVideo = false;
	bool bMakeWriteAudio = false;
	HR_S32 isKey = 0;
	HRSDK_DecFrame	*pVideoDataInfo = (HRSDK_DecFrame*)pVideoBuf;
	HRSDK_DecFrame	*pAudioDataInfo = (HRSDK_DecFrame*)pAudioBuf;
	int  audioIndex = 0;

	HR_S64	iPtsTmp = 0;
	HR_S64 iVideoPrevPts = 0;
	HR_S64 iAudioPrevPts = 0;

	HR_S64 iAudioPrePtsByData = 0;
	HR_S64	iAudioPtsByData = 0;
	HR_S64 iAudioDataPtsTmp = 0;
    
    HR_S16 sAudioPreIsMultiPage = 0;
	HR_BOOL bError = HR_FALSE;
    
    
	while(m_IsThreadRun)
	{
		if (!m_IsRecord)
		{
			if (!m_bFristFrame || !bGetVideo || !bGetAudio)
			{
				m_pVideoReader->rPosUpdate();
				m_pAudioReader->rPosUpdate();
			}
			
			m_bWaitQuit = HR_TRUE;
			bError = HR_FALSE;
			audioIndex = 0;
			iPtsTmp = 0;
			iVideoPrevPts = 0;
			iAudioPrevPts = 0;
			iAudioPrePtsByData = 0;
			iAudioPtsByData = 0;
			iAudioDataPtsTmp = 0;

			bMakeWriteVideo = false;
			bMakeWriteAudio = false;
			m_bFristFrame = HR_TRUE;
			bWriteVideo = false;
			bWriteAudio = false;
			bGetVideo = true;
			bGetAudio = true;
			SLEEP(20);
			continue;
		}
		
		if (bGetVideo)
		{
			if( m_pVideoReader->getFrame(pVideoBuf, VIDEOBUFSIZE, &isKey) > 0 )
			{
                pVideoDataInfo->pts /= 1000;
				bGetVideo = false;
				bWriteVideo = true;
				pVideoDataInfo->pts -= iPtsTmp;
			}else{
				bWriteVideo = false;
			}
		}
		if (bGetAudio && m_bHasAudio)
		{
			if( m_pAudioReader->getFrame(pAudioBuf, AUDIOBUFSIZE, &isKey) > 0 )
			{
                pAudioDataInfo->pts /= 1000;
				audioIndex = pAudioDataInfo->pts / SIGLEAUDIOFRAMETM;
				iAudioPtsByData = audioIndex * SIGLEAUDIOFRAMETM;
				bGetAudio = false;
				bWriteAudio = true;
				iAudioPtsByData -= iAudioDataPtsTmp;
				pAudioDataInfo->pts -= iPtsTmp;
			}else{
				bWriteAudio = false;
			}
		}
		
		if ( (bWriteAudio && bWriteVideo) || (!m_bHasAudio && bWriteVideo))
		{
			if (m_bFristFrame)//this frist frame
			{
				if (m_bHasAudio)
				{
					if (pVideoDataInfo->pts >= pAudioDataInfo->pts)
					{
						pVideoDataInfo->pts = pAudioDataInfo->pts;
						iVideoPrevPts = pAudioDataInfo->pts;
						iAudioPrePtsByData = (audioIndex-1) * SIGLEAUDIOFRAMETM;
					}else{
						int audioSize = (pAudioDataInfo->pts - pVideoDataInfo->pts) * BITOFPERMS;
						fillEmputyAudio(pAudioDataInfo, 0, audioSize);
						
						iVideoPrevPts = pVideoDataInfo->pts;
						iAudioPrePtsByData = (audioIndex-1) * SIGLEAUDIOFRAMETM;
					}
					iAudioPrevPts = pAudioDataInfo->pts;
				}			
				m_bFristFrame = HR_FALSE;
			}
			// pts is too big
			else if ((m_bHasAudio && (HR_S64)(pAudioDataInfo->pts - iAudioPrevPts) >= MAXEMPUTYTIME) || (HR_S64)(pVideoDataInfo->pts - iVideoPrevPts) >= MAXEMPUTYTIME)
			{
				if (m_bHasAudio)
				{
					HR_S64 videoPts = pVideoDataInfo->pts - iVideoPrevPts;
					HR_S64 audioPts = pAudioDataInfo->pts - iAudioPrevPts;
					HR_S64 pts = 0;
					if (videoPts >= audioPts)
					{
						pts = iAudioPtsByData - (iAudioPrePtsByData + SIGLEAUDIOFRAMETM);						
					}else{
						pts = videoPts / SIGLEAUDIOFRAMETM * SIGLEAUDIOFRAMETM;
					}

					iAudioPtsByData -= pts;
					iAudioDataPtsTmp += pts;

					iPtsTmp += pts;
					pVideoDataInfo->pts -= pts;
					pAudioDataInfo->pts -= pts;
				}else{
					iPtsTmp += (pVideoDataInfo->pts - iVideoPrevPts - 30);
					pVideoDataInfo->pts -= (iVideoPrevPts - 30);
				}				
			}
			//pts is too small
			else if ((m_bHasAudio && (HR_S64)(pAudioDataInfo->pts - iAudioPrevPts) < 0) || (HR_S64)(pVideoDataInfo->pts - iVideoPrevPts) < 0)
			{
                
				if (m_bHasAudio)
				{
					HR_S64 videoPts = pVideoDataInfo->pts - iVideoPrevPts;
					HR_S64 audioPts = pAudioDataInfo->pts - iAudioPrevPts;
                    if (pVideoDataInfo->pts >= (iAudioPrevPts + MAXEMPUTYTIME/2)) {
                        bMakeWriteAudio = true;
                    }else if ((sAudioPreIsMultiPage && (audioPts<0 && audioPts >= -SIGLEAUDIOFRAMETM))) {
                        
//                        FILE *pMultiPageFile = fopen("/Users/foscam-hwh/Documents/recordMultiPage.txt", "at");
//                        if (pMultiPageFile) {
//                            char a[64] = {0};
//                            sprintf(a, "sAudioPreIsMultiPage:%d audioPts:%lld\n",sAudioPreIsMultiPage,audioPts);
//                            fwrite(a, strlen(a), 1, pMultiPageFile);
//                            fclose(pMultiPageFile);
//                            pMultiPageFile = NULL;
//                        }
                        
                        bMakeWriteAudio = true;
                    }else if (videoPts > 0)
					{
						bMakeWriteVideo = true;
					}else if (audioPts > 0)
					{
						bMakeWriteAudio = true;
					}else{
						HR_S64 pts = max(-audioPts, -videoPts);
						pts = pts / SIGLEAUDIOFRAMETM * SIGLEAUDIOFRAMETM + SIGLEAUDIOFRAMETM;
						iAudioPtsByData += pts;
						iAudioDataPtsTmp -= pts;

						iPtsTmp -= pts;
						pVideoDataInfo->pts += pts;
						pAudioDataInfo->pts += pts;
					}
				}else{
					iPtsTmp -= (iVideoPrevPts - pVideoDataInfo->pts + 30);
					pVideoDataInfo->pts += (iVideoPrevPts - pVideoDataInfo->pts + 30);					
				}
			}

			if (bMakeWriteAudio || (!bMakeWriteVideo && m_bHasAudio && pVideoDataInfo->pts >= pAudioDataInfo->pts))
			{
				if (iAudioPtsByData > (iAudioPrePtsByData + SIGLEAUDIOFRAMETM))
				{
					int audioSize = (int)(iAudioPtsByData - (iAudioPrePtsByData + SIGLEAUDIOFRAMETM)) * BITOFPERMS;
					fillEmputyAudio(pAudioDataInfo, 0, audioSize);				
				}
				
				int ret = m_pRecord->WriteAudio(pAudioDataInfo);
				if ( 0 != ret && m_pCallBackFunc && !bError){
					bError = HR_TRUE;
					(*m_pCallBackFunc)((HRSDK_CMDRESULT)ret, m_pContext);
				}
				iAudioPrePtsByData = iAudioPtsByData;
				iAudioPrevPts = pAudioDataInfo->pts;
                //sAudioPreIsMultiPage = pAudioDataInfo->multiAudioPage;
				bGetAudio = true;
				bMakeWriteAudio = false;
			}else if (bWriteVideo || bMakeWriteVideo)
			{
				int ret = m_pRecord->WriteVideo(pVideoDataInfo);
				if ( 0 != ret && m_pCallBackFunc && !bError){
				    bError = HR_TRUE;
					(*m_pCallBackFunc)((HRSDK_CMDRESULT)ret, m_pContext);
				}
				iVideoPrevPts = pVideoDataInfo->pts;
				bGetVideo = true;
				bMakeWriteVideo = false;
			}	
		}
		SLEEP(20);
	}

	HR_SAFACE_FREE(pVideoBuf);
	HR_SAFACE_FREE(pAudioBuf);
	return 0;
}

#endif

