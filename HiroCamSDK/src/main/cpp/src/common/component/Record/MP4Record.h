#pragma once
#include "IRecord.h"
#include "AudioCodec.h"
#include "mp4/MP4Encoder.h"

#ifdef _USE_MP4

class CMP4Record : public IRecord
{
public:
	CMP4Record();
	virtual ~CMP4Record();

	virtual HRSDK_CMDRESULT OpenFile(const HR_CHAR* filename);
	virtual HR_S32 WriteVideo(HRSDK_DecFrame* data);
	virtual HR_S32 WriteAudio(HRSDK_DecFrame* data);
	virtual HRSDK_CMDRESULT CloseFile();
protected:
	HiroCamSDK::Model::CMP4Encoder m_MP4Encoder;
	HR_BOOL		m_bHasRecordKeyFrameGot;
	HR_S32		m_nVideoWidth;
	HR_S32		m_nVideoHeight;
	HR_S64		m_nFileTotalSize;
    
    HR_BOOL     m_bFrist;
    HR_U64      m_iFristPts;

	AudioCodec	m_AACEncode;
	HR_BOOL		m_isInitAACEncode;
	HR_U32		m_nEncoderSize;
	HR_CHAR*	m_pEncodeBuf;
	HR_CHAR*	m_pAACBuf;
	HR_U32		m_iEncodeBufPos;
};
#endif


