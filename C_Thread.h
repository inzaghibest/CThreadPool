// CThread.h
#ifndef C_THREAD_H_H
#define C_THREAD_H_H 
#include <WINDOWS.H>
#include <PROCESS.H>
#include <stdio.h>
#include "C_Event.h"
#include "C_Thread_Exception.h"

//------------------------------------------------------------------------
//EPriority:定义线程的优先级
//------------------------------------------------------------------------
typedef enum
{
	tpLow,
	tpMedium = tpLow + 1,
	tpHigh = tpMedium + 1
} EPriority;

//------------------------------------------------------------------------
// EThreadStatus:定义线程的状态
//------------------------------------------------------------------------
typedef enum
{
	etsThreadInit = 0, // 初始化
	etsThreadCreate = 1, // 创建
	etsThreadSuspend = 2, // 挂起
	etsThreadRunning = 3, // 运行
	etsThreadBlocked = 4, // 阻塞
	etsThreadTimeBlocked = 5, // 限时等待
	etsThreadFinish = 6 // 结束

}EThreadStatus;

//------------------------------------------------------------------------
// 类 　: CRunable
// 用途 : 线程执行单元,每个线程类CThread实际调用此类的run方法
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
// 类 　: CThread
// 用途 : 线程类
// 用法 : 一个典型的使用,CThread MyThread(Runable* pRunable)
// MyThread.start() // 创建线程
// MyThread.notify() // 通知线程执行
// MyTreahd.join() // 等待线程执行完成
//------------------------------------------------------------------------
class CThread
{
private:
	// 线程函数
	friend unsigned int __stdcall ThreadFunc(void* lpParam);
	HANDLE m_hThreadHandle;
	unsigned int m_nThreadId;
	EThreadStatus m_etsThreadStatus; // 线程状态
	CRunable* m_pRunable; // 线程执行单元
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
	// 等待线程执行完成
	void join();
	// 线程挂起
	void suspend();
	// 线程恢复
	void resume();
	// 线程等待
	void wait();
	// 限时等待
	// void wait(int nSecs);
	// 线程通知
	void notify();
	void finish()
	{
		m_etsThreadStatus = etsThreadFinish;
	}

};
#endif // C_THREAD_H_H

