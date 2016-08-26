// C_ThreadPool.h
#ifndef C_THREADPOOL_H_H
#define C_THREADPOOL_H_H
#include "C_Event.h"
#include "C_Thread.h"
#include "C_Task.h"
#include "CLog.h"
#define MAX_THREAD_COUNT 10
#define MIN_IDLTHREAD_COUNT 5
#define MAX_IDLTHREAD_COUNT 5
#define RECOVER_TIME        120
class CThreadPool;
//------------------------------------------------------------------------
// 类 　: CWorkThread
// 用途 : 工作线程,控制线程创建和执行。一个典型的使用案例
// CWorkThread MyWorkThread;
// MyWorkThread.set_task(PTask)
// MyWorkThread.join()
//------------------------------------------------------------------------
class CWorkThread:public CRunable
{
private:
	CThread* m_pThread; // 线程
	CTask* m_pTask; // 任务
	CThreadPool* m_pThreadPool; // 线程池对象
	CEvent m_event;
	bool m_bStop; // 控制线程结束的标志
	static int m_nThreadCount;
public:
	CWorkThread(CThreadPool* pThreadPool = NULL);
	~CWorkThread();
	static int get_ThreadCount(){return m_nThreadCount;}
	// 给工作线程分配任务
	void set_task(CTask* pTask);
	// 任务实际执行由此方法实现
	void run();
	// 等待线程执行完成
	void join();
	// 停止线程执行
	void stop();
	void suspend()
	{
		m_pThread->suspend();
	}
	void resume()
	{
		m_pThread->resume();
	}
	CThread* get_thread()
	{
		return m_pThread;
	}
};

//------------------------------------------------------------------------
// 类 　: CQueueHandler
// 用途 : 处理任务队列的线程,将任务分配给空闲线程
//------------------------------------------------------------------------
class CQueueHandler:public CRunable
{
private:
	CThread* m_pThread;
	CThreadPool* m_pThreadPool;
	bool m_bStop;
public:
	CQueueHandler(CThreadPool* pThreadPool);
	~CQueueHandler();
	void run();
	void stop();

};

//------------------------------------------------------------------------
// 类 　: CRecoverHandler
// 用途 : 空闲线程回收线程,负责定时对多余空闲线程进行回收
//------------------------------------------------------------------------
class CRecoverHandler:public CRunable
{
private:
	CThread* m_pThread;
	CThreadPool* m_pThreadPool;
	bool m_bStop;
public:
	CRecoverHandler(CThreadPool* pThreadPool);
	~CRecoverHandler();
	void run();
	void stop();
};

//------------------------------------------------------------------------
// 类 　: CWorkList
// 用途 : 工作线程列表
//------------------------------------------------------------------------
class CWorkList
{
private:
	vector<CWorkThread*> m_pvtWorker;
	CEvent m_event;
public:
	void push_worker(CWorkThread* pWork);
	CWorkThread* pop_worker();
	void erase_worker(CWorkThread* pWork);
	void clear();
	void wait();
	int wait(int nSecs);
	void notify();
};
/************************************************************************/
/*        线程池实现                                                    */
/************************************************************************/
class CThreadPool
{
private:
	int m_nMaxThreads; // 允许的同时存在的最大线程数
	int m_nMaxIdleThreads; // 最大允许的空闲线程
	int m_nMinIdleThreads; // 最小允许的空闲线程
	int m_nBusiThreads; // 当前忙碌线程 
	//int m_nMaxQueueSize; // 任务队列最大数量
	int m_nTotalThread; // 当前创建的线程数
	bool m_bStop; // 记录线程池状态
private:
	CTaskQueue m_cTaskQueue; // 要执行的任务队列
	CWorkList m_ltWorkList; // 用来记录创建的线程,最后用于释放资源
	CWorkList m_ltIdlList; // 空闲线程列表
	HANDLE m_hmutex;
	CQueueHandler* m_pQueueHandler; // 任务调度线程
	CRecoverHandler* m_pRecoverHandler; // 定时回收线程

public:
	CThreadPool(int nMaxThreads = MAX_THREAD_COUNT, int nMaxIdleThreads = MAX_IDLTHREAD_COUNT,
		int nMinIdleThreads = MIN_IDLTHREAD_COUNT);
	~CThreadPool(){}
	void start();
	void stop();
	// 添加取出任务
	void push_task(PTask pTask);
	PTask pop_task();
public:
	// 处理任务函数
	void run_task(PTask pTask);
	// 回收空闲线程
	void recover_idlThread();
	void create_wroker(int nWorks);
	// 空闲线程通知
	void idl_toPool(CWorkThread* pWork);
	void notify_task();
	void wait_task();




};
#endif // C_THREADPOOL_H_H