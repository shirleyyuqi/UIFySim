#pragma once
#include "BaseDataType.h"
#include "HiroCamDef.h"

#define FILEMAXSIZE 256*1024*1024
#define TIMESETP 1024				//“Ù∆µ∞¸º‰∏Ù÷µ°£
namespace HiroCamSDK
{
	class IRecord
	{
	public:
        virtual ~IRecord(){}
		virtual HRSDK_CMDRESULT OpenFile(const HR_CHAR* filename) = 0;
		virtual HR_S32 WriteVideo(HRSDK_DecFrame* data) = 0;
		virtual HR_S32 WriteAudio(HRSDK_DecFrame* data) = 0;
		virtual HRSDK_CMDRESULT CloseFile() = 0;
	};
}


