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
// �� ��: CWorkThread
// ��; : �����߳�,�����̴߳�����ִ�С�һ�����͵�ʹ�ð���
// CWorkThread MyWorkThread;
// MyWorkThread.set_task(PTask)
// MyWorkThread.join()
//------------------------------------------------------------------------
class CWorkThread:public CRunable
{
private:
	CThread* m_pThread; // �߳�
	CTask* m_pTask; // ����
	CThreadPool* m_pThreadPool; // �̳߳ض���
	CEvent m_event;
	bool m_bStop; // �����߳̽����ı�־
	static int m_nThreadCount;
public:
	CWorkThread(CThreadPool* pThreadPool = NULL);
	~CWorkThread();
	static int get_ThreadCount(){return m_nThreadCount;}
	// �������̷߳�������
	void set_task(CTask* pTask);
	// ����ʵ��ִ���ɴ˷���ʵ��
	void run();
	// �ȴ��߳�ִ�����
	void join();
	// ֹͣ�߳�ִ��
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
// �� ��: CQueueHandler
// ��; : ����������е��߳�,���������������߳�
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
// �� ��: CRecoverHandler
// ��; : �����̻߳����߳�,����ʱ�Զ�������߳̽��л���
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
// �� ��: CWorkList
// ��; : �����߳��б�
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
/*        �̳߳�ʵ��                                                    */
/************************************************************************/
class CThreadPool
{
private:
	int m_nMaxThreads; // �����ͬʱ���ڵ�����߳���
	int m_nMaxIdleThreads; // �������Ŀ����߳�
	int m_nMinIdleThreads; // ��С����Ŀ����߳�
	int m_nBusiThreads; // ��ǰæµ�߳� 
	//int m_nMaxQueueSize; // ��������������
	int m_nTotalThread; // ��ǰ�������߳���
	bool m_bStop; // ��¼�̳߳�״̬
private:
	CTaskQueue m_cTaskQueue; // Ҫִ�е��������
	CWorkList m_ltWorkList; // ������¼�������߳�,��������ͷ���Դ
	CWorkList m_ltIdlList; // �����߳��б�
	HANDLE m_hmutex;
	CQueueHandler* m_pQueueHandler; // ��������߳�
	CRecoverHandler* m_pRecoverHandler; // ��ʱ�����߳�

public:
	CThreadPool(int nMaxThreads = MAX_THREAD_COUNT, int nMaxIdleThreads = MAX_IDLTHREAD_COUNT,
		int nMinIdleThreads = MIN_IDLTHREAD_COUNT);
	~CThreadPool(){}
	void start();
	void stop();
	// ���ȡ������
	void push_task(PTask pTask);
	PTask pop_task();
public:
	// ����������
	void run_task(PTask pTask);
	// ���տ����߳�
	void recover_idlThread();
	void create_wroker(int nWorks);
	// �����߳�֪ͨ
	void idl_toPool(CWorkThread* pWork);
	void notify_task();
	void wait_task();




};
#endif // C_THREADPOOL_H_H