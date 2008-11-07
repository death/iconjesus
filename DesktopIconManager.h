#pragma once

class CDesktopIconManager
{
public:
    CDesktopIconManager();
    ~CDesktopIconManager();

    int GetNumIcons(void) const;
    
    bool GetIconText(int nIndex, LPTSTR pszText, int cchText) const;

    bool GetIconPosition(int nIndex, LPPOINT ppt) const;
    bool SetIconPosition(int nIndex, LPPOINT ppt);

private:
    HWND m_hwnd;
    HANDLE m_hProcess;
    LPVOID m_lpMem;
};