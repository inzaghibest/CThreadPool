
#include "C_Thread.h"
#include "C_ThreadPool.h"
#include "C_Mutex.h"
#include "CLog.h"


// 互斥锁测试
HANDLE g_hMutex = CreateMutex(NULL, false, NULL);
class CMyTask:public CTask
{
private:
	static long m_nTaskCount;
public:
	static long get_taskCount(){return m_nTaskCount;} 
public:
	CMyTask(int nPriority = 0):CTask(nPriority)
	{
		m_nTaskCount++;
	}
	~CMyTask()
	{
	}
	virtual void Run()
	{
		{
			int i = 0;
			char szLog[128];
			memset(szLog, 0x0, sizeof(szLog));
			while(i<=10000)
			{
				i++;
				if (i == 10000)
				{
					sprintf(szLog, "第%d个任务处理完成,处理线程id:%d %d",m_nTaskCount, get_threadId(), i);
					writelog(szLog);
				}
				printf("线程id:%d: %d", get_threadId(), i);
			}
		}
	}
};
long CMyTask::m_nTaskCount = 0;

int main()
{
	/*char szBuffer[128];
	memset(szBuffer, 0x0, sizeof(szBuffer));
	get_local_time(szBuffer);
	printf("%s", szBuffer);
	writelog("hello");
	Sleep(2000);
	writelog("world!");*/
	CThreadPool* pThreadPool = new CThreadPool();
	pThreadPool->start();
	int nTaskCount = 0;
	int i = 10;
	while (i>=0)
	{
		int j = i*1;
		printf("%d\r\n", j);
		for (j; j>0;j--)
		{
			CMyTask* pMyTask = new CMyTask;
			pThreadPool->push_task(pMyTask);
			nTaskCount++;
			char szLog[64];
			memset(szLog, 0x0,sizeof(szLog));
			sprintf(szLog, "添加第%d个任务", nTaskCount);
			writelog(szLog);
		}
		Sleep(i*10000);
		if (i == 0)
		{
			i = 10;
		}
	}

	/* CWorkThread Test */
    /*CWorkThread* pWrokThread = new CWorkThread();
	CMyTask* pMyTask = new CMyTask();
	pWrokThread->set_task(pMyTask);
	Sleep(10);
	pWrokThread->suspend();
	Sleep(5000);
	pWrokThread->resume();
	pWrokThread->join();
	delete pWrokThread;*/

    // 任务优先队列测试
	/*CMyTask* pMyTask1 = new CMyTask(1);
	CMyTask* pMyTask2 = new CMyTask(2);
	CMyTask* pMyTask3 = new CMyTask(3);
	CTaskQueue taskQueue;
	taskQueue.push_task(pMyTask2);
	taskQueue.push_task(pMyTask1);
	taskQueue.push_task(pMyTask3);
	PTask pTask = taskQueue.pop_task();
	printf("任务优先级%d", pTask->get_taskPriority());
	pTask = taskQueue.pop_task();
	printf("任务优先级%d", pTask->get_taskPriority());
	pTask = taskQueue.pop_task();
	printf("任务优先级%d", pTask->get_taskPriority());
	delete pMyTask1;
	delete pMyTask2;
	delete pMyTask3;*/

	//CMyRunable* pMyRunable = new CMyRunable();
	//CThread MyThread(pMyRunable);
	/*MyThread.start();
	Sleep(100);
	printf("线程状态%d\r\n", MyThread.get_threadStatus());
	MyThread.suspend();
	Sleep(100);
	printf("线程状态%d\r\n", MyThread.get_threadStatus());
	MyThread.resume();
	Sleep(100);
	printf("线程状态%d\r\n", MyThread.get_threadStatus());
	MyThread.notify();
	Sleep(100);
	printf("线程状态%d\r\n", MyThread.get_threadStatus());
	MyThread.join();
	printf("线程状态%d\r\n", MyThread.get_threadStatus());*/
	/*if (pMyRunable)
	{
		delete pMyRunable;
	}*/
	getchar();
	return 0;
}