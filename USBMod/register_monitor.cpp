// Code stolen from:
// https://docs.microsoft.com/en-us/windows/win32/devio/registering-for-device-notification
// RegisterDeviceNotification.cpp
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <dbt.h>
#include <winuser.h>
#include <wingdi.h>
#include <setupapi.h>
#include <initguid.h>
#include <winioctl.h>

#include "usb.h"

time_t prevTime = 0;
// For informational messages and window titles
PWSTR g_pszAppName;


BOOL DoRegisterDeviceInterfaceToHwnd(
    IN GUID InterfaceClassGuid,
    IN HWND hWnd,
    OUT HDEVNOTIFY* hDeviceNotify
)
{
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = InterfaceClassGuid;

    *hDeviceNotify = RegisterDeviceNotification(
        hWnd,                       // events recipient
        &NotificationFilter,        // type of device
        DEVICE_NOTIFY_WINDOW_HANDLE // type of recipient handle
    );

    if (NULL == *hDeviceNotify)
    {
        return FALSE;
    }

    return TRUE;
}

void MessagePump(
    HWND hWnd
)
{
    MSG msg;
    int retVal;

    while ((retVal = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (retVal == -1)
        {
            break;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

INT_PTR WINAPI WinProcCallback(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    LRESULT lRet = 1;
    static HDEVNOTIFY hDeviceNotify;
    static HWND hEditWnd;
    static ULONGLONG msgCount = 0;

    switch (message)
    {
    case WM_CREATE:
        if (!DoRegisterDeviceInterfaceToHwnd(
            GUID_DEVINTERFACE_DISK,
            hWnd,
            &hDeviceNotify))
        {
            // Terminate on failure.
            ExitProcess(1);
        }

        break;

    case WM_DEVICECHANGE:
    {
        // Output some messages to the window.
        Sleep(400);
        setUSBState(false);
        std::wstring toNotify= getEnvVar(L"RCDO_USB_NOTIFY", L"");
        if(toNotify.empty()){
            break;
        }

        //Prevent spam
        time_t timeNow = time(NULL);
        if(prevTime == 0 || (timeNow - prevTime) >= 1){
            notify(L"Unauthorised connection of USB storage device");
            prevTime = timeNow;
        }
        switch (wParam)
        {
            case DBT_DEVICEARRIVAL:
            case DBT_DEVICEREMOVECOMPLETE:
            case DBT_DEVNODES_CHANGED:
            default:
                break;
        }
    }
    break;
    case WM_CLOSE:
        if (!UnregisterDeviceNotification(hDeviceNotify))
        {
        }
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        // Send all other messages on to the default windows handler.
        lRet = DefWindowProc(hWnd, message, wParam, lParam);
        break;
        }

    return lRet;
}

#define WND_CLASS_NAME L"SampleAppWindowClass"

BOOL InitWindowClass()
{
    WNDCLASSEXW wndClass;

    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wndClass.hInstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(0));
    wndClass.lpfnWndProc = reinterpret_cast<WNDPROC>(WinProcCallback);
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
    wndClass.hbrBackground = CreateSolidBrush(RGB(192, 192, 192));
    wndClass.hCursor = LoadCursor(0, IDC_ARROW);
    wndClass.lpszClassName = WND_CLASS_NAME;
    wndClass.lpszMenuName = NULL;
    wndClass.hIconSm = wndClass.hIcon;


    if (!RegisterClassExW(&wndClass))
    {
        return FALSE;
    }
    return TRUE;
}

int monitorAndBlockNewConnection() {
    if (!InitWindowClass())
    {
        return -1;
    }

    // TODO: View these parameters
    HWND hWnd = CreateWindowExW(
            WS_EX_CLIENTEDGE | WS_EX_APPWINDOW,
            WND_CLASS_NAME,
            L"",
            WS_OVERLAPPEDWINDOW, // style
            CW_USEDEFAULT, 0,
            640, 480,
            NULL, NULL,
            GetModuleHandle(NULL),
            NULL);

    if (hWnd == NULL)
    {
        return -1;
    }

    MessagePump(hWnd);

    return 1;
}
