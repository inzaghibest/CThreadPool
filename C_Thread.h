// CThread.h
#ifndef C_THREAD_H_H
#define C_THREAD_H_H 
#include <WINDOWS.H>
#include <PROCESS.H>
#include <stdio.h>
#include "C_Event.h"
#include "C_Thread_Exception.h"

//------------------------------------------------------------------------
//EPriority:�����̵߳����ȼ�
//------------------------------------------------------------------------
typedef enum
{
	tpLow,
	tpMedium = tpLow + 1,
	tpHigh = tpMedium + 1
} EPriority;

//------------------------------------------------------------------------
// EThreadStatus:�����̵߳�״̬
//------------------------------------------------------------------------
typedef enum
{
	etsThreadInit = 0, // ��ʼ��
	etsThreadCreate = 1, // ����
	etsThreadSuspend = 2, // ����
	etsThreadRunning = 3, // ����
	etsThreadBlocked = 4, // ����
	etsThreadTimeBlocked = 5, // ��ʱ�ȴ�
	etsThreadFinish = 6 // ����

}EThreadStatus;

//------------------------------------------------------------------------
// �� ��: CRunable
// ��; : �߳�ִ�е�Ԫ,ÿ���߳���CThreadʵ�ʵ��ô����run����
//------------------------------------------------------------------------
class CThread;
class CRunable
{
private:
	CThread* m_pThread;
	CEvent m_event;
public:
	CRunable(CThread* pThread = NULL):m_pThread(pThread)
	{

	}
	~CRunable(){}
	void set_owner(CThread* pThread)
	{
		m_pThread = pThread;
	}
	CThread* get_thread()
	{
		return m_pThread;
	}
	int wait(int nSecs){return m_event.wait(nSecs);}
	void notify(){m_event.notify();}
	virtual void run() = 0;
};
//------------------------------------------------------------------------
// �� ��: CThread
// ��; : �߳���
// �÷� : һ�����͵�ʹ��,CThread MyThread(Runable* pRunable)
// MyThread.start() // �����߳�
// MyThread.notify() // ֪ͨ�߳�ִ��
// MyTreahd.join() // �ȴ��߳�ִ�����
//------------------------------------------------------------------------
class CThread
{
private:
	// �̺߳���
	friend unsigned int __stdcall ThreadFunc(void* lpParam);
	HANDLE m_hThreadHandle;
	unsigned int m_nThreadId;
	EThreadStatus m_etsThreadStatus; // �߳�״̬
	CRunable* m_pRunable; // �߳�ִ�е�Ԫ
	CEvent m_event;
public:
	CThread(CRunable* pRunable);
	~CThread();
public:
	EPriority get_threadPriority();
	void set_threadPriority(EPriority ePriority);
	EThreadStatus get_threadStatus();
	int get_threadId()
	{
		return m_nThreadId;
	}
public:
	void start(bool bSuspend = false);
	void excute();
	// �ȴ��߳�ִ�����
	void join();
	// �̹߳���
	void suspend();
	// �ָ̻߳�
	void resume();
	// �̵߳ȴ�
	void wait();
	// ��ʱ�ȴ�
	// void wait(int nSecs);
	// �߳�֪ͨ
	void notify();
	void finish()
	{
		m_etsThreadStatus = etsThreadFinish;
	}

};
#endif // C_THREAD_H_H

