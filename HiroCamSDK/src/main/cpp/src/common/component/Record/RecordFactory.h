#pragma once
#include "IRecord.h"

namespace HiroCamSDK{
    
	class CRecordFactory
	{
	public:
		static IRecord* CreateRecord(HRRECORDTYPE type);
		static void ReleaseRecord(IRecord* record);
	};
}

