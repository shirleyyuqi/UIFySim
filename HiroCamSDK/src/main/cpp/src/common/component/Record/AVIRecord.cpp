#include "AVIRecord.h"

#define DEFALUT_FRAME_RATE      30
#define DEFAULT_AUDIO_OFFSET		480

namespace HiroCamSDK {
 

CAVIRecord::CAVIRecord(void)
	: m_bHasRecordKeyFrameGot(HR_FALSE)
	, m_nVideoWidth(0)
	, m_nVideoHeight(0)
	, m_nFileTotalSize(0)
	, m_iPrePts(0)
	, m_pFristAudio(NULL)
	, m_iFristAudioLen(0)
{
}


CAVIRecord::~CAVIRecord(void)
{
	CloseFile();
}

HRSDK_CMDRESULT CAVIRecord::OpenFile(const HR_CHAR* filename)
{
	if(m_AVIEncoder.CreateAVIFile(filename) != NULL)
	{
		HR_SAFACE_FREE(m_pFristAudio);
		m_nFileTotalSize = 0;
		m_iFristAudioLen = 0;
		m_iTotalTime = 0;
		m_bHasRecordKeyFrameGot = HR_FALSE;
		return HRSDKCMDRST_SUCCESS;
	}
	return HRSDKCMDRST_ACCESSDENY;
}

HRSDK_CMDRESULT CAVIRecord::CloseFile()
{
	HR_SAFACE_FREE(m_pFristAudio);
	m_AVIEncoder.CloseAVIFile();	
	return HRSDKCMDRST_SUCCESS;
}

HR_S32 CAVIRecord::WriteVideo(HRSDK_DecFrame* data)
{
	if( !m_bHasRecordKeyFrameGot )
	{
		if(1 == data->isKey)
		{
			HiroCamSDK::Model::AVIENC_Metadata metaData;
			memset( &metaData, 0x00, sizeof(HiroCamSDK::Model::AVIENC_Metadata) );
			metaData.width = data->w;
			metaData.height = data->h;
			metaData.frameRate = DEFALUT_FRAME_RATE;
			m_AVIEncoder.WriteMetadata(&metaData);
			int ret = m_AVIEncoder.WriteVideoData((HR_U8*)data->data, data->len, data->pts, data->isKey);
			HR_ASSERT(ret == 0, HRLOG_E("AVI WriteVideoData err (frist frame)\n"), HRSDKCMDRST_ERR_UNKNOW);
			m_bHasRecordKeyFrameGot = HR_TRUE;
			m_nVideoHeight = metaData.width;
			m_nVideoWidth = metaData.height;
			m_nFileTotalSize += data->len;
			m_iPrePts = data->pts;
			m_iTotalTime = 0;
		}
	}
	else
	{
		if( (data->w != m_nVideoWidth) || (data->h != m_nVideoHeight) )
		{
			HRLOG_W("Video resolution change,stop record!");
			return HRSDKCMDRST_ACHIEVE_FILE_CHANGE_RES;
		}

		m_iTotalTime += (data->pts - m_iPrePts - 33.3);
		m_iPrePts = data->pts;
		while(m_iTotalTime >= 33.3){
			m_iTotalTime -= 33.3;
			int ret = m_AVIEncoder.WriteVideoData((HR_U8*)data->data, 0, data->pts, data->isKey);
			HR_ASSERT(ret == 0, HRLOG_E("AVI WriteVideoData err"), HRSDKCMDRST_ERR_UNKNOW);
		}

		int ret = m_AVIEncoder.WriteVideoData((HR_U8*)data->data, data->len, data->pts, data->isKey);
		HR_ASSERT(ret == 0, HRLOG_E("AVI WriteVideoData err"), HRSDKCMDRST_ERR_UNKNOW);
		m_nFileTotalSize += data->len;
		if (m_nFileTotalSize >= FILEMAXSIZE)
		{
			HRLOG_W("Achieve file max size,stop record!");
			return HRSDKCMDRST_ACHIEVE_FILE_MAXSIZE;
		}
	}
	return 0;
}

HR_S32 CAVIRecord::WriteAudio(HRSDK_DecFrame* data)
{
	if (!m_bHasRecordKeyFrameGot)
	{
		HR_SAFACE_FREE(m_pFristAudio);
		if (data->len > DEFAULT_AUDIO_OFFSET)
		{
			m_iFristAudioLen = data->len - DEFAULT_AUDIO_OFFSET;
			m_pFristAudio = (HR_CHAR*)malloc(m_iFristAudioLen);
			memcpy(m_pFristAudio, data->data+DEFAULT_AUDIO_OFFSET, m_iFristAudioLen);
		}
		return HRSDKCMDRST_SUCCESS;
	}else if (m_pFristAudio){
		int ret = m_AVIEncoder.WriteAudioData((const unsigned char*)m_pFristAudio, m_iFristAudioLen, TIMESETP);
		HR_ASSERT(ret == 0, HRLOG_E("AVI WriteAudioData err\n"), HRSDKCMDRST_ERR_UNKNOW);

		m_nFileTotalSize += m_iFristAudioLen;
		if (m_nFileTotalSize >= FILEMAXSIZE)
		{
			HRLOG_W("Achieve file max size,stop record!");
			return HRSDKCMDRST_ACHIEVE_FILE_MAXSIZE;
		}

		HR_SAFACE_FREE(m_pFristAudio);
		m_iFristAudioLen = 0;
	}

	if (data->len > 0)
	{
		int ret = m_AVIEncoder.WriteAudioData((const unsigned char*)data->data, data->len, TIMESETP);
		HR_ASSERT(ret == 0, HRLOG_E("AVI WriteAudioData err"), HRSDKCMDRST_ERR_UNKNOW);

		m_nFileTotalSize += data->len;
		if (m_nFileTotalSize >= FILEMAXSIZE)
		{
			HRLOG_W("Achieve file max size,stop record!");
			return HRSDKCMDRST_ACHIEVE_FILE_MAXSIZE;
		}
	}
	return 0;
}
    
}
