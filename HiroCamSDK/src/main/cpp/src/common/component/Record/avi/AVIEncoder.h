#ifndef __AVI_ENCODER___H__
#define __AVI_ENCODER___H__

#include "BaseDataType.h"
#include "gmavi.h"
#include "gmavi_api.h"
#include "gmtypes.h"
#include "HiroCamDef.h"

typedef void* AVIFileHandle;

namespace HiroCamSDK
{
	namespace Model
	{

typedef struct _AVIENC_Metadata  
{  
	unsigned int width;
	unsigned int height;
	unsigned int frameRate;
} AVIENC_Metadata,*LPAVIENC_Metadata;  

class CAVIEncoder
{
public:
	CAVIEncoder(void);
	~CAVIEncoder(void);

	// open or creat a avi file.  
	AVIFileHandle CreateAVIFile(const char* pFileName);
	// wirte metadata in avi file.   
	HR_BOOL WriteMetadata(LPAVIENC_Metadata lpMetadata);

	// wirte data, data can contain  multiple frame.  
	HR_S32 WriteVideoData(const unsigned char* pData,int size, uint64_t timeStamp, int isKey);
	HR_S32 WriteAudioData(const unsigned char* pData,int size, int timeSetp);

	// close avi file.   
	HR_VOID CloseAVIFile();

private:
	HR_S32 WriteAVIStreamData(int streamId, char* buf, int len, int isKeyFrame, unsigned int timeStamp);

private:
	AVIFileHandle m_hAviFile;

	char		m_RecordFileName[MAX_PATH * 2];
	int		m_iAudioStreamId;
	int		m_iVideoStreamId;

	bool		m_bIsWriteMetadata;
	AviStreamHeader    m_aviAudioStreamHeader;
	GmAviStreamFormat  m_aviAudioStreamFormat;
	AviStreamHeader    m_aviVideoStreamHeader;
	GmAviStreamFormat  m_aviVideoStreamFormat;
	AviMainHeader      m_aviMainHeader;
};

	}
}
#endif

