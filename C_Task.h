// C_Task.h
//------------------------------------------------------------------------
// 类 　: CTask任务基类 CTaskQueue任务优先队列
// 用途 : 实现任务类的功能
//------------------------------------------------------------------------
#ifndef C_TASK_H_H
#define C_TASK_H_H
#include <queue>
#include "C_Mutex.h"
#include "C_Event.h"
using namespace std;

//------------------------------------------------------------------------
// 类 　: CTask
// 用途 : 任务的基类,传递给工作线程必须是此基类的子类,任务类的销毁在线程中负责。
// 子类需要重载实际执行任务的run方法
//------------------------------------------------------------------------
class CTask
{
public:
	CTask(int nPriority = 0)
	{
		m_nTaskCount++;
		m_nPriority = nPriority;
		m_nThreadId = -1;
	}
	~CTask(){}
public:
	virtual void Run() = 0;
	virtual void TimeOut()
	{
		printf("没有得到处理线程,超时...\r\n");
	}
	static long get_taskCount()
	{
		return m_nTaskCount;
	}
	void set_threadId(int nThreadId)
	{
		m_nThreadId = nThreadId;
	}
	int get_threadId()
	{
		return m_nThreadId;
	}
	void set_taskPriority(int nPriority)
	{
		m_nPriority = nPriority;
	}
	int get_taskPriority()
	{
		return m_nPriority;
	}
private:
	static long m_nTaskCount;
	int m_nThreadId; // 执行此任务的线程id
	int m_nPriority; // 任务优先级,优先级越大越现执行
};

/* 队列排序 */
template <class T>
struct task_priority
{
	bool operator () (const T t1, const T t2) const
	{
		if (t1->get_taskPriority() < t2->get_taskPriority())
		{
			return true;
		}
		return false;
	}
};

typedef CTask* PTask;
//------------------------------------------------------------------------
// 类 　: CTaskQueue
// 用途 : 实现任务的优先队列管理
//------------------------------------------------------------------------
class CTaskQueue
{
private:
	HANDLE m_hMutex; // 用于线程互斥访问任务队列
	CEvent m_event; // 当任务队列有新任务时通知事件
public:
	CTaskQueue()
	{
		m_hMutex = CreateMutex(NULL, false, NULL);
	}
	~CTaskQueue()
	{
		if (m_hMutex)
		{
			CloseHandle(m_hMutex);
			m_hMutex = INVALID_HANDLE_VALUE;
		}
	}
private:
	priority_queue<PTask, vector<PTask>, task_priority<PTask>> m_cTaskQueue;
public:
	PTask pop_task()
	{
		PTask pTask = NULL;
		CMutex mutex(m_hMutex);
		mutex.Lock();
		if (!m_cTaskQueue.empty())
		{
			// 获取头元素
			pTask = dynamic_cast<PTask>(m_cTaskQueue.top());
			// 清除头元素
			m_cTaskQueue.pop();
		}
		mutex.Unlock();
		return pTask;
	}

	bool push_task(PTask pTask)
	{
		CMutex mutex(m_hMutex);
		mutex.Lock();
		m_cTaskQueue.push(pTask);
		mutex.Unlock();
		m_event.notify(); // 得到新任务通知
		return true;
	}

	bool empty()
	{
		CMutex mutex(m_hMutex);
		mutex.Lock();
		bool bEmpty = m_cTaskQueue.empty();
		mutex.Unlock();
		return bEmpty;
	}

	int size()
	{
		return m_cTaskQueue.size();
	}

	// 通知新任务到来
	void notify_task()
	{
		m_event.notify();
	}
	// 等待新任务
	void wait_task()
	{
		m_event.wait();
	}


};

#endif // C_TASK_H_H