// C_Mutex.h
#ifndef C_MUTEX_H_H
#define C_MUTEX_H_H 
#include "stdafx.h"
//------------------------------------------------------------------------
// �� ��: CMutex
// ��; : ����C++�ֲ������͹���������������ʵ���Զ������ͽ����Ļ�����ʶ���,
// ���ڶ���̶߳Թ�����Դ�Ļ������
// �÷� : Mutex�Ĵ����͹ر��ɵ�����ʵ��,ʹ���ǽ�������Mutex���ݸ��˶���
//------------------------------------------------------------------------
class CMutex
{
private:
	HANDLE m_hMutex;
public:
	CMutex(HANDLE hMutex);
	void Lock();
	void Unlock();
	~CMutex();
};

#endif // C_MUTEX_H_H
