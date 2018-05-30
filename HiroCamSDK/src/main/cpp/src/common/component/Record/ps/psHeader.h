/* *************************************************************************************************
 *
 *	Filename:  ps header.h
 *
 *	Description:  ps header file
 *
 *	Version:  1.0
 *	Created:  8/17/2013
 *
 *	Author:  cyb
 *	Organization:  FOSCAM TECH.CO.,LTD.
 *
 * *************************************************************************************************/
#ifndef __HEADER_PACK_HEADER_DEF_H__
#define __HEADER_PACK_HEADER_DEF_H__

//包含其他头文件
#include "BaseDataType.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
using namespace std;
using namespace HiroCamSDK;


#define PES_PAYLOAD_SIZE_MAX 	65500

#define PES_MAX_HEADER_SIZE		50
#define PES_MAX_SYSTEM_SIZE		50
#define PES_MAX_MAP_SIZE		50


#define AV_TYPE_VIDEO	0
#define AV_TYPE_AUDIO	1

#define AV_TYPE_VIDEO_STREAM_ID		0xE0 // h264

#define AV_TYPE_AUDIO_STREAM_ID		0xC0 // g711
//#define AV_TYPE_AUDIO_STREAM_ID		0xA0 // pcm

//#define AV_TYPE_VIDEO_STREAM_TYPE	0x1B // h264
#define AV_TYPE_VIDEO_STREAM_TYPE	0x10 // mpeg4

//#define AV_TYPE_AUDIO_STREAM_TYPE	0x90 // g711
#define AV_TYPE_AUDIO_STREAM_TYPE	0x0F	//aac

//typedef struct PS_HEADER_tag
typedef struct PS_HEADER{ // 14 Byte
	unsigned char pack_start_code[4];		//'0x000001BA'

	unsigned char system_clock_reference_base21:2;
	unsigned char marker_bit:1;
	unsigned char system_clock_reference_base1:3;
	unsigned char fix_bit:2;				//'01'
	
	unsigned char system_clock_reference_base22;

	unsigned char system_clock_reference_base31:2;
	unsigned char marker_bit1:1;
	unsigned char system_clock_reference_base23:5;

	unsigned char system_clock_reference_base32;
	

	unsigned char system_clock_reference_extension1:2;
	unsigned char marker_bit2:1;
	unsigned char system_clock_reference_base33:5;	//system_clock_reference_base 33bit

	unsigned char marker_bit3:1;
	unsigned char system_clock_reference_extension2:7; //system_clock_reference_extension 9bit
	
	unsigned char program_mux_rate1;

	unsigned char program_mux_rate2;
	

	unsigned char marker_bit4:2;
	unsigned char program_mux_rate3:6;

	unsigned char pack_stuffing_length:3;
	unsigned char reserved:5;
}*pPS_HEADER_tag , PS_HEADER_tag;


//typedef struct PS_SYSTEM_HEADER_tag{
typedef struct PS_SYSTEM_HEADER{ // 15 Byte
	unsigned char system_header_start_code[4];
	unsigned char header_length[2];
	
	unsigned char rate_bound16:7;
	unsigned char marker_bits1:1;
	
	unsigned char rate_bound8;
	
	unsigned char marker_bits2:1;
	unsigned char rate_bound1:7;
	
	unsigned char CSPS_flag:1;
	unsigned char fixed_flag:1;
	unsigned char audio_bound:6;
	
	unsigned char video_bound:5;
	unsigned char marker_bit3:1;
	unsigned char system_video_lock_flag:1;
	unsigned char system_audio_lock_flag:1;
	
	unsigned char reserved_bits:7;
	unsigned char packet_rate_restriction_flag:1;
	
	unsigned char stream_id ;
	
	unsigned char P_STD_buffer_size_bound5:5 ;
	unsigned char P_STD_buffer_bound_scale:1 ;
	unsigned char marker_bit4:2 ;
	
	unsigned char P_STD_buffer_size_bound8 ;
}PS_SYSTEM_HEADER_tag , *pPS_SYSTEM_HEADER_tag ;

//typedef struct PES_HEADER_tag
typedef struct PES_HEADER{ // 9 Byte
	unsigned char	packet_start_code_prefix[3];
	unsigned char	stream_id;
	
	unsigned char	PES_packet_length[2];
	
	unsigned char	original_or_copy:1;
	unsigned char	copyright:1;
	unsigned char	data_alignment_indicator:1;
	unsigned char	PES_priority:1;
	unsigned char	PES_scrambling_control:2;
	unsigned char	fix_bit:2;

	unsigned char	PES_extension_flag:1;
	unsigned char	PES_CRC_flag:1;
	unsigned char	additional_copy_info_flag:1;
	unsigned char	DSM_trick_mode_flag:1;
	unsigned char	ES_rate_flag:1;
	unsigned char	ESCR_flag:1;
	unsigned char	PTS_DTS_flags:2;

	unsigned char	PES_header_data_length;
}*pPES_HEADER_tag , PES_HEADER_tag;

//typedef struct PTS_tag{
typedef struct PTS{ // 5 byte
	unsigned char marker_bit:1;
	unsigned char PTS1:3;
	unsigned char fix_bit:4;

	unsigned char PTS21;

	unsigned char marker_bit1:1;
	unsigned char PTS22:7;

	unsigned char PTS31; 

	unsigned char marker_bit2:1;
	unsigned char PTS32:7;


}*pPTS_tag , PTS_tag;


//typedef struct PSM_tag
typedef struct PSM{ // 20 Byte
	unsigned char packet_start_code_prefix[3];
	unsigned char map_stream_id;
	unsigned char program_stream_map_length[2];
	
	unsigned char program_stream_map_version:5;
	unsigned char reserved1:2;
	unsigned char current_next_indicator:1;

	unsigned char marker_bit:1;
	unsigned char reserved2:7;

	unsigned char program_stream_info_length[2];
	unsigned char elementary_stream_map_length[2];
	unsigned char stream_type;
	unsigned char elementary_stream_id;
	unsigned char elementary_stream_info_length[2];
	unsigned char CRC_32[4];
}*pPSM_tag , PSM_tag;

int makeAudioPesPakectHeader(char *pesHeader, unsigned int es_data_size, HR_U64 pts, HR_U64 dts);

int makeVideoPesPakectHeader(char *pesHeader, unsigned int es_data_size, HR_U64 pts, HR_U64 dts);

int makePsPacketHeader(char *psPacketHeader, HR_U64 scrBase, HR_U64 scExt, unsigned int muxRate);

int makePsSystemHeader(char *psSystemHeader, long muxRate);

int makePsSystemMapHeader(char *psSystemMapHeader);


#endif
