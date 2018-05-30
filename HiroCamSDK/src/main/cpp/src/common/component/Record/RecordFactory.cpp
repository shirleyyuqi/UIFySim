#include "RecordFactory.h"
#include "MP4Record.h"
#include "AVIRecord.h"
// #include "RecordPsFile.h"

namespace HiroCamSDK{
	IRecord* CRecordFactory::CreateRecord(HRRECORDTYPE type)
	{
		switch(type)
		{
#ifdef _USE_MP4
		case HRRCTYPE_MP4:
			return new CMP4Record();
#endif

		case HRRCTYPE_PS:
			//return new CFosRecordPsFile();
			break;

		case HRRCTYPE_AVI:
			return new CAVIRecord();
			break;

		default:
			//FOSLOG_W("not that file type!");
			break;
		}
		return NULL;
	}

	void CRecordFactory::ReleaseRecord(IRecord* record)
	{
		delete record;
	}
}
