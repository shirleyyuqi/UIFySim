/*
 * FilePS.cpp
 *
 *  Created on: Mar 25, 2014
 *      Author: foscam
 */

#include "FilePS.h"

#include <time.h>
#include <errno.h>

CFilePS::CFilePS(int fileBufSize) : CPSPacket(fileBufSize){
	fp = NULL;
	memset( recordPath, 0x00, sizeof(recordPath) );
	recordFileType = 0;
}

CFilePS::~CFilePS() {
	closeRecordFile();
}

int CFilePS::openRecordFile(const char* path, int recordType)
{
	char filePrefix[64] = {0};
	recordFileType = recordType;
	int maxFileSize = 0;
	if( recordFileType==RECORD_FILE_TYPE_MANUAL )
	{
		maxFileSize = 256*1024*1024;
		strncpy( filePrefix, "manual", 64 );
	}
	else if(recordFileType==RECORD_FILE_TYPE_SCHEDULE)
	{
		maxFileSize = 256*1024*1024;
		strncpy( filePrefix, "schedule", 64 );
	}
	else if( recordFileType==RECORD_FILE_TYPE_ALARM )
	{
		if (fp)
		{
			maxFileSize += 64*1024*1024;
			return 0;
		}
		
		maxFileSize = 64*1024*1024;
		strncpy( filePrefix, "alarm", 64 );
	}
	init(maxFileSize);

	strncpy( recordPath, path, 256 );

	char recordFilePathName[256] = {0};
	// Create record file
	struct tm *tm_time;
	time_t curTime = time(0);//CAppTimer::getSecond();
	tm_time = gmtime( &curTime);
	// example: alarm_20120206_141400.avi
	sprintf(recordFilePathName, "%s/%s_%04d%02d%02d_%02d%02d%02d.mpg",recordPath, filePrefix,
			tm_time->tm_year + 1900,
			tm_time->tm_mon + 1,
			tm_time->tm_mday,
			tm_time->tm_hour,
			tm_time->tm_min,
			tm_time->tm_sec );

	//这个函数中，在这之前的那么多代码，其实都是在构建路径，然后创建文件名，而现在录像模块不需要这样做，因为现在的录像模块是录像文件名直接由外界穿进来。洪伟焕，20141013.
	fp = fopen(path, "wb");					//创建文件。
	if( fp == NULL )
	{
		//DEBUG_ERROR("Open file:%s fail:%s", recordFilePathName, strerror(errno));
		return 1;	
	}
	bFirst = true;
	return 0;
}

int CFilePS::closeRecordFile(void)
{
	onBufferOverflow();

	if( fp != NULL )
	{
		//long long start = localtime(time(0));//CAppTimer::getmSecond();
		fflush( fp );
		fclose( fp );
		//long long end = localtime(time(0));//CAppTimer::getmSecond();
		//DEBUG_INFO("Close record file, cost:%lld ms", end-start);
		fp = NULL;
	}
	return 0;
}

int CFilePS::onBufferOverflow(void)
{
	if( fp != NULL )
	{
		if( bufSize > 0 )
		{
			//long long start = time(0);//CAppTimer::getmSecond();
			int ret = fwrite( psBuf, 1, bufSize, fp );
			if (ret != bufSize)
			{
				int a=0;
				a = 0;
			}
			
			fflush( fp );
			//long long end = CAppTimer::getmSecond();
			//DEBUG_INFO("Write record file, len:%d, cost:%lld ms", bufSize, end-start);
		}
	}

	return 0;
}

int CFilePS::onExceedMaxSize(void)
{
	//DEBUG_INFO("Record exceed max size");

	closeRecordFile();
	if( recordFileType!=RECORD_FILE_TYPE_ALARM )
	{
		openRecordFile(recordPath, recordFileType);
	}
	return 0;
}

int CFilePS::onTimeStampError(void)
{
	//DEBUG_INFO("Record timestamp error");

	closeRecordFile();
	openRecordFile(recordPath, recordFileType);

	return 0;
}

int CFilePS::packH264Frame(char* data, int dataLen, int isKeyFrame, HR_U64 timeStamp)
{
	if (fp)
	{
		if(bFirst && !isKeyFrame){
			return 0;
		}
		bFirst = false;
		return CPSPacket::packH264Frame(data, dataLen, isKeyFrame, timeStamp);
	}
	return 0;
}

int CFilePS::packAACFrame(char* data, int dataLen, HR_U64 timeStamp)
{
	if (fp && !bFirst)
	{
		return CPSPacket::packAACFrame(data, dataLen, timeStamp);
	}
	return 0;
}


// EOF


