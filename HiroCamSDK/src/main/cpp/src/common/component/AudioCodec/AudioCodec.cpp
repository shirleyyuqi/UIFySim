#ifndef DECODE_VIDEO

#include "AudioCodec.h"
#include <faac.h>

#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif

#define ADPCM_ENCODE_UINT	84*4

extern "C"
{
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include <stdint.h>
#endif

#ifdef HAVE_AV_CONFIG_H
#undef HAVE_AV_CONFIG_H
#endif
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
}

MUTEX AudioCodec::m_mutex;
HR_U32 AudioCodec::m_codecInstNum = 0;

//HR_S32 (AudioCodec::*pEncode)(HR_U8 *,HR_U32 ,HR_U8 *,HR_U32* );
//HR_S32 (AudioCodec::*pFreeEncoder)();
//
//HR_S32 (AudioCodec::*pDecode)(HR_U8 *,HR_U32 ,HR_U8 *,HR_U32* );  
//HR_S32 (AudioCodec::*pFreeDecoder)();

AudioCodec::AudioCodec(void)
{
	m_faacEncoder = NULL;
	m_nMaxOutputBytes = 0;
	m_faacBuffer = NULL;
	m_config = 0;
	m_avCodecId = AV_Decoder_ID_AAC;
	m_pDecoderInst=NULL;
	m_outBufSize = 0;
	memset(&m_state,0x00,sizeof(adpcm_state));
	if(m_codecInstNum==0)
	{
		INIT_MUTEX(&m_mutex);
	}
	m_codecInstNum++;
}

AudioCodec::~AudioCodec(void)
{
	try{
		FreeDecoder();
		FreeEncoder();
		if (m_codecInstNum==1)
		{
			RELEASE_MUTEX(&m_mutex);
		}
		m_codecInstNum--;
	}catch(...){}
}

HR_S32 AudioCodec::InitEncoder(ACodecID decoderId,HR_U32 outBufferSize,HR_S32 bit_rate,HR_S32 sample_rate,HR_S32 channels,HR_S32 bits_per_coded_sample,HR_S32 channel_layout)
{
	m_avCodecId=decoderId;
	this->m_outBufSize=outBufferSize;
	switch (m_avCodecId)
	{
	case AV_Decoder_ID_AAC:
		return CreateAudioEncoderInstance(AV_CODEC_ID_AAC,outBufferSize,bit_rate,sample_rate,channels,bits_per_coded_sample,channel_layout);
	case AV_Decoder_ID_FAAC:
		return CreateFaacEncoderInstance(AV_CODEC_ID_AAC,outBufferSize,bit_rate,sample_rate,channels,bits_per_coded_sample,channel_layout);
	case AV_Decoder_ID_ADPCM_G726:
		return CreateAudioEncoderInstance(AV_CODEC_ID_ADPCM_G726,outBufferSize,bit_rate,sample_rate,channels,bits_per_coded_sample,channel_layout);
	case AV_Decoder_ID_PCM_ALAW:
		return CreateAudioEncoderInstance(AV_CODEC_ID_PCM_ALAW,outBufferSize,bit_rate,sample_rate,channels,bits_per_coded_sample,channel_layout);
	case AV_Decoder_ID_PCM_MULAW:
		return CreateAudioEncoderInstance(AV_CODEC_ID_PCM_MULAW,outBufferSize,bit_rate,sample_rate,channels,bits_per_coded_sample,channel_layout);
	case AV_Decoder_ID_ADPCM:
		return CreateAdpcmEncoderInstance();
	default:
		HRLOG_E("don't support this codec_id\n");
		return -1;
	}
}

HR_S32 AudioCodec::encode(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize)
{
	switch (m_avCodecId)
	{
	case AV_Decoder_ID_AAC:
		return EncodeAudio(inBuffer,bufsize,outBuffer,outBufferSize);
	case AV_Decoder_ID_FAAC:
		return EncodeFaac(inBuffer,bufsize,outBuffer,outBufferSize);
	case AV_Decoder_ID_ADPCM:
		return EncodeAdpcm(inBuffer,bufsize,outBuffer,outBufferSize);
	default:
		return EncodeAudio(inBuffer,bufsize,outBuffer,outBufferSize);
	}
}

void AudioCodec::FreeEncoder()
{
	switch (m_avCodecId)
	{
	case AV_Decoder_ID_AAC:
		DestroyAudioEncInstance();
		break;
	case AV_Decoder_ID_FAAC:
		DestroyFaacEncInstance();
		break;
	case AV_Decoder_ID_ADPCM:
		DestroyAdpcmEncInstance();
		break;
	default:
		DestroyAudioEncInstance();
		break;
	}
}

HR_S32 AudioCodec::InitDeocder(ACodecID decoderId,HR_U32 outBufferSize,HR_S32 channels,HR_S32 bits_per_coded_sample,HR_S32 channel_layout)
{
	m_avCodecId=decoderId;
	this->m_outBufSize=outBufferSize;
	switch (m_avCodecId)
	{
	case AV_Decoder_ID_AAC:
		return CreateAudioDecoderInstance(AV_CODEC_ID_AAC,outBufferSize,channels,bits_per_coded_sample,channel_layout);
	case AV_Decoder_ID_PCM_ALAW:
		return CreateAudioDecoderInstance(AV_CODEC_ID_PCM_ALAW,outBufferSize,channels,bits_per_coded_sample,channel_layout);
	case AV_Decoder_ID_PCM_MULAW:
		return CreateAudioDecoderInstance(AV_CODEC_ID_PCM_MULAW,outBufferSize,channels,bits_per_coded_sample,channel_layout);
	case AV_Decoder_ID_ADPCM_G726:
		return CreateAudioDecoderInstance(AV_CODEC_ID_ADPCM_G726,outBufferSize,channels,bits_per_coded_sample,channel_layout);
	case AV_Decoder_ID_ADPCM:
		return CreateAdpcmDecoderInstance();
	default:
		HRLOG_E("don't support this codec_id\n");
		return 0;
	}
}

HR_S32 AudioCodec::decode(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize)
{
	switch (m_avCodecId)
	{
	case AV_Decoder_ID_AAC:
		return DecodeAudio(inBuffer,bufsize,outBuffer,outBufferSize);
	case AV_Decoder_ID_ADPCM:
		return DecodeAdpcm(inBuffer,bufsize,outBuffer,outBufferSize);
	default:
		return DecodeAudio(inBuffer,bufsize,outBuffer,outBufferSize);
	}
}
void AudioCodec::FreeDecoder()
{
	switch (m_avCodecId)
	{
	case AV_Decoder_ID_AAC:
		DestroyAudioDecInstance();
		break;
	case AV_Decoder_ID_ADPCM:
		DestroyAdpcmDecInstance();
		break;
	default:
		DestroyAudioDecInstance();
		break;
	}
}

HR_S32 AudioCodec::CreateAudioDecoderInstance(HR_S32 decoderId,HR_U32 outBufferSize,HR_S32 channels,HR_S32 bits_per_coded_sample,HR_S32 channel_layout)
{
	AVCodecContext *codecCtx; 
	AVCodec *codec; 
	if (m_pDecoderInst!=NULL)
	{
		return 0;
	}
	m_pDecoderInst =new AudioCodecInstance;
	LOCK_MUTEX(&m_mutex);
#ifdef _MAC
	m_pDecoderInst->avframe=avcodec_alloc_frame();
#else
	m_pDecoderInst->avframe=av_frame_alloc();
#endif
	avcodec_register_all();
	codec = avcodec_find_decoder((AVCodecID)decoderId);
	codecCtx = avcodec_alloc_context3(codec);
	m_pDecoderInst->avcodeContext=codecCtx;
	codecCtx->bits_per_coded_sample = bits_per_coded_sample; //g726压缩比为 8:1 编码前采样用bit数为16 那么编码后应该占16/8 = 2 这是我的理解
	codecCtx->channels = channels;
	codecCtx->channel_layout=channel_layout;
	/*codecCtx->sample_rate=48000;
	codecCtx->bit_rate=63000;
	codecCtx->channel_layout=AV_CH_LAYOUT_STEREO;
	codecCtx->channels = av_get_channel_layout_nb_channels(codecCtx->channel_layout);*/
	if(!codec || (avcodec_open2(codecCtx, codec,NULL)< 0))  
	{  
#ifdef _WIN32
		avcodec_free_context(&codecCtx);
#else
		avcodec_close(codecCtx);
		av_free(codecCtx);
#endif
		UNLOCK_MUTEX(&m_mutex);
		delete m_pDecoderInst;
		m_pDecoderInst=NULL;
		HRLOG_E("avcodec_open2 failed,Unsupported codec!\n");
		return 0;  
	}
	UNLOCK_MUTEX(&m_mutex);
	return 1;
}

HR_S32 AudioCodec::DecodeAudio(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize)
{
	HR_S32 data_size, frameFinished=0;
	AVPacket pkt1,*pkt=&pkt1;
	if(m_pDecoderInst == NULL)
	{
		return  -1;
	}
	AVFrame *pFrame=(AVFrame *)m_pDecoderInst->avframe;
	AVCodecContext *avctx=(AVCodecContext *)m_pDecoderInst->avcodeContext;
	if (this->m_outBufSize<bufsize*20)
	{
		HRLOG_E("the out buffer of decoder is not enough!\n");
		return -1;
	}
	av_init_packet(&pkt1); 
	pkt->data=inBuffer;
	pkt->size=bufsize;
	/*if (!pFrame) 
	{ 
	if (!(pFrame = av_frame_alloc())) 
	{ 
	return 0; 
	} 
	else
	{
	m_pDecoderInst->avframe=pFrame;
	}
	} 
	else 
	{ 
	avcodec_get_frame_defaults(pFrame); 
	} */
	avcodec_decode_audio4(avctx, pFrame, &frameFinished,pkt);  // Decode video frame 
	if (frameFinished==1&&outBuffer!=NULL)//解码出yuv帧数据
	{
		if (avctx->sample_fmt==AV_SAMPLE_FMT_S16)
		{
			data_size = av_samples_get_buffer_size(NULL, avctx->channels, pFrame->nb_samples, avctx->sample_fmt, 1);
			memcpy(outBuffer,pFrame->extended_data[0],data_size);
			*outBufferSize=data_size;
		}
		else
		{
			HR_S32 in_samples = pFrame->nb_samples;
			HR_S16 *sample_buffer = (HR_S16 *)outBuffer;/*(short*)malloc(pFrame->nb_samples * 2 * 2)*/;
			memset(sample_buffer, 0, pFrame->nb_samples * 4);
			float *inputChannel0 = (float*)(pFrame->extended_data[0]);
			float* inputChannel1 = (float*)(pFrame->extended_data[1]);
			if( inputChannel1 == NULL ) {		// Mono
				for(HR_S32 i=0; i<in_samples; i++ ) {
					float sample = *inputChannel0++;
					if( sample < -1.0f ) {
						sample = -1.0f;
					} else if( sample > 1.0f ) {
						sample = 1.0f;
					}
					sample_buffer[i] = (HR_S16)(sample * 32767.0f);
				}
				*outBufferSize=in_samples*2;
			} else {					// Stereo
				for(HR_S32 i=0; i<in_samples; i++) {
					sample_buffer[i*2] = (HR_S16)((*inputChannel0++) * 32767.0f);
					sample_buffer[i*2+1] = (HR_S16)((*inputChannel1++) * 32767.0f);
				}
				*outBufferSize=in_samples*4;
			}
			/**outBufferSize=in_samples*2;*/
			/*memcpy(outBuffer,sample_buffer,in_samples*2);
			free(sample_buffer);*/
		}
		return 1;
	}
	return 0;
}
HR_S32 AudioCodec::DestroyAudioDecInstance()
{
	return DestroyAudioEncInstance();
}

HR_S32 AudioCodec::DestroyAudioEncInstance()
{
	AVCodecContext *avctx;
	AVFrame *pFrame;
	if (m_pDecoderInst==NULL)
	{
		HRLOG_E("destory audio encoder,Free NULL pointer!\n");
		return -1;
	}
	avctx=(AVCodecContext *)m_pDecoderInst->avcodeContext;
	LOCK_MUTEX(&m_mutex);
	pFrame=(AVFrame *)m_pDecoderInst->avframe;
	if (pFrame!=NULL)
	{
#ifdef _MAC
		avcodec_free_frame(&pFrame);
#else
		av_frame_free(&pFrame);
#endif
	}
	avcodec_close(avctx);
#ifdef _WIN32
	avcodec_free_context(&avctx);
#else
	av_free(avctx);
#endif
	UNLOCK_MUTEX(&m_mutex);
	delete m_pDecoderInst;
	m_pDecoderInst=NULL;
	return 0;
}

HR_S32 AudioCodec::CreateAudioEncoderInstance(HR_S32 decoderId,HR_U32 outBufferSize,HR_S32 bit_rate,HR_S32 sample_rate,HR_S32 channels,HR_S32 bits_per_coded_sample,HR_S32 channel_layout)//创建编码器实例
{
	AVCodecContext *codecCtx; 
	AVCodec *codec; 
	if (m_pDecoderInst!=NULL)
	{
		return 0;
	}
	m_pDecoderInst =new AudioCodecInstance;
	LOCK_MUTEX(&m_mutex);
#ifdef _MAC
	m_pDecoderInst->avframe= avcodec_alloc_frame();
#else
	m_pDecoderInst->avframe= av_frame_alloc();
#endif
	avcodec_register_all();
	m_outBufSize=outBufferSize;
	codec = avcodec_find_encoder((AVCodecID)decoderId); 
	codecCtx = avcodec_alloc_context3(codec);
	m_pDecoderInst->avcodeContext=codecCtx;
	codecCtx->bits_per_coded_sample = bits_per_coded_sample; //g726压缩比为 8:1 编码前采样用bit数为16 那么编码后应该占16/8 = 2 这是我的理解
	codecCtx->codec_type=AVMEDIA_TYPE_AUDIO;
	codecCtx->bit_rate = bit_rate; // 
	codecCtx->sample_rate = sample_rate; //采样率 
	codecCtx->channels = channels; //通道数 
	codecCtx->sample_fmt = AV_SAMPLE_FMT_S16; //应该是采样位数
	codecCtx->channel_layout=channel_layout;
	if(!codec || (avcodec_open2(codecCtx, codec,NULL)< 0))  
	{  
#ifdef _WIN32
		avcodec_free_context(&codecCtx);
#else
		avcodec_close(codecCtx);
		av_free(codecCtx);
#endif
		UNLOCK_MUTEX(&m_mutex);
		delete m_pDecoderInst;
		m_pDecoderInst=NULL;
		HRLOG_E("avcodec_open2 failed,Unsupported codec!\n");
		return 0;  
	}
	HR_S32 size=av_samples_get_buffer_size(NULL, codecCtx->channels,codecCtx->frame_size,codecCtx->sample_fmt, 1);
	UNLOCK_MUTEX(&m_mutex);
	return size>0?size:8192;
}
HR_S32 AudioCodec::EncodeAudio(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize)
{
	HR_S32 len1, frameFinished;
	AVPacket avpkt;
	if(m_pDecoderInst == NULL)
	{
		return  -1;
	}
	AVFrame *pFrame=(AVFrame *)m_pDecoderInst->avframe;
	AVCodecContext *avctx=(AVCodecContext *)m_pDecoderInst->avcodeContext;
	//av_init_packet(&avpkt);//如下两句在编码中是必须的，要不avcodec_encode_audio2会失败
	//avpkt.data = NULL;
	//avpkt.size = 0;
	//avcodec_get_frame_defaults(pFrame);
	av_new_packet(&avpkt,bufsize);
	pFrame->nb_samples = (HR_S32)bufsize/(avctx->channels*av_get_bytes_per_sample(avctx->sample_fmt));
	len1 = avcodec_fill_audio_frame(pFrame, avctx->channels, avctx->sample_fmt, (HR_U8 *)inBuffer,bufsize, 1); 
	if (len1 < 0) 
	{
		av_free_packet(&avpkt);
		return 0; 
	}
	if (avcodec_encode_audio2(avctx, &avpkt, pFrame, &frameFinished) < 0) 
	{
		av_free_packet(&avpkt);
		return 0; 
	} 
	if (frameFinished) 
	{ 
		memcpy(outBuffer,avpkt.data,avpkt.size);
		*outBufferSize=avpkt.size;
		av_free_packet(&avpkt);
		return 1;
	} 
	av_free_packet(&avpkt);
	return 0;
}

HR_S32 AudioCodec::CreateAdpcmDecoderInstance()//创建解码器实例
{
	memset(&m_state,0,sizeof(adpcm_state));
	return ADPCM_ENCODE_UINT;
}

HR_S32 AudioCodec::DecodeAdpcm(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize)
{
	if (this->m_outBufSize<bufsize*4)
	{
		HRLOG_E("the out buffer of adpcm-decoder is not enough!\n");
		return -1;
	}
	*outBufferSize=bufsize*4;
	adpcm_decoder((HR_CHAR*)inBuffer,(HR_S16 *)outBuffer,bufsize*2,&m_state);
	return 1;
}

HR_S32 AudioCodec::DestroyAdpcmDecInstance()
{
	return 0;
}

HR_S32 AudioCodec::CreateFaacEncoderInstance(HR_S32 decoderId,HR_U32 outBufferSize,HR_S32 bit_rate,HR_S32 sample_rate,HR_S32 channels,HR_S32 bits_per_coded_sample,HR_S32 channel_layout)//创建解码器实例
{
	HR_U32 nInputSamples = 0;
	HR_S32 nPCMBufferSize = 0;
	HR_S32 nPCMBitSize = 16;
    HR_U8* config = NULL;
    //HR_U8 config[20] = {0};
    HR_U32 configsize=0;//sizeof(config);
	faacEncConfigurationPtr pConfiguration; 
	m_outBufSize=outBufferSize;
	m_faacEncoder = faacEncOpen((unsigned long)sample_rate, channels, (unsigned long*)&nInputSamples, (unsigned long*)&m_nMaxOutputBytes);
	if(m_faacEncoder == NULL)
	{
		HRLOG_E("Failed to call faacEncOpen()\n");
		return -1;
	}
	nPCMBufferSize = nInputSamples * nPCMBitSize / 8;
	m_faacBuffer = new HR_U8[m_nMaxOutputBytes];
	pConfiguration = faacEncGetCurrentConfiguration(m_faacEncoder);
	pConfiguration->inputFormat = FAAC_INPUT_16BIT;
	pConfiguration->outputFormat = 1;
	pConfiguration->useTns= true;
	pConfiguration->useLfe= false;
	pConfiguration->aacObjectType= LOW;
	pConfiguration->shortctl= SHORTCTL_NORMAL;
	pConfiguration->quantqual= 100;
	pConfiguration->bandWidth= 0;
	pConfiguration->bitRate= 0;
	faacEncSetConfiguration(m_faacEncoder, pConfiguration);
	faacEncGetDecoderSpecificInfo(m_faacEncoder, (HR_U8**)&config, (unsigned long*)&configsize);
	m_config = *(short*)config;
	return nPCMBufferSize;
}

HR_S16 AudioCodec::GetConfig()
{
	return m_config;
}

HR_S32 AudioCodec::EncodeFaac(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize)
{
	HR_S32 iRet,nPCMBitSize = 16;
	int nInputSamples = (HR_S32)bufsize / (nPCMBitSize / 8);
//    HRLOG_I(" buffsize=%d, nInputSamples= %d", bufsize, nInputSamples);
	iRet=faacEncEncode(
		m_faacEncoder, (HR_S32*) inBuffer, nInputSamples, m_faacBuffer, m_nMaxOutputBytes);
//     HRLOG_I(" out??????  = %d", m_nMaxOutputBytes);
	if (iRet>0)
	{
		memcpy(outBuffer,m_faacBuffer,iRet);
		*outBufferSize=iRet;
	}
	return iRet;
}

HR_S32 AudioCodec::DestroyFaacEncInstance()
{
	if (m_faacEncoder!=NULL)
	{
		faacEncClose(m_faacEncoder);
		m_faacEncoder=NULL;
	}
	if (NULL!=m_faacBuffer)
	{
		delete []m_faacBuffer;
		m_faacBuffer=NULL;
	}
	return 0;
}

HR_S32 AudioCodec::CreateAdpcmEncoderInstance()//创建编码器实例
{
	memset(&m_state,0,sizeof(adpcm_state));
	return ADPCM_ENCODE_UINT;
}

HR_S32 AudioCodec::EncodeAdpcm(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize)
{
	*outBufferSize=bufsize/4;
	adpcm_coder((HR_S16*)inBuffer,(HR_CHAR *)outBuffer,bufsize/2,&m_state);
	return 1;
}

HR_S32 AudioCodec::DestroyAdpcmEncInstance()//释放解码器实例
{
	return 0;
}


#endif
