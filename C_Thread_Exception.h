// C_Thread_Exception.h
/* �����߳��쳣�� */
#ifndef C_THREAD_EXCEPTION_H_H
#define C_THREAD_EXCEPTION_H_H
#include "C_Error.h"
//------------------------------------------------------------------------
// �� ��: CThreadException
// ��; : �����̴߳�����쳣��
//------------------------------------------------------------------------
class CThreadExcepiton:public CErrorMsg
{
public:
	CThreadExcepiton(string strErrorMsg = "", string strHint = "", int nErrorCode = 0):CErrorMsg(strErrorMsg, nErrorCode, strHint)
	{

	}
};
#endif // C_THREAD_EXCEPTION_H_H