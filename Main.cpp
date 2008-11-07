// IconJesus by DEATH in 2003
#include "StdAfx.h"
#include "nicon/NotificationIcon.h"
#include "Xml/Xml.h"
#include "DesktopIconManager.h"
#include "Resource.h"
#include "SingleInstance.h"

using namespace XML;

#define WM_NOTIFYICON (WM_USER + 1)

const int cIdIconJesus = 1;

const int cIdSaveIcons = 0x100;
const int cIdRestoreIcons = 0x101;
const int cIdExit = 0x102;

CNotificationIcon *g_pni;
CDesktopIconManager g_dim;
HICON g_hIcon;
HINSTANCE g_hInstance;
HMENU g_hMenu;

ATOM RegisterIconJesusClass(void);
LRESULT CALLBACK IconJesusWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND CreateIconJesusWindow(void);
void SaveIconPositions(void);
void RestoreIconPositions(void);
void RestoreIconPosition(const char *pszName, int x, int y);
void PlayHallelujah(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    g_hInstance = hInstance;

	CSingleInstance *psi = new CSingleInstance("IconJesus");
	if (psi->IsAlreadyLoaded() == true) {
		MessageBox(NULL, "IconJesus is already loaded!", NULL, MB_OK | MB_ICONERROR);
		delete psi;
		return 0;
	}
	delete psi;

    if (RegisterIconJesusClass() == 0)
        return(0);

    HWND hwndIconJesus = CreateIconJesusWindow();
    if (hwndIconJesus == NULL)
        return(0);

    MSG msg;
    BOOL bRet;
    while (bRet = GetMessage(&msg, hwndIconJesus, 0, 0)) {
        if (bRet == -1)
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return(0);
}

// Register IconJesus window class
ATOM RegisterIconJesusClass(void)
{
    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(WNDCLASS));
    wc.hInstance = g_hInstance;
    wc.lpfnWndProc = IconJesusWndProc;
    wc.lpszClassName = "IconJesus";
    wc.style = CS_HREDRAW | CS_VREDRAW;
    return(RegisterClass(&wc));
}

// Create an IconJesus window
HWND CreateIconJesusWindow(void)
{
    return(CreateWindow("IconJesus", NULL, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, g_hInstance, 0));
}

// IconJesus window
LRESULT CALLBACK IconJesusWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CSingleInstance *psi = 0;

	switch (uMsg) {
        case WM_CREATE:
            // Create window
            g_pni = new CNotificationIcon(hwnd, cIdIconJesus);
            g_hIcon = reinterpret_cast<HICON>(LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
            g_pni->SetIcon(g_hIcon);
            g_pni->SetTip("IconJesus");
            g_pni->SetCallbackMessage(WM_NOTIFYICON);
            g_pni->Add();

            g_hMenu = CreatePopupMenu();
            if (g_hMenu) {
                AppendMenu(g_hMenu, MF_ENABLED | MF_DEFAULT, cIdRestoreIcons, "Restore icon positions");
                AppendMenu(g_hMenu, MF_ENABLED, cIdSaveIcons, "Save icon positions");
                AppendMenu(g_hMenu, MF_SEPARATOR, -1, NULL);
                AppendMenu(g_hMenu, MF_ENABLED, cIdExit, "Exit");
            }

			psi = new CSingleInstance("IconJesus", true);
            return(0);
        case WM_COMMAND:
            // Perform action
            switch (LOWORD(wParam)) {
                case cIdExit:
                    // Exit application
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
                case cIdSaveIcons:
                    // Save icon positions
                    SaveIconPositions();
                    break;
                case cIdRestoreIcons:
                    // Restore icon positions
					PlayHallelujah();
                    RestoreIconPositions();
                    break;
                default:
                    break;
            }
            break;
        case WM_NOTIFYICON:
            // Notification icon message
            if (wParam == cIdIconJesus) {
                switch (lParam) {
                    case WM_RBUTTONDOWN:
                        // Right mouse button down
                        if (g_hMenu) {
                            POINT pt;
                            GetCursorPos(&pt);
                            SetForegroundWindow(hwnd);
                            TrackPopupMenu(g_hMenu, 0, pt.x, pt.y, 0, hwnd, NULL);
                        }
                        break;
                    case WM_LBUTTONDOWN:
						PlayHallelujah();
                        RestoreIconPositions();
                        break;
                    default:
                        break;
                }
                return(0);
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return(0);
        case WM_DESTROY:
            // Destroy window
            if (g_pni) {
                g_pni->Remove();
                delete g_pni;
                g_pni = NULL;
            }
            if (g_hMenu) {
                DestroyMenu(g_hMenu);
                g_hMenu = NULL;
            }
            if (g_hIcon) {
                DestroyIcon(g_hIcon);
            }
			if (psi) {
				delete psi;
			}
            PostQuitMessage(0);
            return(0);
        default:
            break;
    }

    return(DefWindowProc(hwnd, uMsg, wParam, lParam));
}

// Save icon positions
void SaveIconPositions(void)
{
    POINT pt;
    char szName[128];
    bool bSuccess;

    CXML xml("IconJesus");
    CXML *icons = xml.CreateChild("Icons");
    
    int nIcons = g_dim.GetNumIcons();
    for (int i = 0; i < nIcons; i++) {
        bSuccess = g_dim.GetIconPosition(i, &pt);
        bSuccess &= g_dim.GetIconText(i, szName, 128);
        if (bSuccess) {
            CXML *icon = icons->CreateChild("Icon");
            icon->SetParam("Name", szName);
            icon->SetParam("X", pt.x);
            icon->SetParam("Y", pt.y);
        }
    }

    xml.SaveAs("IconJesus.xml");
}

// Restore icon positions
void RestoreIconPositions(void)
{
    CXML xml;
    if (xml.Load("IconJesus.xml", "IconJesus") == TRUE) {
        CXML *icons = xml.GetFirstChild();
        while (icons) {
            if (lstrcmp(icons->GetTagName(), "Icons") == 0) {
                CXML *icon = icons->GetFirstChild();
                while (icon) {
                    if (lstrcmp(icon->GetTagName(), "Icon") == 0) {
                        POINT pt;
                        pt.x = icon->GetParamInt("X", -1);
                        pt.y = icon->GetParamInt("Y", -1);
                        LPCSTR pszName = icon->GetParamText("Name");
                        if (pszName[0] && pt.x != -1 && pt.y != -1) {
                            RestoreIconPosition(pszName, pt.x, pt.y);
                        }
                    }
                    icon = icons->GetNextChild();
                }
            }
            icons = xml.GetNextChild();
        }
    }
}

// Restore an icon's position
void RestoreIconPosition(const char *pszName, int x, int y)
{
    int nIcons = g_dim.GetNumIcons();
    char szName[128];

    for (int i = 0; i < nIcons; i++) {
        if (g_dim.GetIconText(i, szName, 128) == true) {
            if (lstrcmpi(szName, pszName) == 0) {
                POINT pt;
                pt.x = x;
                pt.y = y;
                g_dim.SetIconPosition(i, &pt);
                break;
            }
        }
    }
}

void PlayHallelujah(void)
{
	//PlaySound(MAKEINTRESOURCE(IDW_HALLELUJAH), g_hInstance, SND_ASYNC | SND_RESOURCE);
}
