#include "MP4Record.h"
#ifdef _USE_MP4
#define  ENCODEBUFSIZE 1024*1024
CMP4Record::CMP4Record()
	: m_bHasRecordKeyFrameGot(HR_FALSE)
	, m_nVideoWidth(0)
	, m_nVideoHeight(0)
	, m_nFileTotalSize(0)
	, m_isInitAACEncode(HR_FALSE)
	, m_nEncoderSize(0)
	, m_pEncodeBuf(NULL)
	, m_pAACBuf(NULL)
	, m_iEncodeBufPos(0)
    , m_bFrist(HR_TRUE)
    , m_iFristPts(0)
{
	
}

CMP4Record::~CMP4Record()
{
	HR_SAFACE_FREE(m_pEncodeBuf);
	HR_SAFACE_FREE(m_pAACBuf);
	CloseFile();
}

HRSDK_CMDRESULT CMP4Record::OpenFile(const HR_CHAR* filename)
{
	if(m_MP4Encoder.CreateMP4File(filename) != NULL)
	{
		m_nFileTotalSize = 0;
		m_bHasRecordKeyFrameGot = HR_FALSE;
		return HRSDKCMDRST_SUCCESS;
	}
	return HRSDKCMDRST_ACCESSDENY;
}

HRSDK_CMDRESULT CMP4Record::CloseFile()
{
	m_MP4Encoder.CloseMP4File();	
	if (m_isInitAACEncode)
	{
		m_AACEncode.FreeEncoder();
		m_isInitAACEncode = HR_FALSE;
	}
	return HRSDKCMDRST_SUCCESS;
}

HR_S32 CMP4Record::WriteVideo(HRSDK_DecFrame* data)
{
	if (!m_bHasRecordKeyFrameGot)
	{
		if (1 == data->isKey)
		{
			HiroCamSDK::Model::MP4ENC_Metadata metaData;
			memset( &metaData, 0x00, sizeof(HiroCamSDK::Model::MP4ENC_Metadata) );
			m_MP4Encoder.PraseMetadata((const unsigned char*)data->data, data->len, metaData);
			metaData.width = data->w;
			metaData.height = data->h;
			m_MP4Encoder.Write264Metadata(&metaData);
            
            if (m_bFrist) {
                m_bFrist = HR_FALSE;
                m_iFristPts = 0;//data->pts;
            }

			HR_S32 ret = m_MP4Encoder.WriteH264VideoData((HR_U8*)data->data, data->len, data->pts, data->isKey);
			HR_ASSERT(ret > 0, HRLOG_E("WriteH264VideoData err"), HRSDKCMDRST_ERR_UNKNOW);
			m_bHasRecordKeyFrameGot = HR_TRUE;
			m_nVideoHeight = metaData.height;
			m_nVideoWidth = metaData.width;
			m_nFileTotalSize += data->len;
		}
	}else{
		if( (data->w != m_nVideoWidth) || (data->h != m_nVideoHeight) )
		{
			HRLOG_W("Video resolution change,stop record!");
			return HRSDKCMDRST_ACHIEVE_FILE_CHANGE_RES;
		}
		HR_S32 ret = m_MP4Encoder.WriteH264VideoData((HR_U8*)data->data, data->len, data->pts, data->isKey);
		HR_ASSERT(ret > 0, HRLOG_E("WriteH264VideoData err"), HRSDKCMDRST_ERR_UNKNOW);
		m_nFileTotalSize += data->len;

		if (m_nFileTotalSize >= FILEMAXSIZE)
		{
			HRLOG_W("Achieve file max size,stop record!");
			return HRSDKCMDRST_ACHIEVE_FILE_MAXSIZE;
		}
	}
	return 0;
}

HR_S32 CMP4Record::WriteAudio(HRSDK_DecFrame* data)
{
	if (!m_isInitAACEncode)
	{
		m_isInitAACEncode = HR_TRUE;
		//m_nEncoderSize = m_AACEncode.InitEncoder(AV_Decoder_ID_FAAC, ENCODEBUFSIZE, data->media.audio.bitRate, data->media.audio.sampale, data->media.audio.channel);
        m_nEncoderSize = m_AACEncode.InitEncoder(AV_Decoder_ID_FAAC, ENCODEBUFSIZE, 64000, 8000, 2);
		m_pEncodeBuf = (HR_CHAR*)malloc(m_nEncoderSize);
		m_pAACBuf = (HR_CHAR*)malloc(m_nEncoderSize);
		m_iEncodeBufPos = 0;
	}

	char* pData = data->data;
	int dataLen = data->len;

	int time = 0;
	while (m_iEncodeBufPos + dataLen >= m_nEncoderSize)
	{
		time ++;
		HR_U32 dSize = m_nEncoderSize - m_iEncodeBufPos;
		memcpy(m_pEncodeBuf+m_iEncodeBufPos, pData, dSize);
		HR_U32 encodeOutSize = 0;
		HR_S32 ret = m_AACEncode.encode((HR_U8*)m_pEncodeBuf, m_nEncoderSize, (HR_U8*)m_pAACBuf, &encodeOutSize);
		m_iEncodeBufPos = 0;
		pData += dSize;
		dataLen -= dSize;

		//int rSize = (dataLen - dSize)>=m_nEncoderSize?m_nEncoderSize:(dataLen - dSize);
		//memcpy(m_pEncodeBuf, pData+dSize, rSize);
		//m_iEncodeBufPos = rSize;
		//pData += rSize;
		//dataLen -= rSize;

		if (encodeOutSize > 0)
		{
			ret = m_MP4Encoder.WriteH264AudioData((const unsigned char*)m_pAACBuf, encodeOutSize, TIMESETP, m_AACEncode.GetConfig());
			HR_ASSERT(ret == 0, HRLOG_E("WriteH264AudioData err"), HRSDKCMDRST_ERR_UNKNOW);

			m_nFileTotalSize += encodeOutSize;
			if (m_nFileTotalSize >= FILEMAXSIZE)
			{
				HRLOG_W("Achieve file max size,stop record!");
				return HRSDKCMDRST_ACHIEVE_FILE_MAXSIZE;
			}
		}		
	}

	if(dataLen>0) {
		memcpy(m_pEncodeBuf+m_iEncodeBufPos, pData, dataLen);
		m_iEncodeBufPos += dataLen;
	}
	return 0;
}

#endif
