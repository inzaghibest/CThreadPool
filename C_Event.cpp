// C_Event.cpp
#include "stdafx.h"
#include "C_Event.h"
#include "public.h"
#include "C_Thread_Exception.h"
/* Ĭ�ϴ���һ���˹����õ��¼����� */
CEvent::CEvent(EventType eType /* = EVENT_SINGLE */)
{
	m_eType = eType;
	switch(eType)
	{
		case EVENT_SINGLE:
			m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			break;
		case EVENT_BORODCAST:
			m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			break;
		default:
			assert(0);
	}
	if (m_hEvent == INVALID_HANDLE_VALUE)
	{
		int nErrorCode = get_lastError();
		string strMsg = get_sysErrorMessage(nErrorCode);
        throw CErrorMsg(strMsg, nErrorCode, "�����¼�����ʧ��!"); 
	}
}

CEvent::~CEvent()
{
	if (m_hEvent != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hEvent);
	}
	m_hEvent = INVALID_HANDLE_VALUE;
}

// �����¼�����Ϊδ����״̬
void CEvent::reset()
{
	if (!ResetEvent(m_hEvent))
	{
		int nErrorCode = get_lastError();
		string strErrMsg = get_sysErrorMessage(nErrorCode);
		throw CErrorMsg(strErrMsg, nErrorCode, "ResetEvent() Error!");
	}
}

// �����¼�����Ϊ���ź�״̬
void CEvent::notify()
{
	BOOL bSuccess = FALSE;
	switch(m_eType)
	{
	case EVENT_SINGLE:
		bSuccess = SetEvent(m_hEvent);
		break;
	case EVENT_BORODCAST:
		bSuccess = PulseEvent(m_hEvent);
		break;
	default:
		assert(0);
	}
	if (!bSuccess)
	{
		int nErrorCode = get_lastError();
		string strErrMsg = get_sysErrorMessage(nErrorCode);
		throw CErrorMsg(strErrMsg, nErrorCode, "notify() error!");
	}
}

void CEvent::wait()
{
	if (WaitForSingleObject(m_hEvent, INFINITE) == WAIT_FAILED)
	{
		int nErrorCode = get_lastError();
		string strErrMsg = get_sysErrorMessage(nErrorCode);
		throw CErrorMsg(strErrMsg, nErrorCode, "wait() error");
	}
}

int CEvent::wait(int nSecs)
{
	int rc = 0;
	DWORD dwMillicSecs = 0;
	DWORD dwStatus = 0;
	dwMillicSecs = nSecs*1000;
	dwStatus = WaitForSingleObject(m_hEvent, dwMillicSecs);
	if (dwStatus == WAIT_TIMEOUT)
	{
		printf("�ȴ���ʱ\r\n");
		return dwStatus;
	}
	if (dwStatus == WAIT_FAILED)
	{
		int nErrorCode = get_lastError();
		string strErrMsg = get_sysErrorMessage(nErrorCode);
		throw CThreadExcepiton(strErrMsg, "", nErrorCode);
	}
	return rc;
}