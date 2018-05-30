#include "AVIEncoder.h"
#include "BaseFunc.h"

namespace HiroCamSDK
{
	namespace Model
	{

CAVIEncoder::CAVIEncoder(void)
{
	m_hAviFile = NULL;
	m_bIsWriteMetadata = false;
	m_iAudioStreamId = m_iVideoStreamId = 0;

	memset(m_RecordFileName, 0x0, sizeof(m_RecordFileName));
	memset(&m_aviAudioStreamHeader, 0, sizeof(m_aviAudioStreamHeader));
	memset(&m_aviAudioStreamFormat, 0, sizeof(m_aviAudioStreamFormat));
	memset(&m_aviVideoStreamHeader, 0, sizeof(m_aviVideoStreamHeader));
	memset(&m_aviVideoStreamFormat, 0, sizeof(m_aviVideoStreamFormat));
	memset(&m_aviMainHeader, 0, sizeof(m_aviMainHeader));
}


CAVIEncoder::~CAVIEncoder(void)
{
	CloseAVIFile();
}

AVIFileHandle CAVIEncoder::CreateAVIFile(const char* pFileName)
{
	if(pFileName == NULL)
	{
		HRLOG_E("CreateAVIFile: pFileName is NULL!!");
		return NULL;
	}

	if (m_hAviFile)
	{
		CloseAVIFile();
	}

	AVIFileHandle hAviFile = GMAVIOpen((char*)pFileName, GMAVI_FILEMODE_CREATE, 0);
	if (hAviFile == NULL)
	{
		HRLOG_E("CreateAVIFile: Open file: %s failed!!",pFileName);
		return NULL;
	}

	StrNCpy(m_RecordFileName, (HR_CHAR*)pFileName, MAX_PATH * 2);
	m_iVideoStreamId = m_iAudioStreamId = 0;
	m_hAviFile = hAviFile;
	m_bIsWriteMetadata = false;
	return hAviFile;
}

HR_BOOL CAVIEncoder::WriteMetadata(LPAVIENC_Metadata lpMetadata)
{
	if (m_bIsWriteMetadata == true)
	{
		return HR_TRUE;
	}

	if (m_hAviFile == NULL)
	{
		HRLOG_E("WriteMetadata fail: AVI file not create!!");
		return HR_FALSE;
	}

	if (lpMetadata == NULL)
	{
		HRLOG_E("WriteMetadata fail: lpMetadata is null!!");
		return HR_FALSE;
	}

	int ret = 0;
	int type = GMAVI_TYPE_H264;
	/*if (lpMetadata->type == FOSIPC_MJ)
	{
		type = GMAVI_TYPE_MJPEG;
	}
	else if (lpMetadata->type != FOSIPC_H264)
	{
		HRLOG_E("WriteMetadata fail: device type is not support!!");
		return HR_FALSE;
	}
    */
	ret = GMAVIFillAviMainHeaderValues( &m_aviMainHeader, lpMetadata->width, lpMetadata->height, lpMetadata->frameRate, 4*1024*1024, 0 );
	ret = GMAVIFillVideoStreamHeaderValues(&m_aviVideoStreamHeader,	&m_aviVideoStreamFormat, type, lpMetadata->width, lpMetadata->height, lpMetadata->frameRate, 0, 0);
	ret = GMAVIFillAudioStreamHeaderValues(&m_aviAudioStreamHeader, &m_aviAudioStreamFormat, GMAVI_TYPE_PCM, 1, 8000, 128000, 0);
	ret = GMAVISetAviMainHeader( m_hAviFile, &m_aviMainHeader );
	ret = GMAVISetStreamHeader( m_hAviFile, &m_aviVideoStreamHeader, &m_aviVideoStreamFormat, &m_iVideoStreamId);
	ret = GMAVISetStreamHeader( m_hAviFile, &m_aviAudioStreamHeader, &m_aviAudioStreamFormat, &m_iAudioStreamId);

	m_bIsWriteMetadata = true;

	return HR_TRUE;
}

HR_S32 CAVIEncoder::WriteVideoData(const unsigned char* pData,int size, uint64_t timeStamp, int isKey)
{
	int iRet = 0;

	if( m_hAviFile == NULL || pData == NULL )
	{
		return GMSTS_INVALID_INPUT;
	}

	iRet = WriteAVIStreamData( m_iVideoStreamId, (char*)pData, size, isKey, (uint32_t)timeStamp);
	if (0 == iRet)
	{
		m_aviMainHeader.dwTotalFrames += 1;
		iRet = GMAVISetAviMainHeader( m_hAviFile, &m_aviMainHeader );
		if (0 == iRet)
		{
			m_aviVideoStreamHeader.dwLength += 1;
			iRet = GMAVIUpdateStreamHeader( m_hAviFile, 0, &m_aviVideoStreamHeader, &m_aviVideoStreamFormat );
		}
	}

	return iRet;
}

HR_S32 CAVIEncoder::WriteAudioData(const unsigned char* pData,int size, int timeSetp)
{
	int iRet = 0;

	if( m_hAviFile == NULL || pData == NULL )
	{
		return GMSTS_INVALID_INPUT;
	}

	iRet = WriteAVIStreamData( m_iAudioStreamId, (char*)pData, size, 0, timeSetp);
	if (0 == iRet)
	{
		m_aviAudioStreamHeader.dwLength += (size / m_aviAudioStreamHeader.dwSampleSize);
		iRet = GMAVIUpdateStreamHeader( m_hAviFile, 1, &m_aviAudioStreamHeader, &m_aviAudioStreamFormat );
	}

	return iRet;
}

HR_VOID CAVIEncoder::CloseAVIFile()
{
	m_bIsWriteMetadata = false;
	m_iVideoStreamId = m_iAudioStreamId = 0;
	
	if (m_hAviFile)
	{
		GMAVIClose( m_hAviFile );
		m_hAviFile = NULL;

		if (strlen(m_RecordFileName) > 0)
		{
			//删除索引文件
			char buf[MAX_PATH * 3] = {0};
			sprintf(buf, "%s_idx", m_RecordFileName);
			remove(buf);
			memset(m_RecordFileName, 0x0, sizeof(m_RecordFileName));
		}
	}
}

HR_S32 CAVIEncoder::WriteAVIStreamData(int streamId, char* buf, int len, int isKeyFrame, unsigned int timeStamp)
{
	int ret = -1;
	GMExtraInfo extraInfo;
	extraInfo.timestamp = timeStamp;
	extraInfo.profile_index = 0;
	extraInfo.flag = isKeyFrame;

	ret = GMAVISetStreamDataAndIndex( m_hAviFile,
		streamId,
		(unsigned char*)buf,
		len,
		isKeyFrame,
		/*(unsigned char*)&extraInfo*/NULL,
		/*sizeof(GMExtraInfo)*/0, 
		512*1024*1024);							//这个是文件的总大小，上层限制的文件内容的大小应该小于此处大小值，不然会发生文件尾部信息无法写入而生成错误文件。hongweihuan.2015.06.10.  add：之前大小值为257，发现还是容易出现文件尾部写入错误的问题，所以改为512m，确保能够写入文件信息。

	if( ret == GMSTS_DATA_FULL )
	{
		// restart recording
		return GMSTS_EXCEED_MAX_SIZE;
	}
	return ret;
}

	}
}

