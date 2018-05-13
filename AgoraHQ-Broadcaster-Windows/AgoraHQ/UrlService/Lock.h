#pragma once
#include <synchapi.h>
class CLock
{
public:

	CLock()
	{
		::InitializeCriticalSection(&m_critSec);
	}
	virtual ~CLock()
	{
		::DeleteCriticalSection(&m_critSec);
	}

	void Lock()
	{
		::EnterCriticalSection(&m_critSec);
	}
	void UnLock()
	{
		::LeaveCriticalSection(&m_critSec);
	}

private:
	CRITICAL_SECTION	m_critSec;
};