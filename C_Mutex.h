// C_Mutex.h
#ifndef C_MUTEX_H_H
#define C_MUTEX_H_H 
#include "stdafx.h"
//------------------------------------------------------------------------
// 类 　: CMutex
// 用途 : 利用C++局部变量和构造析构函数特性实现自动上锁和解锁的互斥访问对象,
// 用于多个线程对共享资源的互斥访问
// 用法 : Mutex的创建和关闭由调用者实现,使用是将创建的Mutex传递给此对象
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
