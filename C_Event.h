// C_Event.h
#ifndef C_EVENT_H_H
#define C_EVENT_H_H
#include "stdafx.h"
#include "C_Error.h"

//------------------------------------------------------------------------
// 类 　: CEvnet
// 用途 : 实现事件对象的封装,用于线程同步使用
// 用法 : 默认生成一个人工重置无信号的事件对象,当需要线程等待时,调用wait()方法,
// 当通知线程执行时,调用notify()方法。
//------------------------------------------------------------------------
class CEvent
{
public:
	enum EventType
	{
		EVENT_SINGLE = 1, // 自动重置, 自动重置的事件对象在一个等待它成功的线程释放后会自动变为无信号状态。
		EVENT_BORODCAST = 2 // 人工重置,当一个线程等待到事件对象后,需要手工重置为无信号状态.
	};
private:
	EventType m_eType; // 事件对象类型
	HANDLE m_hEvent; // 事件对象句柄
public:
	explicit CEvent(EventType eType = EVENT_SINGLE);
	void reset();
	void wait();
	int wait(int nSecs);
	void notify();
	~CEvent();
};
#endif //C_EVENT_H_H
