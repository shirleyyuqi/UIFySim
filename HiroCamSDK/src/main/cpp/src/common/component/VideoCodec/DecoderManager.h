#ifndef PATH_DECODERMANAGER_H
#define PATH_DECODERMANAGER_H
#pragma once
#include "BaseDataType.h"
#include "HiroCamDef.h"
#include "AutoLock.h"

#ifndef MAX_PATH
#define MAX_PATH				260
#endif
using namespace HiroCamSDK;

#ifndef DECODE_VIDEO

enum VCodecID{
	AV_Decoder_ID_MJPEG,
	AV_Decoder_ID_H264
};

//enum FOSPixelFormat
//{
//	HR_PIX_FMT_YUV420P,
//	HR_PIX_FMT_YUV422P,
//	HR_PIX_FMT_RGB24,
//	HR_PIX_FMT_ARGB
//};

typedef struct DecoderInstance
{
	void* avcodeContext;
	void* avframe;
	void* pDstFrame;
	void* img_convert_ctx;
	DecoderInstance ()
	{
		avcodeContext=NULL;
		avframe=NULL;
		pDstFrame=NULL;
		img_convert_ctx=NULL;
	}
}DecoderInstance;

class DecoderManager
{
public:
    static void InitDecordManager();
    static void ReleaseDecordManager();
    
	DecoderManager(void);
	~DecoderManager(void);
	void *CreateDecoderInstance(VCodecID decoderId, HR_U32 headsize=0);//创建解码器实例
	HR_S32 DecoderVideo(HR_U8 *inBuffer,HR_U32 inBufsize,HR_U8 **outBuffer,HR_U32* outBufferSize,HR_U32 *width,HR_U32 *height, HRDECFMT pixelFormat = HRDECTYPE_YUV420);//解码原始数据成CreateDecoderInstance中设定的像素格式
	HR_S32 DestroyDecoderInstance();//释放解码器实例
	HR_S32 SavePicture(const HR_CHAR* picturePath);//截图
	HR_S32 CapturePicture(HR_U8 *mjpegBuffer,HR_S32 bufferSize,HR_S32 width = 0,HR_S32 height = 0);//截图
	
private:
	DecoderInstance *m_pDecoderInst;
	HR_U32 m_frameWidth;
	HR_U32 m_frameHeigth;
	VCodecID m_avCodecId;
	HR_U32 m_frameDataSize;
	HR_U8 *m_outBuffer; 
	HR_U32	m_headSize;

    static MUTEX s_mutexInstance;//对象共享锁
    
	MUTEX m_captureLock;//对象私有锁，当解码线程和截图线程都使用yuv图像缓存时，避免不安全
	HR_S32 CaptureFrame(HR_S32 width,HR_S32 height,HR_U8 *mjpegBuffer,HR_S32 bufferSize, const HR_CHAR* picturePath=NULL,HR_S32 dstWidth = 0,HR_S32 dstHeight = 0);
};
#endif

#endif //DECODE_VIDEO
