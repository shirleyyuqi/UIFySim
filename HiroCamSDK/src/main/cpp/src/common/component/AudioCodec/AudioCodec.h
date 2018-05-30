#ifndef PATH_AUDIOCODEC_H
#define PATH_AUDIOCODEC_H
#pragma once
#include "adpcm.h"
#include "BaseDataType.h"

using namespace HiroCamSDK;

#ifndef DECODE_VIDEO

enum ACodecID{
	AV_Decoder_ID_PCM_ALAW,
	AV_Decoder_ID_PCM_MULAW,
	AV_Decoder_ID_ADPCM_G726,
	AV_Decoder_ID_AAC,//��ffmpeg�����aac������붼֧�֣�
	AV_Decoder_ID_FAAC,//��faac����aac ��ֻ֧�ֱ��룩
	AV_Decoder_ID_ADPCM
};

typedef struct AudioCodecInstance
{
	void* avcodeContext;
	void* avframe;
	AudioCodecInstance ()
	{
		avcodeContext=NULL;
		avframe=NULL;
	}
}AudioCodecInstance;


class AudioCodec
{
public:
	AudioCodec(void);
	~AudioCodec(void);
	HR_S32 InitEncoder(ACodecID decoderId,HR_U32 outBufferSize,HR_S32 bit_rate,HR_S32 sample_rate,HR_S32 channels=1,HR_S32 bits_per_coded_sample=2,HR_S32 channel_layout=4);
	HR_S32 encode(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize);
	void FreeEncoder();
	
	HR_S32 InitDeocder(ACodecID decoderId,HR_U32 outBufferSize,HR_S32 channels=1,HR_S32 bits_per_coded_sample=2,HR_S32 channel_layout=4);
	HR_S32 decode(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize);
	void FreeDecoder();

	HR_S16 GetConfig();
private:
	HR_S32 CreateAudioDecoderInstance(HR_S32 decoderId,HR_U32 outBufferSize,HR_S32 channels=1,HR_S32 bits_per_coded_sample=2,HR_S32 channel_layout=4);//����������ʵ��
	HR_S32 DecodeAudio(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize);//����g711��g726ԭʼ���ݳ�pcm
	HR_S32 DestroyAudioDecInstance();

	HR_S32 CreateAudioEncoderInstance(HR_S32 decoderId,HR_U32 outBufferSize,HR_S32 bit_rate,HR_S32 sample_rate,HR_S32 channels=1,HR_S32 bits_per_coded_sample=2,HR_S32 channel_layout=4);//����������ʵ��
	HR_S32 EncodeAudio(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize);//����g711��g726
	HR_S32 DestroyAudioEncInstance();//�ͷŽ�����ʵ��


	HR_S32 CreateAdpcmDecoderInstance();//����������ʵ��
	HR_S32 DecodeAdpcm(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize);//����adpcmԭʼ���ݳ�pcm
	HR_S32 DestroyAdpcmDecInstance();

	HR_S32 CreateAdpcmEncoderInstance();//����������ʵ��
	HR_S32 EncodeAdpcm(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize);//����adpcm
	HR_S32 DestroyAdpcmEncInstance();//�ͷŽ�����ʵ��

	HR_S32 CreateFaacEncoderInstance(HR_S32 decoderId,HR_U32 outBufferSize,HR_S32 bit_rate,HR_S32 sample_rate,HR_S32 channels=1,HR_S32 bits_per_coded_sample=2,HR_S32 channel_layout=4);//����������ʵ��
	HR_S32 EncodeFaac(HR_U8 *inBuffer,HR_U32 bufsize,HR_U8 *outBuffer,HR_U32* outBufferSize);//ͨ��faac�����aac
	HR_S32 DestroyFaacEncInstance();//�ͷŽ�����ʵ��
	
	AudioCodecInstance *m_pDecoderInst;
	ACodecID m_avCodecId;
	HR_U32 m_outBufSize;
	adpcm_state m_state;

	HR_U8 *m_faacBuffer;
	void* m_faacEncoder;
	HR_U32 m_nMaxOutputBytes;
	HR_S16 m_config;
public:
	static MUTEX m_mutex;
	static HR_U32 m_codecInstNum;
};

#endif //DECODE_VIDEO

#endif

