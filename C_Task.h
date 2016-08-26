// C_Task.h
//------------------------------------------------------------------------
// �� ��: CTask������� CTaskQueue�������ȶ���
// ��; : ʵ��������Ĺ���
//------------------------------------------------------------------------
#ifndef C_TASK_H_H
#define C_TASK_H_H
#include <queue>
#include "C_Mutex.h"
#include "C_Event.h"
using namespace std;

//------------------------------------------------------------------------
// �� ��: CTask
// ��; : ����Ļ���,���ݸ������̱߳����Ǵ˻��������,��������������߳��и���
// ������Ҫ����ʵ��ִ�������run����
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
		printf("û�еõ������߳�,��ʱ...\r\n");
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
	int m_nThreadId; // ִ�д�������߳�id
	int m_nPriority; // �������ȼ�,���ȼ�Խ��Խ��ִ��
};

/* �������� */
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
// �� ��: CTaskQueue
// ��; : ʵ����������ȶ��й���
//------------------------------------------------------------------------
class CTaskQueue
{
private:
	HANDLE m_hMutex; // �����̻߳�������������
	CEvent m_event; // �����������������ʱ֪ͨ�¼�
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
			// ��ȡͷԪ��
			pTask = dynamic_cast<PTask>(m_cTaskQueue.top());
			// ���ͷԪ��
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
		m_event.notify(); // �õ�������֪ͨ
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

	// ֪ͨ��������
	void notify_task()
	{
		m_event.notify();
	}
	// �ȴ�������
	void wait_task()
	{
		m_event.wait();
	}


};

#endif // C_TASK_H_H