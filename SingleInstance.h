#pragma once
#include <windows.h>

class CSingleInstance
{
public:
	CSingleInstance(LPCTSTR applicationName, bool bIsOwner = false);
	~CSingleInstance(void);
	bool IsAlreadyLoaded(void);

private:
	bool m_bIsLoaded;
	HANDLE m_hMutex;
};
