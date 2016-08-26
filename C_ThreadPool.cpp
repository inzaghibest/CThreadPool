// C_ThreadPool.cpp
#include "C_ThreadPool.h"

// CWorkThread 
int CWorkThread::m_nThreadCount = 0;
CWorkThread::CWorkThread(CThreadPool* pThreadPool)
{
	char szLog[128];
	m_pThreadPool = pThreadPool;
	m_nThreadCount++;
	m_pTask = NULL;
	m_pThread = new CThread(this);
	memset(szLog, 0x0, sizeof(szLog));
	sprintf(szLog, "CWorkThread() 第%d次", m_nThreadCount);
	writelog(szLog);
	m_pThread->start();
	m_pThread->notify();
	m_bStop = false;
}

CWorkThread::~CWorkThread()
{
	if (m_pThread)
	{
		delete m_pThread;
		m_nThreadCount--;
		char szLog[128];
		memset(szLog, 0x0, sizeof(szLog));
		sprintf(szLog, "~CWorkThread() 第%d", m_nThreadCount);
		writelog(szLog);
	}
}

void CWorkThread::set_task(CTask* pTask)
{
	m_pTask = pTask;
	m_pTask->set_threadId(get_thread()->get_threadId());
	// 分配任务后,开始执行
	m_event.notify();
}

// 此方法在工作线程创建后，就被调用,初始等待状态，直到分配任务
void CWorkThread::run()
{
	while (true)
	{
		m_event.wait();
		// stop判断的位置很重要,放在这里,当回收线程通知时直接退出,不会在下边的线程池空闲通知,
		// 导致在将此线程放入线程池空闲线程列表中。
		if (m_bStop)
		{
			break;
		}
		m_pTask->Run();
		// 任务执行完成,销毁
		if (m_pTask)
		{
			delete m_pTask;
		}
		writelog("CWorkThread::Task end");
		// 通知线程池空闲
		m_pThreadPool->idl_toPool(this);
	}
}

void CWorkThread::stop()
{
	m_bStop = true;
	m_event.notify();
}

void CWorkThread::join()
{
	if (m_pThread)
	{
		m_pThread->join();
	}
}

// CQueueHandler
CQueueHandler::CQueueHandler(CThreadPool* pThreadPool)
{
	char szLog[128];
	memset(szLog, 0x0, sizeof(szLog));
	m_pThreadPool = pThreadPool;
	m_pThread = new CThread(this);
	writelog("CQueueHandler 任务分配线程开启...");
	m_bStop = false;
	m_pThread->start();
	m_pThread->notify();
}

CQueueHandler::~CQueueHandler()
{
	if (m_pThread)
	{
		delete m_pThread;
		writelog("CQueueHandler 任务分配线程结束...");
	}
	m_pThread = NULL;
}

void CQueueHandler::run()
{
	writelog("CQueueHandler begin...");
	while (true)
	{
		if (m_bStop)
		{
			break;
		}
		// 不断从任务队列取出任务去执行
		PTask pTask = m_pThreadPool->pop_task();
		if (pTask) // 如果有任务就执行
		{
			m_pThreadPool->run_task(pTask);
		}else // 没有任务,等待获得任务通知
		{
			m_pThreadPool->wait_task();
		}
	}
	writelog("CQueueHandler end...");
}

void CQueueHandler::stop()
{
	m_bStop = true;
	// 通知,防止还在等待获取任务
	m_pThreadPool->notify_task();
}

// CRecoverHandler
CRecoverHandler::CRecoverHandler(CThreadPool* pThreadPool)
{
	m_bStop = false;
	m_pThreadPool = pThreadPool;
	m_pThread = new CThread(this);
	writelog("CRecoverHandler 定时回收线程启动...");
	m_pThread->start();
	m_pThread->notify();
}

CRecoverHandler::~CRecoverHandler()
{
	if (m_pThread)
	{
		delete m_pThread;
		writelog("CRecoverHandler 定时回收线程结束...");
	}
	m_pThread = NULL;
}
void CRecoverHandler::run()
{
	writelog("定时回收线程Run Begin...");
	while (true)
	{
		wait(RECOVER_TIME); 
		if (m_bStop)
		{
			break;
		}
		m_pThreadPool->recover_idlThread();
	}
	writelog("定时回收线程Run End...");
}

void CRecoverHandler::stop()
{
	m_bStop = true;
	notify();

}
// CWorkList
void CWorkList::push_worker(CWorkThread* pWork)
{
	if (pWork)
	{
		m_pvtWorker.push_back(pWork);
		notify(); // 通知有一个空闲线程
		printf("%d线程空闲\r\n", pWork->get_thread()->get_threadId());
	}
}

CWorkThread* CWorkList::pop_worker()
{
	CWorkThread* pWork = NULL;
	if (!m_pvtWorker.empty())
	{
		pWork = m_pvtWorker.front();
		m_pvtWorker.erase(m_pvtWorker.begin());
	}
	return pWork;

}

void CWorkList::erase_worker(CWorkThread* pWork)
{
	if (pWork)
	{
		vector<CWorkThread*>::iterator it;
		it = find(m_pvtWorker.begin(), m_pvtWorker.end(), pWork);
		if (it != m_pvtWorker.end())
		{
			m_pvtWorker.erase(it);
		}
	}
}

void CWorkList::clear()
{
	m_pvtWorker.clear();
}

void CWorkList::wait()
{
	m_event.wait();
}

int CWorkList::wait(int nSecs)
{
	int rc = m_event.wait(nSecs);
	return rc;
}

void CWorkList::notify()
{
	m_event.notify();
}
// CThreadPool
CThreadPool::CThreadPool(int nMaxThreads, int nMaxIdleThreads, int nMinIdleThreads)
{
	char szLog[128];
	memset(szLog, 0x0, sizeof(szLog));
	sprintf(szLog, "最大线程数%d,允许最大空闲线程%d,最小空闲线程数%d", nMaxThreads, nMaxIdleThreads, nMinIdleThreads);
	writelog(szLog);
	m_nMaxThreads = nMaxThreads;
	m_nMaxIdleThreads = nMaxIdleThreads;
	m_nMinIdleThreads = nMinIdleThreads;
	m_hmutex = CreateMutex(NULL, false, NULL);
	m_bStop = true;
}

// 向线程池添加任务
void CThreadPool::push_task(PTask pTask)
{
	m_cTaskQueue.push_task(pTask);
}

// 从线程池取任务
PTask CThreadPool::pop_task()
{
	return m_cTaskQueue.pop_task();
}


void CThreadPool::wait_task()
{
	m_cTaskQueue.wait_task();
}

void CThreadPool::notify_task()
{
	m_cTaskQueue.notify_task();
}

// 创建工作线程
void CThreadPool::create_wroker(int nWorks)
{
	// 如果要创建的线程数大于
	if (nWorks>m_nMaxThreads)
	{
		nWorks = m_nMaxThreads;
	}
	CWorkThread* pWork = NULL;
	for (int i = 0; i<nWorks; i++)
	{
		pWork = new CWorkThread(this);
		m_ltWorkList.push_worker(pWork);
		m_ltIdlList.push_worker(pWork);
		m_nTotalThread++;
	}
}

void CThreadPool::run_task(PTask pTask)
{
	char szLog[128];
	memset(szLog, 0x0, sizeof(szLog));
	sprintf(szLog, "当前线程数%d,忙碌线程%d", m_nTotalThread, m_nBusiThreads);
	writelog(szLog);
	if (m_nTotalThread == 0)
	{
		throw CThreadExcepiton("the threadpool is not start!", "", -1);
	}
	if (pTask)
	{
		// 当前线程都在忙碌,需要创建新线程来处理任务
		CMutex mutex(m_hmutex);
		mutex.Lock();
		if (m_nBusiThreads == m_nTotalThread)
		{
			writelog("当前线程都在忙碌,需要创建新线程来执行任务");
			if (m_nTotalThread < m_nMaxThreads)
			{
				// 现在还可以创建的线程
				int nAllow = m_nMaxThreads - m_nTotalThread;
				// 取最小空闲线程和允许创建线程的最小值
				int nWorks = m_nMinIdleThreads < nAllow ? m_nMinIdleThreads:nAllow;
				memset(szLog, 0x0, sizeof(szLog));
				sprintf(szLog, "重新创建%d个线程", nWorks);
				writelog(szLog);
				printf("启动%d个线程\r\n",nWorks);
				create_wroker(nWorks);
			}else // 已经达到最大线程限制,等待有空闲线程可用
			{
				printf("当前没有可用线程,等待可用线程\r\n");
				memset(szLog, 0x0, sizeof(szLog));
				writelog("达到最大线程,等待可用线程...");
				mutex.Unlock();
				m_ltIdlList.wait();
				mutex.Lock();
				writelog("等到可用线程了!!!");
			}
		}
		// 开始处理任务
		CWorkThread* pWork = m_ltIdlList.pop_worker();
		if (pWork)
		{
			printf("分配任务给线程\r\n");
			pWork->set_task(pTask);
			m_nBusiThreads++;
		}
		mutex.Unlock();

	}
}

// 回收空闲线程
void CThreadPool::recover_idlThread()
{
	char szLog[128];
	CMutex mutex(m_hmutex);
	// 当前空闲线程
	mutex.Lock();
	int nTotalFree = m_nTotalThread - m_nBusiThreads;
	// 当前空闲线程小于允许的最大空闲线程不回收
	if (nTotalFree < m_nMaxIdleThreads)
	{
		mutex.Unlock();
		return;
	}
	// 需要回收的线程
	int nNeedRecvory = nTotalFree - m_nMaxIdleThreads;
	memset(szLog, 0x0,sizeof(szLog));
	sprintf(szLog, "当前线程数:%d,当前空闲线程线程数:%d, 需要回收的线程数:%d", m_nTotalThread, nTotalFree, nNeedRecvory);
	writelog(szLog);
	CWorkThread* pWork = NULL;
	for (int i = 0; i<nNeedRecvory; i++)
	{
		pWork = m_ltIdlList.pop_worker();
		if (pWork)
		{
			pWork->stop(); // 停止线程
			pWork->join();
			m_ltWorkList.erase_worker(pWork);
			delete pWork;
			m_nTotalThread--;
		}
	}
	mutex.Unlock();
}
// 线程池开始工作
void CThreadPool::start()
{
	char szLog[128];
	memset(szLog, 0x0, sizeof(szLog));
	sprintf(szLog, "CThreadPool::start() %d个线程", m_nMinIdleThreads);
	writelog(szLog);
	m_nTotalThread = m_nBusiThreads = 0;
	create_wroker(m_nMinIdleThreads);
	// 创建任务调度线程
	m_pQueueHandler = new CQueueHandler(this);
	// 创建回收线程
	m_pRecoverHandler = new CRecoverHandler(this);
	m_bStop = false;
}

void CThreadPool::idl_toPool(CWorkThread* pWork)
{
	CMutex mutex(m_hmutex);
	mutex.Lock();
	m_ltIdlList.push_worker(pWork);
	printf("线程%d处理任务完成\r\n",pWork->get_thread()->get_threadId());
	char szLog[128];
	memset(szLog, 0x0, sizeof(szLog));
	sprintf(szLog, "线程%d处理任务完成\r\n",pWork->get_thread()->get_threadId());
	writelog(szLog);
	m_nBusiThreads--;
	mutex.Unlock();
}

void CThreadPool::stop()
{
	if (!m_bStop)
	{
		m_pQueueHandler->stop();
		m_pRecoverHandler->stop();
		if (m_pQueueHandler)
		{
			delete m_pQueueHandler;
		}
		if (m_pRecoverHandler)
		{
			delete m_pRecoverHandler;
		}
		m_pQueueHandler = NULL;
		m_pRecoverHandler = NULL;
		while(true)
		{
			CMutex mutex(m_hmutex);
			CWorkThread* pWork = m_ltWorkList.pop_worker();
			if (pWork == NULL)
			{
				break;
			}
			pWork->stop(); // 通知线程结束
			delete pWork;
		}
		m_ltIdlList.clear();
		m_nBusiThreads = m_nTotalThread = 0;
		PTask pTask = NULL;
		while (true)
		{
			pTask = m_cTaskQueue.pop_task();
			if (pTask)
			{
				pTask->TimeOut();
				delete pTask;
			}else
			{
				break;
			}
		}
	}
}

