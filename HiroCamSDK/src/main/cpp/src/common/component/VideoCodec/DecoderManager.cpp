#ifndef DECODE_VIDEO
#include "DecoderManager.h"
#ifndef INT64_C
#define INT64_C   
#define UINT64_C
#endif

extern "C"
{
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#include <stdint.h>
#endif

#ifdef HAVE_AV_CONFIG_H
#undef HAVE_AV_CONFIG_H
#endif
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
}

MUTEX DecoderManager::s_mutexInstance;

void DecoderManager::InitDecordManager()
{
	INIT_MUTEX(&s_mutexInstance);
}

void DecoderManager::ReleaseDecordManager()
{
	RELEASE_MUTEX(&s_mutexInstance);
}

DecoderManager::DecoderManager(void)
{
	m_pDecoderInst=NULL;
	m_frameWidth=0;
	m_frameHeigth=0;
	m_outBuffer=NULL;
	m_frameDataSize = 0;
	m_headSize = 0;
	m_avCodecId = AV_Decoder_ID_H264;

	INIT_MUTEX(&m_captureLock);
}

DecoderManager::~DecoderManager(void)
{
	try{
		if (m_pDecoderInst!=NULL)
		{
			DestroyDecoderInstance();
		}
		m_pDecoderInst = NULL;
		m_outBuffer = NULL;
		RELEASE_MUTEX(&m_captureLock);
	}catch(...){}
}

void *DecoderManager::CreateDecoderInstance(VCodecID decoderId, HR_U32 headsize)//创建解码器实例
{
	CAutoLock lock(s_mutexInstance);//对DecoderManager类共享锁加锁，对ffmpeg接口进行线程保护
	AVCodecContext *codecCtx; 
	AVCodec *codec; 
	AVCodecID avCodecID;
	if (m_pDecoderInst!=NULL)
	{
		return NULL;
	}
	m_headSize = headsize;
	m_avCodecId=decoderId;
	switch(decoderId)
	{
	case AV_Decoder_ID_MJPEG:
		avCodecID=AV_CODEC_ID_MJPEG;
		break;
	case AV_Decoder_ID_H264:
		avCodecID=AV_CODEC_ID_H264;
		break;
	default:
		return NULL;
	}
	m_pDecoderInst =new DecoderInstance;
	memset(m_pDecoderInst, 0x00, sizeof(DecoderInstance));
	//av_register_all();
	avcodec_register_all();
#ifdef _MAC
	m_pDecoderInst->avframe=avcodec_alloc_frame();//av_frame_alloc();
	m_pDecoderInst->pDstFrame=avcodec_alloc_frame();//av_frame_alloc();
#else
	m_pDecoderInst->avframe=av_frame_alloc();
	m_pDecoderInst->pDstFrame=av_frame_alloc();
#endif
	codec = avcodec_find_decoder(avCodecID); 
	codecCtx = avcodec_alloc_context3(codec);
	m_pDecoderInst->avcodeContext=codecCtx;
	if(!codec || (avcodec_open2(codecCtx, codec,NULL) < 0))  
	{
#ifdef _WIN32
		avcodec_free_context(&codecCtx);
#else
		avcodec_close(codecCtx);
		av_free(codecCtx);
#endif
		delete m_pDecoderInst;
		m_pDecoderInst=NULL;
		HRLOG_E("Failed to call avcodec_open2()\n");
		return 0;  
	}
	return (void *)m_pDecoderInst;
}

HR_S32 DecoderManager::DecoderVideo(HR_U8 *inBuffer,HR_U32 inBufsize,HR_U8 **outBuffer,HR_U32* outBufferSize,HR_U32 *width, HR_U32 *height, HRDECFMT pixelFormat)//循环解码
{
	HR_S32 frameFinished = 0;
	AVPacket pkt1,*pkt=&pkt1;
	if(m_pDecoderInst == NULL)
	{
		return -1;
	}
	AVFrame *pFrame=(AVFrame *)m_pDecoderInst->avframe;
	AVCodecContext *avctx=(AVCodecContext *)m_pDecoderInst->avcodeContext;
	AVPixelFormat pixformat=(AVPixelFormat)pixelFormat;
	memset(pkt,0,sizeof(AVPacket));
	switch(pixelFormat)
	{
	case HRDECTYPE_RGB24:
		pixformat=AV_PIX_FMT_RGB24;
		break;
	case HRDECTYPE_YUV420:
		pixformat=AV_PIX_FMT_YUV420P;
		break;
	case HRDECTYPE_YUYV422:
		pixformat=AV_PIX_FMT_YUYV422;
		break;
	case HRDECTYPE_UYVY422:
		pixformat=AV_PIX_FMT_UYVY422;
		break;
	case HRDECTYPE_RGBA32:
		pixformat=AV_PIX_FMT_RGBA;
		break;
	case HRDECTYPE_ARGB32:
		pixformat=AV_PIX_FMT_ARGB;
		break;
	case HRDECTYPE_ABGR32:
		pixformat=AV_PIX_FMT_ABGR;
		break;
	case HRDECTYPE_BGRA32:
		pixformat=AV_PIX_FMT_BGRA;
		break;
	case HRDECTYPE_BGR24:
		pixformat=AV_PIX_FMT_BGR24;
		break;
	case HRDECTYPE_RGB565BE:
		pixformat=AV_PIX_FMT_RGB565BE;
		break;
	case HRDECTYPE_RGB565LE:
		pixformat=AV_PIX_FMT_RGB565LE;
		break;
	case HRDECTYPE_BGR565BE:
		pixformat=AV_PIX_FMT_BGR565BE;
		break;
	case HRDECTYPE_BGR565LE:
		pixformat=AV_PIX_FMT_BGR565LE;
		break;
	default:
		HRLOG_E("Failed to call DecoderVideo(),unsupport pix_format\n");
		return -1;
	}
	pkt->data=inBuffer;
	pkt->size=inBufsize;
	LOCK_MUTEX(&m_captureLock);
	avcodec_decode_video2(avctx, pFrame, &frameFinished,pkt);  // Decode video frame 
	UNLOCK_MUTEX(&m_captureLock);
	if (frameFinished==1)//解码出yuv帧数据
	{
		AVFrame *pDstFrame=(AVFrame *)m_pDecoderInst->pDstFrame;
		struct SwsContext *img_convert_ctx=(struct SwsContext *)m_pDecoderInst->img_convert_ctx;
		if (img_convert_ctx==NULL)
		{
			CAutoLock lock(s_mutexInstance);//对DecoderManager类共享锁加锁，对ffmpeg接口进行线程保护
			m_frameDataSize=avpicture_get_size(pixformat, avctx->width, avctx->height);
			m_outBuffer=(HR_U8 *)malloc(m_frameDataSize + m_headSize);
			if(m_outBuffer == NULL)
			{
				return -1;
			}
			avpicture_fill((AVPicture *)pDstFrame,m_outBuffer+m_headSize , pixformat, avctx->width, avctx->height);  
			img_convert_ctx = sws_getContext(avctx->width, avctx->height, avctx->pix_fmt, avctx->width, avctx->height, pixformat, SWS_FAST_BILINEAR, NULL, NULL, NULL);   
			m_pDecoderInst->img_convert_ctx=img_convert_ctx;
		}
		else
		{
			if (m_frameWidth!=avctx->width||m_frameHeigth!=avctx->height)
			{
				//LOCK_MUTEX(&s_mutexInstance); //对DecoderManager类共享锁加锁，对ffmpeg接口进行线程保护
				//m_frameDataSize=avpicture_get_size(pixformat, avctx->width, avctx->height);
				//free(m_outBuffer);
				//m_outBuffer=(HR_U8 *)malloc(m_frameDataSize + m_headSize);
				//avpicture_fill((AVPicture *)pDstFrame, m_outBuffer+m_headSize, pixformat, avctx->width, avctx->height);
				//sws_freeContext((struct SwsContext *)m_pDecoderInst->img_convert_ctx);
				//img_convert_ctx = sws_getContext(avctx->width, avctx->height, avctx->pix_fmt, avctx->width, avctx->height, pixformat, SWS_FAST_BILINEAR, NULL, NULL, NULL);   
				//UNLOCK_MUTEX(&s_mutexInstance);
				//m_pDecoderInst->img_convert_ctx=img_convert_ctx;
				CAutoLock lock(m_captureLock);
				DestroyDecoderInstance();
				CreateDecoderInstance(m_avCodecId,m_headSize);
                avcodec_decode_video2((AVCodecContext *)m_pDecoderInst->avcodeContext, (AVFrame *)m_pDecoderInst->avframe, &frameFinished,pkt);
				return 0;
			}
			else if (pixformat != avctx->pix_fmt)
			{
				CAutoLock lock(s_mutexInstance);//对DecoderManager类共享锁加锁，对ffmpeg接口进行线程保护
				m_frameDataSize=avpicture_get_size(pixformat, avctx->width, avctx->height);
				free(m_outBuffer);
				m_outBuffer=(HR_U8 *)malloc(m_frameDataSize + m_headSize);
				if(m_outBuffer == NULL)
				{
					return -1;
				}
				avpicture_fill((AVPicture *)pDstFrame, m_outBuffer+m_headSize, pixformat, avctx->width, avctx->height);
				sws_freeContext((struct SwsContext *)m_pDecoderInst->img_convert_ctx);
				img_convert_ctx = sws_getContext(avctx->width, avctx->height, avctx->pix_fmt, avctx->width, avctx->height, pixformat, SWS_FAST_BILINEAR, NULL, NULL, NULL);   
				m_pDecoderInst->img_convert_ctx=img_convert_ctx;
			}
		}
		LOCK_MUTEX(&m_captureLock);
		sws_scale(img_convert_ctx, (const HR_U8* const*)pFrame->data, pFrame->linesize, 0, avctx->height, pDstFrame->data, pDstFrame->linesize); 
		UNLOCK_MUTEX(&m_captureLock);
		m_frameWidth=avctx->width;
		m_frameHeigth=avctx->height;
		*outBuffer=m_outBuffer;
		*outBufferSize=m_frameDataSize;
		*width=avctx->width;
		*height=avctx->height;
		return 1;
	}
	return 0;
}

HR_S32 DecoderManager::DestroyDecoderInstance()//释放解码器实例
{
	CAutoLock lock(s_mutexInstance);//对DecoderManager类共享锁加锁，对ffmpeg接口进行线程保护
	AVCodecContext *avctx;
	AVFrame *pFrame;
	if (m_pDecoderInst==NULL)
	{
		HRLOG_E("Failed to call DestroyDecoderInstance(),null pointer\n");
		return -1;
	}
	avctx=(AVCodecContext *)m_pDecoderInst->avcodeContext;			
	if (m_pDecoderInst->img_convert_ctx!=NULL)
	{
		sws_freeContext((struct SwsContext *)m_pDecoderInst->img_convert_ctx);
	}
	pFrame=(AVFrame *)m_pDecoderInst->avframe;
#ifdef _MAC
	avcodec_free_frame(&pFrame);
#else
	av_frame_free(&pFrame);
#endif   
	pFrame=(AVFrame *)m_pDecoderInst->pDstFrame;
#ifdef _MAC
	avcodec_free_frame(&pFrame);
#else
	av_frame_free(&pFrame);
#endif
	avcodec_close(avctx);
#ifdef _WIN32
	if (avctx) {
		avcodec_free_context(&avctx);
		avctx = NULL;
	}
#else
	if (avctx) {
		av_free(avctx);
		avctx = NULL;
	}
#endif

	HR_SAFACE_FREE(m_outBuffer);
	HR_SAFACE_DELETE(m_pDecoderInst);
	return 0;
}

HR_S32 DecoderManager::SavePicture(const HR_CHAR* picturePath)
{
	if (strlen(picturePath)<MAX_PATH)
	{
		if (m_pDecoderInst && m_pDecoderInst->avframe!=NULL&&m_frameWidth!=0&&m_frameHeigth!=0)
		{
			return CaptureFrame(m_frameWidth,m_frameHeigth,NULL,0,picturePath);
		}
	}
	return -1;
}

HR_S32 DecoderManager::CapturePicture(HR_U8 *mjpegBuffer,HR_S32 bufferSize,HR_S32 width,HR_S32 height)
{
	if (m_pDecoderInst && m_pDecoderInst->avframe!=NULL&&m_frameWidth!=0&&m_frameHeigth!=0)
	{
		return CaptureFrame(m_frameWidth,m_frameHeigth,mjpegBuffer,bufferSize,NULL,width,height);
	}
	return -1;
}

HR_S32 DecoderManager::CaptureFrame(HR_S32 width,HR_S32 height,HR_U8 *mjpegBuffer,HR_S32 bufferSize, const HR_CHAR* picturePath,HR_S32 dstWidth,HR_S32 dstHeight)
{   
	CAutoLock lock(s_mutexInstance);//对DecoderManager类共享锁加锁，对ffmpeg接口进行线程保护
	AVCodecContext* pCodecCtx;  
	AVCodec* pCodec; 
	AVPacket pkt;  
	HR_S32 ret =0 ;
	HR_S32 got_picture=0;
	if (dstWidth <= 0 || dstHeight <= 0)
	{
		dstWidth = width;
		dstHeight = height;
	}
	//av_register_all(); 
	pCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (!pCodec) {
        HRLOG_E("Failed to call avcodec_find_encoder(AV_CODEC_ID_MJPEG)\n");
        return -1;
    }
	pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx) {
        HRLOG_E("Failed to call avcodec_alloc_context3(pCodec)\n");
		return -1;
    }
	AVCodecContext *pCodecontext = NULL;
	if (m_pDecoderInst)
	{
		pCodecontext = (AVCodecContext *)m_pDecoderInst->avcodeContext;
	}
	if(pCodecontext && pCodecontext->pix_fmt == PIX_FMT_YUVJ422P)
	{
		pCodecCtx->pix_fmt = pCodecontext->pix_fmt; 
	}
	else
	{
		pCodecCtx->pix_fmt = PIX_FMT_YUVJ420P;
	}
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;  
	pCodecCtx->width = dstWidth;    
	pCodecCtx->height = dstHeight;  
	pCodecCtx->time_base.num = 1;    
	pCodecCtx->time_base.den = 25;     
	if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0)  
	{  
#ifdef _WIN32
		avcodec_free_context(&pCodecCtx);
#else
		avcodec_close(pCodecCtx);
		av_free(pCodecCtx);
#endif
		HRLOG_E("Failed to call avcodec_open2()\n");
		return -1;  
	}     
	av_new_packet(&pkt,dstWidth * dstHeight*3);   
	LOCK_MUTEX(&m_captureLock);
	if (m_pDecoderInst && m_pDecoderInst->avframe && m_outBuffer)
	{
		AVFrame *pSrcFrame = (AVFrame *)m_pDecoderInst->avframe;
		if (pSrcFrame->width <= 0 || pSrcFrame->height <= 0)
		{
			ret = -1;
		}
		else if (dstWidth != width || dstHeight != height)
		{
			AVCodecContext *pSrcCodecontext = (AVCodecContext *)m_pDecoderInst->avcodeContext;
			int frameDataSize=avpicture_get_size(pSrcCodecontext->pix_fmt, dstWidth, dstHeight);
			HR_U8 *outBuffer=(HR_U8 *)malloc(frameDataSize);
			if(outBuffer != NULL)
			{
#ifdef _MAC
				AVFrame *pDstFrame=avcodec_alloc_frame();
#else
				AVFrame *pDstFrame=av_frame_alloc();
#endif
				avpicture_fill((AVPicture *)pDstFrame,outBuffer , pSrcCodecontext->pix_fmt, dstWidth, dstHeight);  
				struct SwsContext *img_convert_ctx = sws_getContext(pSrcFrame->width, pSrcFrame->height, 
					pSrcCodecontext->pix_fmt, dstWidth, dstHeight, pSrcCodecontext->pix_fmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);
                if (img_convert_ctx) {
                    ret = sws_scale(img_convert_ctx, (const HR_U8* const*)pSrcFrame->data, pSrcFrame->linesize,
                                    0, pSrcCodecontext->height, pDstFrame->data, pDstFrame->linesize);
                    ret = avcodec_encode_video2(pCodecCtx, &pkt, pDstFrame, &got_picture);  //编码
                    sws_freeContext(img_convert_ctx);
                }
#ifdef _MAC
				avcodec_free_frame(&pDstFrame);
#else
				av_frame_free(&pDstFrame);
#endif
				free(outBuffer);
			}
		}
		else
		{
			ret = avcodec_encode_video2(pCodecCtx, &pkt,pSrcFrame, &got_picture);  //编码  
		}
	}
	UNLOCK_MUTEX(&m_captureLock);
	if (got_picture==1)  
	{  
		if (mjpegBuffer!=NULL&&bufferSize>=pkt.size)
		{
			memcpy(mjpegBuffer,pkt.data,pkt.size);
			ret = pkt.size;
		}
		if (picturePath!=NULL)
		{
			HR_FILE *pf=HR_FILE_OPEN(picturePath,"w+b");
			if (NULL == pf)
			{
				HRLOG_E("the path of picture is error when captured,file opened error\n");
				ret = -1;
			}
			else
			{
				HR_FILE_WRITE(pkt.data,1,pkt.size,pf);
				HR_FILE_CLOSE(pf);
				ret =pkt.size;
			}
		}
	}   
	av_free_packet(&pkt);  
#ifdef _WIN32
	avcodec_free_context(&pCodecCtx);
#else
	avcodec_close(pCodecCtx);
	av_free(pCodecCtx);
#endif
	return ret;  
}
#endif //ECODE_VIDEO
