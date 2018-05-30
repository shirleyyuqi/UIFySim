#ifndef __MP4_ENCODER___H__
#define __MP4_ENCODER___H__
#ifdef _USE_MP4
#include "mp4v2/mp4v2.h"
#include "BaseDataType.h"

namespace HiroCamSDK
{
	namespace Model
	{

#define BUFFER_SIZE  (1024*1024)
typedef struct _MP4ENC_NaluUnit  
{
	int type;  
	int size;  
	unsigned char *data;  
}MP4ENC_NaluUnit;  

typedef struct _MP4ENC_Metadata  
{  
	unsigned int width;
	unsigned int height;
	// video, must be h264 type   
	unsigned int    nSpsLen;  
	unsigned char   Sps[64];  
	unsigned int    nPpsLen;  
	unsigned char   Pps[1024];  

} MP4ENC_Metadata,*LPMP4ENC_Metadata;  

class CMP4Encoder  
{  
public:  
	CMP4Encoder(void);  
	~CMP4Encoder(void);  
public:  
	// open or creat a mp4 file.   
	MP4FileHandle CreateMP4File(const char *fileName,int timeScale = 90000,int frameRate = 30);
	
	// wirte 264 metadata in mp4 file.   
	HR_BOOL Write264Metadata(LPMP4ENC_Metadata lpMetadata);
	
	// wirte 264 data, data can contain  multiple frame.   
	HR_S32 WriteH264VideoData(const unsigned char* pData,int size, uint64_t timeStamp, int isKey);
	HR_S32 WriteH264AudioData(const unsigned char* pData,int size, int timeSetp, short aacConfig, int rate=8000);
	
	// close mp4 file.   
	HR_VOID CloseMP4File();
	
	// Prase H264 metamata from H264 data frame   
	HR_BOOL PraseMetadata(const unsigned char* pData,unsigned int size,MP4ENC_Metadata &metadata);
	HR_U64 getMP4FileDuration(void);
private:  
	// read one nalu from H264 data buffer   
	static HR_S32 ReadOneNaluFromBuf(const unsigned char *buffer,unsigned int nBufferSize,unsigned int offSet,MP4ENC_NaluUnit &nalu);
private:  
	int m_nFrameRate;  
	int m_nTimeScale;  
	unsigned char * m_dataBuf;
	MP4FileHandle m_hMp4file;
	MP4TrackId m_videoId;  
	MP4TrackId m_audioId; 
	uint64_t m_lastVideoTimeStamp;
};

	}
}
#endif
#endif
