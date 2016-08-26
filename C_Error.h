// C_Error.h
#ifndef C_ERROR_H_H
#define C_ERROR_H_H
#include <string>
using namespace std;
//------------------------------------------------------------------------
// �� ��: CErrorMsg
// ��; : �����쳣�Ļ���,����ͨ���̳���ɸ��������쳣��ʵ��
//------------------------------------------------------------------------
class CErrorMsg
{
public:
	CErrorMsg(string strErrorMsg = "", int nErrorCode = 0, string strHint=""):m_strErrorMsg(strErrorMsg),
		m_strHint(strHint),m_nErrorCode(nErrorCode)
	{

	}
	~CErrorMsg(){}
	int get_errorCode(){return m_nErrorCode;}
	string get_errorMsg(){return m_strErrorMsg;}
	string get_errorHint(){return m_strHint;}
	void set_errorCode(int nErrorCode){m_nErrorCode = nErrorCode;}
	void set_errorMsg(string strMsg){m_strErrorMsg = strMsg;}
	void set_errorHint(string strHint){m_strHint = strHint;}
protected:
private:
	string m_strErrorMsg;
	int m_nErrorCode;
	string m_strHint;

};
#endif // C_ERROR_H_H