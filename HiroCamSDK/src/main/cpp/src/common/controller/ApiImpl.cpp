#include "ApiImpl.h"
#include <string.h>
#define VIDEOSTREAMBUF_SIZE         1*1024*1024
#define VIDEOSTREAMBUF_FRAEMSIZE    160

#define AUDIOFRAME_SIZE             512*1024
#define AUDIOFRAME_CNT              160

namespace HiroCamSDK
{
	CApiImpl::CApiImpl()
		: m_iRef(0)
		, m_iARef(0)
	{
        memset(m_pUid, 0x00, sizeof(m_pUid));
        m_pDecData = NULL;
        m_pAudioData = NULL;
        
        INIT_MUTEX(&m_mutexVDec);
        
        char videoName[32];
        sprintf(videoName, "APIIMPL_VIDEO%p", this);
        m_pVideoWriter = new CStreamBuff(videoName, SHM_ACCESS_WRITE_NONBLOCK, VIDEOSTREAMBUF_SIZE, VIDEOSTREAMBUF_FRAEMSIZE);
        m_pVideoReader = new CStreamBuff(videoName, SHM_ACCESS_READ, VIDEOSTREAMBUF_SIZE, VIDEOSTREAMBUF_FRAEMSIZE);
        
        
        char audioName[32];
        sprintf(audioName, "APIIMPL_AUDIO%p", this);
        m_pAudioWriter = new CStreamBuff(audioName, SHM_ACCESS_WRITE_NONBLOCK, AUDIOFRAME_SIZE, AUDIOFRAME_CNT);
        m_pAudioReader = new CStreamBuff(audioName, SHM_ACCESS_READ, AUDIOFRAME_SIZE, AUDIOFRAME_CNT);
        
        m_pParse = new Protocol::HiroCam::CHiroCamParse(m_pUid, m_pVideoWriter, m_pAudioWriter);
        m_VideoDecManager.CreateDecoderInstance(AV_Decoder_ID_H264, sizeof(HRSDK_DecFrame));

#ifdef _USE_RECORD
        m_record.Init(videoName, VIDEOSTREAMBUF_SIZE, VIDEOSTREAMBUF_FRAEMSIZE, audioName, AUDIOFRAME_SIZE, AUDIOFRAME_CNT, RecordErrorCallBack, this);
#endif

	}
	CApiImpl::~CApiImpl()
	{
        delete m_pParse;
        m_VideoDecManager.DestroyDecoderInstance();

        delete m_pVideoWriter;
        delete m_pVideoReader;
        delete m_pAudioWriter;
        delete m_pAudioReader;
        if (m_pDecData) {
            free(m_pDecData);
        }
        if (m_pAudioData) {
            free(m_pAudioData);
        }
        RELEASE_MUTEX(&m_mutexVDec);
	}
    
    
    void CApiImpl::Release()
    {
        
    }

	void CApiImpl::WillBeRelease()
	{
		m_pParse->WillBeRelease();
	}

	HR_U32 CApiImpl::Ref()
	{
		m_iRef ++;
		return m_iRef;
	}

	HR_U32 CApiImpl::UnRef()
	{
		if (m_iRef > 0) {
			m_iRef--;
		}
		return m_iRef;
	}

	HR_U32 CApiImpl::GetRef()
	{
		return m_iRef;
	}

	HR_U32 CApiImpl::ARef()
	{
		m_iARef ++;
		return m_iARef;
	}

	HR_U32 CApiImpl::UnARef()
	{
		if (m_iARef > 0) {
			m_iARef--;
		}
		return m_iARef;
	}

	HR_U32 CApiImpl::GetARef()
	{
		return m_iARef;
	}
    
    
    void CApiImpl::SetUsrInfo(char* uid)
    {
        strcpy(m_pUid, uid);
    }
    
    HRSDK_CMDRESULT CApiImpl::Login(HR_S8* usr, HR_S8* pwd, HR_S32 bLive, HRSDK_Login* loginRst, HR_S32& timeout)
    {
        HRSDK_CMDRESULT rst = m_pParse->Login(NULL, 0, pwd, (HR_BOOL)bLive, timeout);
        if (HRSDKCMDRST_SUCCESS == rst && loginRst) {
            m_pParse->getLoginRst(loginRst);
        }
        return rst;
    }
    
    HRSDK_CMDRESULT CApiImpl::LogOut()
    {
        return m_pParse->LogOut();
    }
    
    HRSDK_CMDRESULT CApiImpl::StartTalk(HR_S32 timeout)
    {
        return m_pParse->StartTalk(timeout);
    }

    HRSDK_CMDRESULT CApiImpl::StopTalk(HR_S32 timeout)
    {
        return m_pParse->StopTalk(timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::ChangePwd(char* oldPwd, char* newPwd, HR_S32 timeout)
    {
        return m_pParse->ChangePwd(oldPwd, newPwd, timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::GetDevInfo(HRSDK_DevInfo* devInfo, HR_S32& timeout)
    {
        return m_pParse->GetDevInfo(devInfo, timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::SetDevInfo(HRSDK_SetDevInfo& devInfo, HR_S32 timeout)
    {
        return m_pParse->SetDevInfo(&devInfo, timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::GetPirInfo(HRSDK_PirInfo* pirInfo, HR_S32& timeout)
    {
        return m_pParse->GetPirInfo(pirInfo, timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::SetPirInfo(HRSDK_PirInfo& pirInfo, HR_S32 timeout)
    {
        return m_pParse->SetPirInfo(&pirInfo, timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::FormatSDCardInfo(HRSDK_SDCardInfo* sdCardInfo, HR_S32 timeout)
    {
        return m_pParse->FormatSDCardInfo(sdCardInfo, timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::VideoFlip(HR_S32 timeout)
    {
        return m_pParse->VideoFlip(timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::VideoMirror(HR_S32 timeout)
    {
        return m_pParse->VideoMirror(timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::GetRecordMap(int year, int month, int* map, HR_S32 timeout)
    {
        return m_pParse->GetRecordMap(year, month, map, timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::GetRecordList(int year, int month, int day, HRSDK_RecordListHead* list, int listBufSize, HR_S32 timeout)
    {
        return m_pParse->GetRecordList(year, month, day, list, listBufSize, timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::SendTalkData(char* data, int len)
    {
        return m_pParse->SendTalkData(data, len);
    }
    
    HRSDK_CMDRESULT CApiImpl::GetVideoData(HRSDK_DecFrame **data, int *outLen, HRDECFMT videoFmt)
    {
#ifndef DECODE_VIDEO
        if (m_pVideoReader && data && outLen)
        {
            CAutoLock lock(m_mutexVDec);
            if (m_pDecData == NULL)
                m_pDecData = (HRSDK_DecFrame*)malloc(VIDEOSTREAMBUF_SIZE);
            
            int isKey = 0;
            int rLen = m_pVideoReader->getFrame((char*)m_pDecData, 2*1024*1024, &isKey);
            if (rLen > 0 && m_pDecData->len > 0 && (rLen == m_pDecData->len + sizeof(HRSDK_DecFrame)))
            {
                if (m_pDecData->type == HRSDKMT_VIDEO )
                {
                    if ( m_VideoDecManager.DecoderVideo((HR_U8*)m_pDecData->data, m_pDecData->len, (HR_U8**)data, (HR_U32*)outLen
                                                        , (HR_U32*)&m_pDecData->w, (HR_U32*)&m_pDecData->h, (HRDECFMT)videoFmt) && *data)
                    {
                        HRSDK_DecFrame *pDecData = *(HRSDK_DecFrame**)data;
                        pDecData->type = m_pDecData->type;
                        pDecData->w = m_pDecData->w;
                        pDecData->h = m_pDecData->h;
                        pDecData->len = *outLen;
                        *outLen = pDecData->len + sizeof(HRSDK_DecFrame);
                        return HRSDKCMDRST_SUCCESS;
                    }
                }/*else if (m_pDecData->type == HRSDKMT_AUDIO){
                    *data = m_pDecData;
                    *outLen = m_pDecData->len + sizeof(HRSDK_DecFrame);
                    return HRSDKCMDRST_SUCCESS;
                }*/
            }
            return HRSDKCMDRST_TIMEOUT;
        }
        return HRSDKCMDRST_TIMESYNC_ERR;
#else
        return HRSDKCMDRST_UNSUPPORT;
#endif

    }
    
    HRSDK_CMDRESULT CApiImpl::GetAudioData(HRSDK_DecFrame **data, int *outLen)
    {
        if (m_pAudioReader && data && outLen)
        {
            if (!m_pAudioData) {
                m_pAudioData = (HRSDK_DecFrame*)malloc(5*1024);
            }
            int isKey = 0;
            int rLen = m_pAudioReader->getFrame((char*)m_pAudioData, 5*1024, &isKey);
            if (rLen > 0 && m_pAudioData->len > 0 && (rLen == m_pAudioData->len + sizeof(HRSDK_DecFrame)))
            {
                if (m_pAudioData->type == HRSDKMT_AUDIO){
                    *data = m_pAudioData;
                    *outLen = m_pAudioData->len + sizeof(HRSDK_DecFrame);
                    return HRSDKCMDRST_SUCCESS;
                }
            }
            
            return HRSDKCMDRST_TIMEOUT;
        }
        return HRSDKCMDRST_TIMESYNC_ERR;
    }
    
    HRSDK_CMDRESULT CApiImpl::StartPlayback(char* filename, HRSDK_RecordInfo* rcdInfo, int *timeout)
    {
        return m_pParse->StartPlayback(filename, rcdInfo, *timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::StopPlayback(int* timeout)
    {
        return m_pParse->StopPlayback(*timeout);
    }
    
    HRSDK_CMDRESULT CApiImpl::StartRecord(HRRECORDTYPE type,const HR_S8 *filename)
    {
#ifdef _USE_RECORD
        if (m_record.IsWillSetAudio())
        {
            return HRSDKCMDRST_SYSBUSY;
        }
        return m_record.StartRecord(type, filename);
#endif
        return HRSDKCMDRST_UNSUPPORT;
    }
    
    HRSDK_CMDRESULT CApiImpl::StopRecord()
    {
#ifdef _USE_RECORD
        return m_record.StopRecord();
#else
        return HRSDKCMDRST_UNSUPPORT;
#endif
        
    }
    
    HR_S32 CApiImpl::RecordErrorCallBack(HRSDK_CMDRESULT errorcode, HR_VOID *pContext)
    {
        CApiImpl *pThis = (CApiImpl*)pContext;
#if 0
        switch(errorcode)
        {
            case HRSDKCMDRST_ACHIEVE_FILE_MAXSIZE:
            {
                FosSdk::EventMSG msg;
                msg.message = RECORD_ACHIEVE_FILE_MAXSIZE;
                pThis->m_pAdapter->PostEvent(msg);
                break;
            }
            case FOSRECORD_ERROR_NO_ENOUGE_SPACE:
            {
                FosSdk::EventMSG msg;
                msg.message = RECORD_NO_ENOUGE_SPACE;
                pThis->m_pAdapter->PostEvent(msg);
                break;
            }
            case HRSDKCMDRST_ACHIEVE_FILE_CHANGE_RES:
            {
                FosSdk::EventMSG msg;
                msg.message = RECORD_RESOLUTION_CHANGE;
                pThis->m_pAdapter->PostEvent(msg);
                break;
            }
            case FOSRECORD_ERROR_FILE_PATH_NOEXIST:
            {
                FosSdk::EventMSG msg;
                msg.message = RECORD_FILE_PATH_NOEXIST;
                pThis->m_pAdapter->PostEvent(msg);
                break;
            }
                
            case FOSRECORD_ERROR_UNKNOW:
            {
                FosSdk::EventMSG msg;
                msg.message = RECORD_UNKNOW;
                pThis->m_pAdapter->PostEvent(msg);
                break;
            }
            default:
            {
                break;
            }
        }
#endif
        return 0;
    }
    
}




