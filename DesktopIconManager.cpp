// Desktop Item Manager
#include "stdafx.h"
#include "DesktopIconManager.h"
#include <commctrl.h>

CDesktopIconManager::CDesktopIconManager()
: m_hProcess(NULL)
, m_hwnd(NULL)
, m_lpMem(NULL)
{
    // Find the icon container window
    m_hwnd = GetTopWindow(GetTopWindow(FindWindow(_T("Progman"), NULL)));
    if (m_hwnd == NULL) return;

    // Get shell process id
    DWORD dwPid;
    GetWindowThreadProcessId(m_hwnd, &dwPid);

    // Open shell process
    m_hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwPid);
    if (m_hProcess == NULL) {
        m_hwnd = NULL;
        return;
    }

    // Allocate one page in shell's address space
    m_lpMem = VirtualAllocEx(m_hProcess, NULL, 4096, MEM_COMMIT, PAGE_READWRITE);
    if (m_lpMem == NULL) {
        CloseHandle(m_hProcess);
        m_hProcess = NULL;
        m_hwnd = NULL;
    }
}

CDesktopIconManager::~CDesktopIconManager()
{
    // Deallocate memory
    if (m_lpMem) {
        VirtualFreeEx(m_hProcess, m_lpMem, 0, MEM_RELEASE);
    }

    // Close process handle
    if (m_hProcess) {
        CloseHandle(m_hProcess);
    }
}

int CDesktopIconManager::GetNumIcons(void) const
{
    if (m_hwnd) {
        return(static_cast<int>(SendMessage(m_hwnd, LVM_GETITEMCOUNT, 0, 0)));
    }
    return(0);
}

bool CDesktopIconManager::GetIconText(int nIndex, LPTSTR pszText, int cchText) const
{
    if (m_hwnd && m_hProcess && m_lpMem) {
        // Make sure we don't overflow
        if (cchText < 0 || cchText > (4096 / sizeof(TCHAR)))
            cchText = 4096 / sizeof(TCHAR);

        // Create a listview item
        LVITEM item;
        ZeroMemory(&item, sizeof(LVITEM));
        item.iSubItem = 0;
        item.cchTextMax = cchText;
        item.pszText = reinterpret_cast<LPTSTR>(m_lpMem) + sizeof(LVITEM);

        // Copy it to shell process's memory
        DWORD dwNumWritten;
        WriteProcessMemory(m_hProcess, m_lpMem, &item, sizeof(LVITEM), &dwNumWritten);
        if (dwNumWritten != sizeof(LVITEM)) return(false);

        // Get item text
        SendMessage(m_hwnd, LVM_GETITEMTEXT, nIndex, reinterpret_cast<LPARAM>(m_lpMem));

        // Read the process's memory into our buffer
        DWORD dwNumRead;
        ReadProcessMemory(m_hProcess, item.pszText, pszText, cchText * sizeof(TCHAR), &dwNumRead);
        if (dwNumRead != cchText * sizeof(TCHAR)) return(false);

        return(true);
    }
    return(false);
}

bool CDesktopIconManager::GetIconPosition(int nIndex, LPPOINT ppt) const
{
    if (m_hwnd && m_hProcess && m_lpMem) {
        // Get item position
        SendMessage(m_hwnd, LVM_GETITEMPOSITION, nIndex, reinterpret_cast<LPARAM>(m_lpMem));

        // Read the process's memory into our buffer
        DWORD dwNumRead;
        ReadProcessMemory(m_hProcess, m_lpMem, ppt, sizeof(POINT), &dwNumRead);
        if (dwNumRead != sizeof(POINT)) return(false);

        return(true);
    }
    return(false);
}

bool CDesktopIconManager::SetIconPosition(int nIndex, LPPOINT ppt)
{
    if (m_hwnd && m_hProcess && m_lpMem) {
        // Write position to process's memory
        DWORD dwNumWritten;
        WriteProcessMemory(m_hProcess, m_lpMem, ppt, sizeof(POINT), &dwNumWritten);
        if (dwNumWritten != sizeof(POINT)) return(false);

        // Set item position
        SendMessage(m_hwnd, LVM_SETITEMPOSITION32, nIndex, reinterpret_cast<LPARAM>(m_lpMem));

        return(true);
    }
    return(false);
}
