/*
 * FilePS.h
 *
 *  Created on: Mar 25, 2014
 *      Author: foscam
 */

#ifndef FILEPS_H_
#define FILEPS_H_

#include "PSPacket.h"

//包含其他头文件
#include "BaseDataType.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
using namespace std;
using namespace HiroCamSDK;

class CFilePS: public CPSPacket {
public:
	enum RECORD_FILE_TYPE
	{
		RECORD_FILE_TYPE_MANUAL,
		RECORD_FILE_TYPE_SCHEDULE,
		RECORD_FILE_TYPE_ALARM,
	};

public:
	CFilePS(int fileBufSize);
	virtual ~CFilePS();

public:
	int openRecordFile(const char* path, int recordType);
	int closeRecordFile(void);
	virtual int packH264Frame(char* data, int dataLen, int isKeyFrame, HR_U64 timeStamp);
	virtual int packAACFrame(char* data, int dataLen, HR_U64 timeStamp);
protected:
	virtual int onBufferOverflow(void);
	virtual int onExceedMaxSize(void);
	virtual int onTimeStampError(void);

private:
	FILE* fp;
	char recordPath[256];
	int recordFileType;
	bool	bFirst;
};

#endif /* FILEPS_H_ */
