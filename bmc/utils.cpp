#include <windows.h>
#include <time.h>

int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tmv;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tmv.tm_year = wtm.wYear - 1900;
	tmv.tm_mon = wtm.wMonth - 1;
	tmv.tm_mday = wtm.wDay;
	tmv.tm_hour = wtm.wHour;
	tmv.tm_min = wtm.wMinute;
	tmv.tm_sec = wtm.wSecond;
	tmv.tm_isdst = -1;
	clock = mktime(&tmv);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}
