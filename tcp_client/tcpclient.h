#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

typedef struct _SYSTEMTIME
{
	unsigned short wYear;
	unsigned short wMonth;
	unsigned short wDayOfWeek;
	unsigned short wDay;
	unsigned short wHour;
	unsigned short wMinute;
	unsigned short wSecond;
	unsigned short wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

void        LogInfo(const char *fmt, ...);
void        GetTime(SYSTEMTIME *systime);
void        GetLocalTime(LPSYSTEMTIME lpSystemTime);

#endif

