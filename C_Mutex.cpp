// C_Mutex.cpp
#include "C_Mutex.h"

CMutex::CMutex(HANDLE hMutex)
{
	if (hMutex)
	{
		m_hMutex = hMutex;
	}
	//::WaitForSingleObject(m_hMutex, INFINITE);
}

void CMutex::Lock()
{
	::WaitForSingleObject(m_hMutex, INFINITE);
}
void CMutex::Unlock()
{
	if (m_hMutex)
	{
		::ReleaseMutex(m_hMutex);
	}
}
CMutex::~CMutex()
{
}