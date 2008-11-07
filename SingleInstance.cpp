#include "StdAfx.h"
#include "singleinstance.h"

CSingleInstance::CSingleInstance(LPCTSTR applicationName, bool bIsOwner)
{
	m_bIsLoaded = false;

	if (bIsOwner) {
		SetLastError(0);
		m_hMutex = CreateMutex(NULL, TRUE, applicationName);
		if (m_hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
			m_bIsLoaded = true;
	} else {
		m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, applicationName);
		if (m_hMutex)
			m_bIsLoaded = true;
	}
}

CSingleInstance::~CSingleInstance(void)
{
	if (m_hMutex) {
		CloseHandle(m_hMutex);
	}
}

bool CSingleInstance::IsAlreadyLoaded(void)
{
	return m_bIsLoaded;
}

