#include "MP4Encoder.h"
#ifdef _USE_MP4
namespace HiroCamSDK
{
	namespace Model
	{

CMP4Encoder::CMP4Encoder(void):
m_videoId(0),
m_nTimeScale(0),
m_nFrameRate(0)
{
	m_audioId = 0;
  	m_dataBuf = new unsigned char[500<<12];
	m_videoId = MP4_INVALID_TRACK_ID;
	m_hMp4file = NULL;
	m_lastVideoTimeStamp = 0;
}

CMP4Encoder::~CMP4Encoder(void)
{
	delete [] m_dataBuf;
}

MP4FileHandle CMP4Encoder::CreateMP4File(const char *pFileName,int timeScale/* = 90000*/,int frameRate/* = 30*/)
{
	if(pFileName == NULL)
	{
		HRLOG_E("CreateMP4File: pFileName is NULL!!");
		return NULL;
	}
	// create mp4 file
	MP4FileHandle hMp4file = MP4Create(pFileName);
	if (hMp4file == MP4_INVALID_FILE_HANDLE)
	{
        HRLOG_E("CreateMP4File: Open file: %s failed!!",pFileName);
		return NULL;
	}
	m_lastVideoTimeStamp = 0;
    m_nTimeScale = 1000;//timeScale;
	m_nFrameRate = 25;//frameRate;
	m_hMp4file   = hMp4file;
	MP4SetTimeScale(m_hMp4file, m_nTimeScale);
	m_videoId    = MP4_INVALID_TRACK_ID;
	return hMp4file;
}

HR_BOOL CMP4Encoder::Write264Metadata(LPMP4ENC_Metadata lpMetadata)
{
	if( m_videoId == MP4_INVALID_TRACK_ID )
	{
		m_videoId = MP4AddH264VideoTrack(m_hMp4file,
                                         m_nTimeScale,
                                         m_nTimeScale / m_nFrameRate,
                                         lpMetadata->width, // width
                                         lpMetadata->height,// height
                                         lpMetadata->Sps[1], // sps[1] AVCProfileIndication
                                         lpMetadata->Sps[2], // sps[2] profile_compat
                                         lpMetadata->Sps[3], // sps[3] AVCLevelIndication
                                         3);           // 4 bytes length before each NAL unit
		if (m_videoId == MP4_INVALID_TRACK_ID)
		{
			HRLOG_E("CreateMP4File: Add video track failed!!");
			return HR_FALSE;
		}
	}
    MP4SetTrackName(m_hMp4file, m_videoId, "");
	//LOGD("Add video track, width:%d, height:%d", lpMetadata->width, lpMetadata->height);
	MP4SetVideoProfileLevel(m_hMp4file, 0x7F); //  Simple Profile @ Level 3
    
	// write sps
	MP4AddH264SequenceParameterSet(m_hMp4file,m_videoId,lpMetadata->Sps,lpMetadata->nSpsLen);
    
	// write pps
    MP4AddH264PictureParameterSet(m_hMp4file,m_videoId,lpMetadata->Pps,lpMetadata->nPpsLen);
    
	return HR_TRUE;
}

HR_S32 CMP4Encoder::WriteH264VideoData(const unsigned char* pData,int size, uint64_t timeStamp, int isKey)
{
	if(pData == NULL)
	{
		HRLOG_E("WriteH264VideoData: pData is NULL!!");
		return -1;
	}
	MP4ENC_NaluUnit nalu;
	int pos = 0, len = 0;
	int datalen;
	uint64_t sampleDuration = 0;
	if( m_lastVideoTimeStamp == 0 ){
		//sampleDuration = m_nTimeScale/33;
	}else{
		if( timeStamp >= m_lastVideoTimeStamp){
			sampleDuration = m_nTimeScale*(timeStamp-m_lastVideoTimeStamp)/1000;
		}else{
			sampleDuration = m_nTimeScale*(m_lastVideoTimeStamp-timeStamp)/1000;
		}
	}
	m_lastVideoTimeStamp = timeStamp;
    
	while( 1 )
	{
		len = ReadOneNaluFromBuf(pData,size,pos,nalu);
		if( len <= 0 ){
			break;
		}
		if(nalu.type == 0x07 ) // sps
		{
			pos += len;
			continue;
		}
		else if(nalu.type == 0x08) // pps
		{
			pos += len;
			continue;
		}
		else if (nalu.type == 0x06)
		{
			pos += len;
			continue;
		}
		else
		{
			datalen = nalu.size+4;
			// MP4 Nalu前四个字节表示Nalu长度
			m_dataBuf[0] = nalu.size>>24;
			m_dataBuf[1] = nalu.size>>16;
			m_dataBuf[2] = nalu.size>>8;
			m_dataBuf[3] = nalu.size&0xff;
			memcpy(m_dataBuf+4,nalu.data,nalu.size);
			if(!MP4WriteSample(m_hMp4file, m_videoId, m_dataBuf, datalen, sampleDuration, 0, isKey))
			{
				HRLOG_E("WriteH264VideoData: MP4WriteSample failed!!");
				return 0;
			}
		}
        
		pos += len;
	}
	return pos;
}

HR_S32 CMP4Encoder::WriteH264AudioData(const unsigned char* pData, int size, int timeSetp, short aacConfig, int rate)
{
	if(m_audioId == MP4_INVALID_TRACK_ID)
	{
		//m_audioId = MP4AddAudioTrack(m_hMp4file, 8000, 1000, MP4_MPEG2_AAC_LC_AUDIO_TYPE);
		m_audioId = MP4AddAudioTrack(m_hMp4file, rate, 100, MP4_MPEG4_AUDIO_TYPE);
		if (m_audioId == MP4_INVALID_TRACK_ID)
		{
			HRLOG_E("WriteH264AudioData: Add audio track failed!!");
			return -1;
		}
        
        MP4SetAudioProfileLevel(m_hMp4file, 0x2);
		MP4SetTrackESConfiguration(m_hMp4file, m_audioId, (const unsigned char* )&aacConfig, 2);
        
        MP4ChangeMovieTimeScale(m_hMp4file, 1000);
	}
    
	// 不要包含ADTS头 MP4_INVALID_DURATION
	//if(!MP4WriteSample(m_hMp4file, m_audioId, (pData+7), (size-7), timeSetp, 0, 1))
	//{
	//	HRLOG_E("WriteH264AudioData: MP4WriteSample failed!!");
	//	return -1;
	//}
	if(!MP4WriteSample(m_hMp4file, m_audioId, (pData+7), (size-7), timeSetp, 0, 1))
	{
		HRLOG_E("WriteH264AudioData: MP4WriteSample failed!!");
		return -1;
	}
    
	return 0;
}

HR_S32 CMP4Encoder::ReadOneNaluFromBuf(const unsigned char *buffer,unsigned int nBufferSize,unsigned int offSet,MP4ENC_NaluUnit &nalu)
{
	unsigned int i = offSet;
	while(i < nBufferSize && nBufferSize>4)
	{
		if(buffer[i++] == 0x00 &&
           buffer[i++] == 0x00 &&
           buffer[i++] == 0x00 &&
           buffer[i++] == 0x01
           )
		{
			unsigned int pos = i;
			unsigned int len = 0;
			while (pos<nBufferSize)
			{
				if( (buffer[pos++] == 0x00 && pos<nBufferSize) &&
                    (buffer[pos++] == 0x00 && pos<nBufferSize) &&
                    (buffer[pos++] == 0x00 && pos<nBufferSize) &&
                    buffer[pos++] == 0x01
                   )
				{
					break;
				}
			}

			if(pos == nBufferSize)
			{
				nalu.size = pos-i;
			}
			else
			{
				nalu.size = (pos-4)-i;
			}
            
			nalu.type = buffer[i]&0x1f;
			nalu.data =(unsigned char*)&buffer[i];
			return (nalu.size+i-offSet);
		}
	}
	return 0;
}

HR_VOID CMP4Encoder::CloseMP4File()
{
	if(m_hMp4file)
	{
        MP4AddTrackEdit(m_hMp4file, m_audioId, 0, 0, MP4GetDuration(m_hMp4file), 0);
        MP4AddTrackEdit(m_hMp4file, m_videoId, 0, 0, MP4GetDuration(m_hMp4file), 0);
		MP4Close(m_hMp4file);
		m_hMp4file = NULL;
	}
    
	m_videoId = MP4_INVALID_TRACK_ID;
	m_audioId = MP4_INVALID_TRACK_ID;
}

HR_BOOL CMP4Encoder::PraseMetadata(const unsigned char* pData, unsigned int size,MP4ENC_Metadata &metadata)
{
	if(pData == NULL || size<4)
	{
		if (pData == NULL)
		{
			HRLOG_E("PraseMetadata: pData is NULL!!");
		}
		else
		{
			HRLOG_E("PraseMetadata: size less than 4 !!");
		}
		return HR_FALSE;
	}
	MP4ENC_NaluUnit nalu;
	int pos = 0;
	bool bRet1 = false;
	bool bRet2 = false;
    
	while (int len = ReadOneNaluFromBuf(pData,size,pos,nalu))
	{
		if(nalu.type == 0x07)
		{
			memcpy(metadata.Sps,nalu.data,nalu.size);
			metadata.nSpsLen = nalu.size;
			bRet1 = true;
		}
		else if((nalu.type == 0x08))
		{
			memcpy(metadata.Pps,nalu.data,nalu.size);
			metadata.nPpsLen = nalu.size;
			bRet2 = true;
		}
		pos += len;
        
		if(bRet1 && bRet2)
		{
			return HR_TRUE;
		}
	}
    HRLOG_E("PraseMetadata failed!!");
	return HR_FALSE;
}

HR_U64 CMP4Encoder::getMP4FileDuration(void)
{
	if( m_hMp4file == NULL )
	{
		HRLOG_E("getMP4FileDuration: m_hMp4file is NULL!!");
		return 0;
	}
    
	return MP4GetDuration(m_hMp4file)/m_nTimeScale;
}

	}
}
#endif
