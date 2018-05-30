#include "HRTime.h"

namespace HiroCamSDK
{

	HR_U64 CTime::GetUTCTime_Ms()
	{
#ifdef _WIN32
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);
		LARGE_INTEGER *pli= (LARGE_INTEGER *)&ft;

		// 116444736000000000 是由以下方法得来
		/*
		SYSTEMTIME st;
		FILETIME ft;
		st.wYear=1970;
		st.wMonth=1;
		st.wDay=1;
		st.wHour=0;
		st.wMinute=0;
		st.wSecond=0;
		st.wMilliseconds=0;
		SystemTimeToFileTime(&st,&ft);
		*/

		return HR_U64((pli->QuadPart - 116444736000000000)/10000);
#else
		long long msec = 0;
		struct timeval tv;
		gettimeofday( &tv, NULL );
		msec += (long long)tv.tv_sec * 1000;
		msec += (long long)tv.tv_usec / 1000;
		return msec;
#endif
	}

	HR_U64 CTime::GetLocalTime_Ms()
	{
#ifdef _WIN32
		SYSTEMTIME st;
		FILETIME ft;
		::GetLocalTime(&st);
		::SystemTimeToFileTime(&st, &ft);
		LARGE_INTEGER *pli= (LARGE_INTEGER *)&ft;
		return HR_U64((pli->QuadPart - 116444736000000000)/10000);
#else
		long long msec = 0;
		struct timeval tv;
		struct timezone tz;
		gettimeofday( &tv, &tz );
		msec += (long long)tv.tv_sec * 1000;
		msec += (long long)tv.tv_usec / 1000;
		return msec + tz.tz_minuteswest * 60 * 1000;
#endif
	}

	HR_U32 CTime::GetUTCTime_S()
	{
		return (HR_U32)time(0); //时间不可能为负数，此处强转不会有问题
	}

	HR_U32 CTime::GetLocalTime_S()
	{
#ifdef _WIN32
		SYSTEMTIME st;
		FILETIME ft;
		::GetLocalTime(&st);
		::SystemTimeToFileTime(&st, &ft);
		LARGE_INTEGER *pli= (LARGE_INTEGER *)&ft;
		return HR_U32((pli->QuadPart - 116444736000000000)/10000000);
#else
		struct timeval tv;
		struct timezone tz;
		gettimeofday( &tv, &tz );
		return HR_U32(tv.tv_sec + tz.tz_minuteswest * 60);
#endif
	}

	HR_U32 CTime::UTC2Localtime(HR_U32 t)
	{
#ifdef _WIN32
		TIME_ZONE_INFORMATION tz;
		::GetTimeZoneInformation(&tz);
		return HR_U32(t - tz.Bias*60);
		//return HR_U32((((LARGE_INTEGER*)&ft)->QuadPart - 116444736000000000)/10000000);
#else
		struct timeval tv;
		struct timezone tz;
		gettimeofday( &tv, &tz );
		return t + tz.tz_minuteswest * 60;
#endif
	}

	HR_U32 CTime::Localtime2UTC(HR_U32 t)
	{
#ifdef _WIN32
		TIME_ZONE_INFORMATION tz;
		::GetTimeZoneInformation(&tz);
		return HR_U32(t + tz.Bias*60);
#else
		struct timeval tv;
		struct timezone tz;
		gettimeofday( &tv, &tz );
		return t - tz.tz_minuteswest * 60;
#endif
	}

	HR_U32 CTime::GetTimeZone()
	{
#ifdef _WIN32
		TIME_ZONE_INFORMATION tz;
		::GetTimeZoneInformation(&tz);
		return tz.Bias * 60;
#else
		struct timeval tv;
		struct timezone tz;
		gettimeofday( &tv, &tz );
		return tz.tz_minuteswest * 60;
#endif
	}
}



