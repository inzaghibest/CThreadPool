#include "stdafx.h"
#include "public.h"

int get_lastError()
{
	int nErrorCode = 0;
#ifdef __Windows
	nErrorCode = ::GetLastError();
#else
	nErrorCode = errno;
#endif
	return nErrorCode;
}

string get_sysErrorMessage(int nErrorCode)
{
	string strMsg;
#ifdef __Windows

	long nLen;
	char szBuffer[256];
	nLen = FormatMessage
		(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
		NULL, nErrorCode, 0, szBuffer, sizeof(szBuffer), NULL);

	strMsg = string(szBuffer, nLen);
#else

	strMsg = strerror(nErrorCode);
#endif
	return strMsg;
}