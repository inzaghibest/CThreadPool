// C_Thread_Exception.h
/* 定义线程异常类 */
#ifndef C_THREAD_EXCEPTION_H_H
#define C_THREAD_EXCEPTION_H_H
#include "C_Error.h"
//------------------------------------------------------------------------
// 类 　: CThreadException
// 用途 : 定义线程处理的异常类
//------------------------------------------------------------------------
class CThreadExcepiton:public CErrorMsg
{
public:
	CThreadExcepiton(string strErrorMsg = "", string strHint = "", int nErrorCode = 0):CErrorMsg(strErrorMsg, nErrorCode, strHint)
	{

	}
};
#endif // C_THREAD_EXCEPTION_H_H