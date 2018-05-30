#include "psHeader.h"

char pes_header[30];
char ps_packet_header[50];
char ps_system_header[50];
char ps_system_map_header[50];

static unsigned char outbfr;
static int outcnt;
static int bytecnt;


/* initialize buffer, call once before first putbits or alignbits */
void initBits()
{
  outcnt = 8;
  bytecnt = 0;
}

/* write rightmost n (0<=n<=32) bits of val to outfile */
void putBits(char *bits,int val,int n)
{
  int i;
  unsigned int mask;

  mask = 1 << (n-1); /* selects first (leftmost) bit */

  for (i=0; i<n; i++)
  {
    outbfr <<= 1;

    if (val & mask)
      outbfr|= 1;

    mask >>= 1; /* select next bit */
    outcnt--;

    if (outcnt==0) /* 8 bit buffer full */
    {
		bits[bytecnt] = outbfr;
		//printf("putbits bytecnt=%d,valu=%d\n",bytecnt,bits[bytecnt]);
    	outcnt = 8;
      	bytecnt++;
	  
    }
  }
}

/* zero bit stuffing to next byte boundary (5.2.3, 6.2.1) */
void alignBits()
{
 // if (outcnt!=8)
 //   putbits(char * bits, int val, int n)(0,outcnt);
}

/* return total number of generated bits */
int bitCount()
{
  return 8*bytecnt + (8-outcnt);
}

int makePsPacketHeader(char *psPacketHeader, HR_U64 scrBase, HR_U64 scrExt, unsigned int muxRate)
{
	initBits();

	putBits(psPacketHeader, 0x000001BA, 32);			//sync bytes
	// 4
	putBits(psPacketHeader, 0x01, 2);					//marker bits '01b'
	putBits(psPacketHeader,(int)((scrBase&0x00000001C0000000ll)>>30),3);		//System clock [32..30]
	putBits(psPacketHeader, 0x01, 1);					//marker bit
	putBits(psPacketHeader,(int)((scrBase&0x000000003fff8000ll)>>15), 15); 	//System clock [29..15]
	putBits(psPacketHeader, 0x01, 1);					//marker bit
	putBits(psPacketHeader, (int)(scrBase&0x0000000000007fffll), 15); 		//System clock [14..0]
	putBits(psPacketHeader, 0x01, 1);					//marker bit
	putBits(psPacketHeader,scrExt, 9);					//SCR extension
	putBits(psPacketHeader, 0x01, 1);					//marker bit

	putBits(psPacketHeader, muxRate, 22);				//bit rate(n units of 50 bytes per second.)
	putBits(psPacketHeader, 0x03, 2);					//marker bits '11'

	putBits(psPacketHeader, 0x1f, 5);					//reserved(reserved for future use)
	putBits(psPacketHeader, 0x00, 3);					//stuffing length
	//14
	return 14;
}

int makeVideoPesPakectHeader(char *pesHeader, unsigned int pesDataSize, HR_U64 pts, HR_U64 dts)
{
	initBits();
	
	putBits(pesHeader, 0x000001, 24);				//startcode
	putBits(pesHeader, AV_TYPE_VIDEO_STREAM_ID, 8);	//streamID
	putBits(pesHeader, pesDataSize+0x0D, 16);
	//putBits(pesHeader, 0x00, 16);
	//6

	//putBits(pesHeader, 0x84, 8);		//flags
	putBits(pesHeader, 0x02, 2); 		// fixed 
	putBits(pesHeader, 0x00, 2); 		// PES_scrambling_control, 00: Not scrambled  
	putBits(pesHeader, 0x00, 1); 		// PES_priority,  '1' indicates a higher priority
	putBits(pesHeader, 0x01, 1); 		// data_alignment_indicator,
	putBits(pesHeader, 0x00, 1); 		// copyright,
	putBits(pesHeader, 0x00, 1); 		// original_or_copy,

	putBits(pesHeader, 0x03, 2);		//PTS_DTS_flags, 0x03:PTS+DTS
	//putBits(pesHeader, 0x00, 6);		//flags
	putBits(pesHeader, 0x00, 1);	 	// ESCR_flag
	putBits(pesHeader, 0x00, 1);	 	// ES_rate_flag
	putBits(pesHeader, 0x00, 1);	 	// DSM_trick_mode_flag
	putBits(pesHeader, 0x00, 1);	 	// additional_copy_info_flag
	putBits(pesHeader, 0x00, 1);	 	// PES_CRC_flag
	putBits(pesHeader, 0x00, 1);	 	// PES_extension_flag

	putBits(pesHeader, 0x0A, 8); 		//pes_header_data_length, PTS len + DTS len
	//9
	putBits(pesHeader, 0x03, 4); 		// fixed
	putBits(pesHeader, (int)(pts>>30),3);
	putBits(pesHeader, 0x01, 1); 		// marker_bit
	putBits(pesHeader, (int)((pts&0x000000003fff8000ll)>>15), 15);
	putBits(pesHeader, 0x01, 1); 		// marker_bit
	putBits(pesHeader, (int)(pts&0x0000000000007fffll), 15);
	putBits(pesHeader, 0x01, 1); 		// marker_bit
	
	//dts
	putBits(pesHeader, 0x01, 4); 		// fixed
	putBits(pesHeader, (int)(dts>>30),3);
	putBits(pesHeader, 0x01, 1); 		// marker_bit
	putBits(pesHeader, (int)((dts&0x000000003fff8000ll)>>15), 15);
	putBits(pesHeader, 0x01, 1); 		// marker_bit
	putBits(pesHeader, (int)(dts&0x0000000000007fffll), 15);
	putBits(pesHeader, 0x01, 1); 		// marker_bit
	//19
	
	return 19;
}

int makeAudioPesPakectHeader(char *pesHeader, unsigned int esDataSize, HR_U64 pts, HR_U64 dts)
{
	initBits();
	
	putBits(pesHeader, 0x000001, 24);				//startcode
	putBits(pesHeader, AV_TYPE_AUDIO_STREAM_ID, 8);	//streamID
	putBits(pesHeader, esDataSize+0x0D, 16);
	//6

	//putBits(pes_header, 0x84, 8);						//flags
	putBits(pesHeader, 0x02, 2); 					// fixed 
	putBits(pesHeader, 0x00, 2); 					// PES_scrambling_control, 00: Not scrambled  
	putBits(pesHeader, 0x00, 1); 					// PES_priority,  '1' indicates a higher priority
	putBits(pesHeader, 0x01, 1); 					// data_alignment_indicator,
	putBits(pesHeader, 0x00, 1); 					// copyright,
	putBits(pesHeader, 0x00, 1); 					// original_or_copy,

	putBits(pesHeader, 0x03, 2);					//PTS_DTS_flags, 0x03:PTS+DTS
	//putBits(pesHeader, 0x00, 6);						//flags
	putBits(pesHeader, 0x00, 1);	 				// ESCR_flag
	putBits(pesHeader, 0x00, 1);	 				// ES_rate_flag
	putBits(pesHeader, 0x00, 1);	 				// DSM_trick_mode_flag
	putBits(pesHeader, 0x00, 1);	 				// additional_copy_info_flag
	putBits(pesHeader, 0x00, 1);	 				// PES_CRC_flag
	putBits(pesHeader, 0x00, 1);	 				// PES_extension_flag

	putBits(pesHeader, 0x0A, 8); 					//pes_header_data_length, PTS len + DTS len
	//9
	putBits(pesHeader, 0x03, 4); 					// fixed
	putBits(pesHeader, (int)(pts>>30),3);
	putBits(pesHeader, 0x01, 1);					// marker_bit
	putBits(pesHeader, (int)((pts&0x000000003fff8000ll)>>15), 15);
	putBits(pesHeader, 0x01, 1); // marker_bit
	putBits(pesHeader, (int)(pts&0x0000000000007fffll), 15);
	putBits(pesHeader, 0x01, 1); // marker_bit
	
	//dts
	putBits(pesHeader, 0x01, 4); // fixed
	putBits(pesHeader, (int)(dts>>30),3);
	putBits(pesHeader, 0x01, 1); // marker_bit
	putBits(pesHeader, (int)((dts&0x000000003fff8000ll)>>15), 15);
	putBits(pesHeader, 0x01, 1); // marker_bit
	putBits(pesHeader, (int)(dts&0x0000000000007fffll), 15);
	putBits(pesHeader, 0x01, 1); // marker_bit
	//19
	
	return 19;
}

int makePsSystemHeader(char *psSystemHeader, long muxRate )
{
	initBits();
	
	//system header
	putBits(psSystemHeader, 0x000001BB, 32);	//start code
	putBits(psSystemHeader, 0x0C, 16);			//header_length
	putBits(psSystemHeader, 0x01, 1);			//marker_bit
	putBits(psSystemHeader, muxRate, 22);		//rate_bound
	putBits(psSystemHeader, 0x01, 1);         	//marker_bit 
	//9
	putBits(psSystemHeader, 0x01, 6);			//audio_bound,  range from 0 to 32????
	putBits(psSystemHeader, 0x00, 1);         	//fixed_flag, 1:CBR, 0:VBR
	putBits(psSystemHeader, 0x00, 1);         	//CSPS_flag 
	
	putBits(psSystemHeader, 0x00, 0);			//system_audio_lock_flag
	putBits(psSystemHeader, 0x00, 0);			//system_video_lock_flag
	putBits(psSystemHeader, 0x01, 1);			//marker_bit 
	putBits(psSystemHeader, 0x01, 5);			//video_bound, range from 0 to 16
	
	//11
	putBits(psSystemHeader, 0x00, 1);			//dif from mpeg1
	putBits(psSystemHeader, 0x7F, 7);			//reserver, shall be 0x7F

	//stream bound
	// 0xE0 0xE0 0xE8
	putBits(psSystemHeader, 0xE0, 8);		//stream_id
	putBits(psSystemHeader, 0x03, 2);		//marker_bit 
	putBits(psSystemHeader, 0x01, 1);		//PSTD_buffer_bound_scale,  0x01 indicates video
	putBits(psSystemHeader, 0xE8, 13);		//PSTD_buffer_size_bound
	// 0xC0 0xC0 0x20
	putBits(psSystemHeader, AV_TYPE_AUDIO_STREAM_ID, 8);	
	//putBits(psSystemHeader, 0xA0, 8);		//stream_id
	putBits(psSystemHeader, 0x03, 2);
	putBits(psSystemHeader, 0x00, 1);		//PSTD_buffer_bound_scale,  0x00 indicates audio
	putBits(psSystemHeader, 0x20, 13);		//PSTD_buffer_size_bound

	//18
	return  18;
}

int makePsSystemMapHeader(char *psSystemMapHeader)
{
	initBits();
	
	putBits(psSystemMapHeader,0x000001, 24);	// psm packet_start_code_prefix
	putBits(psSystemMapHeader,0xBC, 8);			// map stream id, shall be 0xBC

	putBits(psSystemMapHeader, 0x15, 16);		//program_stream_map_length is 24(0x18);or 0x12
	// 6

	putBits(psSystemMapHeader, 0x01, 1);       //current next indicator 
	putBits(psSystemMapHeader, 0x03, 2);		//reserved
	putBits(psSystemMapHeader, 0x01, 5);       //program_stream_map_versio
	
	putBits(psSystemMapHeader, 0x7F, 7);       //reserved 
	putBits(psSystemMapHeader, 0x01, 1);       //marker bit 
	
	putBits(psSystemMapHeader, 0x00, 16);      //program_stream_info_length
	// 10
	
	putBits(psSystemMapHeader, 0x08, 16);      //elementary_stream_map_length  is 8,  include video and audio
	// 12
	//video
	putBits(psSystemMapHeader, 0x1B, 8); 		//stream_type h.264
	putBits(psSystemMapHeader, 0xE0, 8);		//elementary_stream_id
	putBits(psSystemMapHeader, 0x00, 16);		//elementary_stream_info_length is 6(include 6 bytes description info as below)
	// 16

	//audio
	putBits(psSystemMapHeader, 0x03, 8);		//stream_type0x90  0x03:11172aac 0x4:13818-3 0x0f:m4a
	putBits(psSystemMapHeader, AV_TYPE_AUDIO_STREAM_ID, 8);		//elementary_stream_id
	putBits(psSystemMapHeader, 0x03, 16);		//elementary_stream_info_length is 0

	putBits(psSystemMapHeader, 0x03, 8);
	putBits(psSystemMapHeader, 0x02, 8);
	putBits(psSystemMapHeader, 0x07, 8);
	// 23
	
	//crc (2e b9 0f 3d)
	putBits(psSystemMapHeader, 0x2E, 8);		//crc (24~31) bits
	putBits(psSystemMapHeader, 0xB9, 8);		//crc (16~23) bits
	putBits(psSystemMapHeader, 0x0F, 8);		//crc (8~15) bits
	putBits(psSystemMapHeader, 0x3D, 8);		//crc (0~7) bits
	// 27

	return 27;
}
