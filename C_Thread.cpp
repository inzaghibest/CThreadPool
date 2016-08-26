// C_Thread.cpp
#include "stdafx.h"
#include "C_Thread.h"
#include "public.h"

unsigned int __stdcall ThreadFunc(void* lpParam)
{
	CThread* pThread = (CThread*)(lpParam);
	pThread->wait();
	pThread->excute();
	pThread->finish();
	return 0;
}

CThread::CThread(CRunable* pRunable)
{
	m_hThreadHandle = INVALID_HANDLE_VALUE;
	m_nThreadId = -1;
	m_pRunable = pRunable;
	pRunable->set_owner(this);
    // ���߳�״̬��ʼ��
	m_etsThreadStatus = etsThreadInit;
}

/* �����߳� */
void CThread::start(bool bSuspend /*= false*/)
{
	if (bSuspend)
	{
		m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, CREATE_SUSPENDED, &m_nThreadId);
		m_etsThreadStatus = etsThreadSuspend;
	}else
	{
        m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, 0, &m_nThreadId);
		m_etsThreadStatus = etsThreadCreate;
	}
	
	if (m_hThreadHandle == INVALID_HANDLE_VALUE)
	{
		int nCode = get_lastError();
		m_etsThreadStatus = etsThreadFinish;
		throw CThreadExcepiton(get_sysErrorMessage(nCode), "�����̳߳���!", nCode);
	}
}

/* �߳�ִ�� */
void CThread::excute()
{
	m_etsThreadStatus = etsThreadRunning;
	printf("�߳�ִ����...\r\n");
	m_pRunable->run();
	printf("CThread::excute() end");
	return;
}

void CThread::wait()
{
	m_etsThreadStatus = etsThreadBlocked;
	m_event.wait();
	m_etsThreadStatus = etsThreadRunning;
}

/*void CThread::wait(int nSecs)
{
	m_etsThreadStatus = etsThreadTimeBlocked;
	int nRc = m_event.wait(nSecs);
	m_etsThreadStatus = etsThreadRunning;
}*/

void CThread::notify()
{
	m_event.notify();
}

/* �ȴ��߳�ִ����� */
void CThread::join()
{
	WaitForSingleObject(m_hThreadHandle,INFINITE);
	m_etsThreadStatus = etsThreadFinish;
}

/* ��ͣ�߳�ִ�� */
void CThread::suspend()
{
	if (m_etsThreadStatus == etsThreadSuspend)
	{
		return;
	}
	// �������ʱ����?�߳�״̬?
	if (m_etsThreadStatus == etsThreadBlocked)
	{
		return;
	}
	::SuspendThread(m_hThreadHandle);
	m_etsThreadStatus = etsThreadSuspend;
}

/* �ָ��߳�ִ�� */
void CThread::resume()
{
	if (m_etsThreadStatus == etsThreadSuspend)
	{
		::ResumeThread(m_hThreadHandle);
		m_etsThreadStatus = etsThreadRunning;
	}
}

EPriority CThread::get_threadPriority()
{
	int nP = 0;
    nP = ::GetThreadPriority(m_hThreadHandle);
	return static_cast<EPriority>(nP);
}

void CThread::set_threadPriority(EPriority ePriority)
{
	::SetThreadPriority(m_hThreadHandle, ePriority);
}

EThreadStatus CThread::get_threadStatus()
{
	return m_etsThreadStatus;
}

CThread::~CThread()
{
	printf("~CThread()");
	// ����߳�û��ִ����,�ֲ��ǳ�ʼ���͹����״̬,��Ҫ�ȴ��߳�ִ�����.
	if (m_etsThreadStatus != etsThreadFinish)
	{
		if (m_etsThreadStatus == etsThreadSuspend)
		{
			resume();
			join();
		}
		if (m_etsThreadStatus == etsThreadBlocked)
		{
			notify();
			join();
		}
		if (m_etsThreadStatus == etsThreadRunning)
		{
			join();
		}
	}
	if (m_hThreadHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hThreadHandle);
	}
	m_hThreadHandle = INVALID_HANDLE_VALUE;
	m_nThreadId = -1;
}
