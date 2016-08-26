#include "CLog.h"

#define FILE_NAME = "E:\\高级程序员进阶之路\\线程同步\\线程池实现\\CThreadPool\\log\\ThreadPool.txt"
HANDLE g_hFileMutex = CreateMutex(NULL, FALSE, NULL);
void get_local_time(char* buffer)
{
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
		(timeinfo->tm_year+1900), timeinfo->tm_mon, timeinfo->tm_mday,
		timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

void writelog(char* szBuffer)
{
	FILE* pFile;
	pFile = fopen("E:\\高级程序员进阶之路\\线程同步\\线程池实现\\CThreadPool\\log\\ThreadPool.txt", "at+");
	if (pFile != NULL)
	{
		char szNow[64];
		memset(szNow, 0x0, sizeof(szNow));
		get_local_time(szNow);
		//fwrite(szNow, strlen(szNow), 1, pFile);
		char szLine[128];
		memset(szLine, 0x0, sizeof(szLine));
		sprintf(szLine, "%s:%s\n", szNow, szBuffer);
		::WaitForSingleObject(g_hFileMutex, INFINITE);
		fwrite(szLine, strlen(szLine), 1, pFile);
		::ReleaseMutex(g_hFileMutex);
		fclose(pFile);
		pFile = NULL;
	}
}
