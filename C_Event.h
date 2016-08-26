// C_Event.h
#ifndef C_EVENT_H_H
#define C_EVENT_H_H
#include "stdafx.h"
#include "C_Error.h"

//------------------------------------------------------------------------
// �� ��: CEvnet
// ��; : ʵ���¼�����ķ�װ,�����߳�ͬ��ʹ��
// �÷� : Ĭ������һ���˹��������źŵ��¼�����,����Ҫ�̵߳ȴ�ʱ,����wait()����,
// ��֪ͨ�߳�ִ��ʱ,����notify()������
//------------------------------------------------------------------------
class CEvent
{
public:
	enum EventType
	{
		EVENT_SINGLE = 1, // �Զ�����, �Զ����õ��¼�������һ���ȴ����ɹ����߳��ͷź���Զ���Ϊ���ź�״̬��
		EVENT_BORODCAST = 2 // �˹�����,��һ���̵߳ȴ����¼������,��Ҫ�ֹ�����Ϊ���ź�״̬.
	};
private:
	EventType m_eType; // �¼���������
	HANDLE m_hEvent; // �¼�������
public:
	explicit CEvent(EventType eType = EVENT_SINGLE);
	void reset();
	void wait();
	int wait(int nSecs);
	void notify();
	~CEvent();
};
#endif //C_EVENT_H_H
