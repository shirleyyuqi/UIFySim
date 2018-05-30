/*
 * PSPacket.h
 *
 *  Created on: Mar 21, 2014
 *      Author: foscam
 */

#ifndef PSPACKET_H_
#define PSPACKET_H_

#include "psHeader.h"

//包含其他头文件
#include "BaseDataType.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
using namespace std;
using namespace HiroCamSDK;

//#include <stdint.h>


class CPSPacket {
public:
	CPSPacket(int psBufSize);
	virtual ~CPSPacket();

public:
	void init(int maxFileSize);
	virtual int packH264Frame(char* data, int dataLen, int isKeyFrame, HR_U64 timeStamp);
	virtual int packAACFrame(char* data, int dataLen, HR_U64 timeStamp);

protected:
	virtual int onBufferOverflow(void) = 0;
	virtual int onExceedMaxSize(void) = 0;
	virtual int onTimeStampError(void) = 0;

protected:
	char* psBuf;
	unsigned int bufSize;
	unsigned int maxBufSize;
	HR_U64 lastTimeStamp;
	HR_U64 ui64SCR;
	HR_U64 totalLen;
	HR_U64 maxFileSize;
};

#endif /* PSPACKET_H_ */
