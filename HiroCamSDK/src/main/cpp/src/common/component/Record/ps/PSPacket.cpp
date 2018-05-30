/*
 * PSPacket.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: foscam
 */

#include "PSPacket.h"
//#include "debugLog.h"

//#include <sys/time.h>

#define PS_PAYLOAD_MAX_SIZE			65500
#define ABS(x) ((x)<0 ? -(x) : (x))

CPSPacket::CPSPacket(int psBufSize)
{
	psBuf = new char[psBufSize];
	maxBufSize = psBufSize;
	bufSize = 0;
	lastTimeStamp = 0;
	ui64SCR = 0;
	totalLen = 0;
	maxFileSize = 0;
}

CPSPacket::~CPSPacket()
{
	if( psBuf )
	{
		delete []psBuf;
		psBuf = 0;
	}
}

void CPSPacket::init(int maxFileSize)
{
	bufSize = 0;
	lastTimeStamp = 0;
	ui64SCR = 0;
	totalLen = 0;
	this->maxFileSize = maxFileSize;
}

int CPSPacket::packH264Frame(char* data, int dataLen, int isKeyFrame, HR_U64 timeStamp)
{
	HR_U64 duration;
	if( lastTimeStamp == 0 )
	{
		duration = 33;
	}
	else
	{
		duration = ABS(timeStamp-lastTimeStamp);
	}
	if( duration > 5000) //5s
	{
		//DEBUG_ERROR("Timestamp check fail, timeStamp:%lld, lastTimeStamp:%lld", timeStamp, lastTimeStamp);
		onBufferOverflow();
		onTimeStampError();
	}
	lastTimeStamp = timeStamp;

	ui64SCR += duration*90;

	HR_U64 pts = 0, dts = 0;
	dts = ui64SCR%0x0000000200000000ll;
	pts = dts;

	char* packPoint = data;
	int remainLen = dataLen;
	int packLen = 0;
	bool isFirstFrag = true;

	while( remainLen > 0 )
	{
		packLen = (remainLen>PS_PAYLOAD_MAX_SIZE)?PS_PAYLOAD_MAX_SIZE:remainLen;
		if( isFirstFrag )
		{
			if( (bufSize+sizeof(PS_HEADER)) > maxBufSize )
			{
				onBufferOverflow();
				bufSize = 0;
			}
			int psHeaderSize = makePsPacketHeader(psBuf+bufSize, dts, 0, 512*1024/50);
			bufSize += psHeaderSize;
			totalLen += psHeaderSize;

			if( isKeyFrame )
			{
				if( (bufSize+sizeof(PS_SYSTEM_HEADER)) > maxBufSize )
				{
					onBufferOverflow();
					bufSize = 0;
				}
				int psSystemHeaderSize = makePsSystemHeader(psBuf+bufSize, 512*1024/50);
				bufSize += psSystemHeaderSize;
				totalLen += psSystemHeaderSize;

				if( (bufSize+sizeof(PSM)) > maxBufSize )
				{
					onBufferOverflow();
					bufSize = 0;
				}
				int psSystemMapHeaderSize = makePsSystemMapHeader(psBuf+bufSize);
				bufSize += psSystemMapHeaderSize;
				totalLen += psSystemMapHeaderSize;
			}

			if( (bufSize+sizeof(PES_HEADER)+sizeof(PTS)+sizeof(PTS)) > maxBufSize )
			{
				onBufferOverflow();
				bufSize = 0;
			}
			int pesHeaderSize = makeVideoPesPakectHeader(psBuf+bufSize, packLen, pts, dts);
			bufSize += pesHeaderSize;
			totalLen += pesHeaderSize;

			if( data != NULL )
			{
				if( (bufSize+packLen) > maxBufSize )
				{
					onBufferOverflow();
					bufSize = 0;
				}
				memcpy( (psBuf+bufSize), packPoint, packLen );
				bufSize += packLen;
				totalLen += packLen;
			}

			packPoint += packLen;
			remainLen -= packLen;

			isFirstFrag = false;
		}
		else
		{
			if( (bufSize+sizeof(PES_HEADER)+sizeof(PTS)+sizeof(PTS)) > maxBufSize )
			{
				onBufferOverflow();
				bufSize = 0;
			}
			int pesHeaderSize = makeVideoPesPakectHeader(psBuf+bufSize, packLen, pts, dts);
			bufSize += pesHeaderSize;
			totalLen += pesHeaderSize;

			if( (bufSize+packLen) > maxBufSize )
			{
				onBufferOverflow();
				bufSize = 0;
			}
			if( data != NULL )
			{
				memcpy( (psBuf+bufSize), packPoint, packLen );
				bufSize += packLen;
				totalLen += packLen;
			}

			packPoint += packLen;
			remainLen -= packLen;
		}
	}

	if( totalLen >= maxFileSize )
	{
		//DEBUG_WARNING("TotalLen:%llu, maxFileSize:%llu", totalLen, maxFileSize);
		onBufferOverflow();
		onExceedMaxSize();
	}

	return 0;
}

int CPSPacket::packAACFrame(char* data, int dataLen, HR_U64 timeStamp)
{
	HR_U64 duration;
	if( lastTimeStamp == 0 )
	{
		duration = 60;
	}
	else
	{
		duration = ABS(timeStamp-lastTimeStamp);
	}

	if( duration > 5000)				//5s
	{
		//DEBUG_ERROR("Timestamp check fail, timeStamp:%lld, lastTimeStamp:%lld", timeStamp, lastTimeStamp);

 		onBufferOverflow();
 		onTimeStampError();

	}
	lastTimeStamp = timeStamp;

	ui64SCR += duration*90;

	HR_U64 pts = 0, dts = 0;
	dts = ui64SCR%0x0000000200000000ll;
	pts = dts;

	// PS
	if( (bufSize+sizeof(PS_HEADER)) > maxBufSize )
	{
		onBufferOverflow();
		bufSize = 0;
	}
	int psHeaderSize = makePsPacketHeader(psBuf+bufSize, dts, 0, 16000/50);
	bufSize += psHeaderSize;
	totalLen += psHeaderSize;

	// PES
	if( (bufSize+sizeof(PES_HEADER)+sizeof(PTS)+sizeof(PTS)) > maxBufSize )
	{
		onBufferOverflow();
		bufSize = 0;
	}
	int pesHeaderSize = makeAudioPesPakectHeader(psBuf+bufSize, dataLen, pts, dts);
	bufSize += pesHeaderSize;
	totalLen += pesHeaderSize;

	if( (bufSize+dataLen) > maxBufSize )
	{
		onBufferOverflow();
		bufSize = 0;
	}
	if( data != NULL )
	{
		memcpy( (psBuf+bufSize), data, dataLen );
		bufSize += dataLen;
		totalLen += dataLen;
	}

	if( totalLen >= maxFileSize )
	{
		//DEBUG_WARNING("TotalLen:%llu, maxFileSize:%llu", totalLen, maxFileSize);
		onBufferOverflow();
		onExceedMaxSize();
	}

	return 0;
}

// EOF
