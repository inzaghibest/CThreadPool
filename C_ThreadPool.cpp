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
	sprintf(szLog, "CWorkThread() ��%d��", m_nThreadCount);
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
		sprintf(szLog, "~CWorkThread() ��%d", m_nThreadCount);
		writelog(szLog);
	}
}

void CWorkThread::set_task(CTask* pTask)
{
	m_pTask = pTask;
	m_pTask->set_threadId(get_thread()->get_threadId());
	// ���������,��ʼִ��
	m_event.notify();
}

// �˷����ڹ����̴߳����󣬾ͱ�����,��ʼ�ȴ�״̬��ֱ����������
void CWorkThread::run()
{
	while (true)
	{
		m_event.wait();
		// stop�жϵ�λ�ú���Ҫ,��������,�������߳�֪ͨʱֱ���˳�,�������±ߵ��̳߳ؿ���֪ͨ,
		// �����ڽ����̷߳����̳߳ؿ����߳��б��С�
		if (m_bStop)
		{
			break;
		}
		m_pTask->Run();
		// ����ִ�����,����
		if (m_pTask)
		{
			delete m_pTask;
		}
		writelog("CWorkThread::Task end");
		// ֪ͨ�̳߳ؿ���
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
	writelog("CQueueHandler ��������߳̿���...");
	m_bStop = false;
	m_pThread->start();
	m_pThread->notify();
}

CQueueHandler::~CQueueHandler()
{
	if (m_pThread)
	{
		delete m_pThread;
		writelog("CQueueHandler ��������߳̽���...");
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
		// ���ϴ��������ȡ������ȥִ��
		PTask pTask = m_pThreadPool->pop_task();
		if (pTask) // ����������ִ��
		{
			m_pThreadPool->run_task(pTask);
		}else // û������,�ȴ��������֪ͨ
		{
			m_pThreadPool->wait_task();
		}
	}
	writelog("CQueueHandler end...");
}

void CQueueHandler::stop()
{
	m_bStop = true;
	// ֪ͨ,��ֹ���ڵȴ���ȡ����
	m_pThreadPool->notify_task();
}

// CRecoverHandler
CRecoverHandler::CRecoverHandler(CThreadPool* pThreadPool)
{
	m_bStop = false;
	m_pThreadPool = pThreadPool;
	m_pThread = new CThread(this);
	writelog("CRecoverHandler ��ʱ�����߳�����...");
	m_pThread->start();
	m_pThread->notify();
}

CRecoverHandler::~CRecoverHandler()
{
	if (m_pThread)
	{
		delete m_pThread;
		writelog("CRecoverHandler ��ʱ�����߳̽���...");
	}
	m_pThread = NULL;
}
void CRecoverHandler::run()
{
	writelog("��ʱ�����߳�Run Begin...");
	while (true)
	{
		wait(RECOVER_TIME); 
		if (m_bStop)
		{
			break;
		}
		m_pThreadPool->recover_idlThread();
	}
	writelog("��ʱ�����߳�Run End...");
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
		notify(); // ֪ͨ��һ�������߳�
		printf("%d�߳̿���\r\n", pWork->get_thread()->get_threadId());
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
	sprintf(szLog, "����߳���%d,�����������߳�%d,��С�����߳���%d", nMaxThreads, nMaxIdleThreads, nMinIdleThreads);
	writelog(szLog);
	m_nMaxThreads = nMaxThreads;
	m_nMaxIdleThreads = nMaxIdleThreads;
	m_nMinIdleThreads = nMinIdleThreads;
	m_hmutex = CreateMutex(NULL, false, NULL);
	m_bStop = true;
}

// ���̳߳��������
void CThreadPool::push_task(PTask pTask)
{
	m_cTaskQueue.push_task(pTask);
}

// ���̳߳�ȡ����
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

// ���������߳�
void CThreadPool::create_wroker(int nWorks)
{
	// ���Ҫ�������߳�������
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
	sprintf(szLog, "��ǰ�߳���%d,æµ�߳�%d", m_nTotalThread, m_nBusiThreads);
	writelog(szLog);
	if (m_nTotalThread == 0)
	{
		throw CThreadExcepiton("the threadpool is not start!", "", -1);
	}
	if (pTask)
	{
		// ��ǰ�̶߳���æµ,��Ҫ�������߳�����������
		CMutex mutex(m_hmutex);
		mutex.Lock();
		if (m_nBusiThreads == m_nTotalThread)
		{
			writelog("��ǰ�̶߳���æµ,��Ҫ�������߳���ִ������");
			if (m_nTotalThread < m_nMaxThreads)
			{
				// ���ڻ����Դ������߳�
				int nAllow = m_nMaxThreads - m_nTotalThread;
				// ȡ��С�����̺߳��������̵߳���Сֵ
				int nWorks = m_nMinIdleThreads < nAllow ? m_nMinIdleThreads:nAllow;
				memset(szLog, 0x0, sizeof(szLog));
				sprintf(szLog, "���´���%d���߳�", nWorks);
				writelog(szLog);
				printf("����%d���߳�\r\n",nWorks);
				create_wroker(nWorks);
			}else // �Ѿ��ﵽ����߳�����,�ȴ��п����߳̿���
			{
				printf("��ǰû�п����߳�,�ȴ������߳�\r\n");
				memset(szLog, 0x0, sizeof(szLog));
				writelog("�ﵽ����߳�,�ȴ������߳�...");
				mutex.Unlock();
				m_ltIdlList.wait();
				mutex.Lock();
				writelog("�ȵ������߳���!!!");
			}
		}
		// ��ʼ��������
		CWorkThread* pWork = m_ltIdlList.pop_worker();
		if (pWork)
		{
			printf("����������߳�\r\n");
			pWork->set_task(pTask);
			m_nBusiThreads++;
		}
		mutex.Unlock();

	}
}

// ���տ����߳�
void CThreadPool::recover_idlThread()
{
	char szLog[128];
	CMutex mutex(m_hmutex);
	// ��ǰ�����߳�
	mutex.Lock();
	int nTotalFree = m_nTotalThread - m_nBusiThreads;
	// ��ǰ�����߳�С��������������̲߳�����
	if (nTotalFree < m_nMaxIdleThreads)
	{
		mutex.Unlock();
		return;
	}
	// ��Ҫ���յ��߳�
	int nNeedRecvory = nTotalFree - m_nMaxIdleThreads;
	memset(szLog, 0x0,sizeof(szLog));
	sprintf(szLog, "��ǰ�߳���:%d,��ǰ�����߳��߳���:%d, ��Ҫ���յ��߳���:%d", m_nTotalThread, nTotalFree, nNeedRecvory);
	writelog(szLog);
	CWorkThread* pWork = NULL;
	for (int i = 0; i<nNeedRecvory; i++)
	{
		pWork = m_ltIdlList.pop_worker();
		if (pWork)
		{
			pWork->stop(); // ֹͣ�߳�
			pWork->join();
			m_ltWorkList.erase_worker(pWork);
			delete pWork;
			m_nTotalThread--;
		}
	}
	mutex.Unlock();
}
// �̳߳ؿ�ʼ����
void CThreadPool::start()
{
	char szLog[128];
	memset(szLog, 0x0, sizeof(szLog));
	sprintf(szLog, "CThreadPool::start() %d���߳�", m_nMinIdleThreads);
	writelog(szLog);
	m_nTotalThread = m_nBusiThreads = 0;
	create_wroker(m_nMinIdleThreads);
	// ������������߳�
	m_pQueueHandler = new CQueueHandler(this);
	// ���������߳�
	m_pRecoverHandler = new CRecoverHandler(this);
	m_bStop = false;
}

void CThreadPool::idl_toPool(CWorkThread* pWork)
{
	CMutex mutex(m_hmutex);
	mutex.Lock();
	m_ltIdlList.push_worker(pWork);
	printf("�߳�%d�����������\r\n",pWork->get_thread()->get_threadId());
	char szLog[128];
	memset(szLog, 0x0, sizeof(szLog));
	sprintf(szLog, "�߳�%d�����������\r\n",pWork->get_thread()->get_threadId());
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
			pWork->stop(); // ֪ͨ�߳̽���
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

