#include <Windows.h>
#include <iostream>
#include "mouse_mod.h"

class MyHook{
public:
    //single ton
    static MyHook& Instance(){
        static MyHook myHook;
        return myHook;
    }
    HHOOK hook; // handle to the hook
    void InstallHook(); // function to install our hook
    void UninstallHook(); // function to uninstall our hook

    MSG msg; // struct with information about all messages in our queue
    int Messsages(); // function to "deal" with our messages
};
LRESULT WINAPI MyMouseCallback(int nCode, WPARAM wParam, LPARAM lParam); //callback declaration

int MyHook::Messsages(){
    while (msg.message != WM_QUIT){ //while we do not close our application
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(1);
    }
    UninstallHook(); //if we close, let's uninstall our hook
    return (int)msg.wParam; //return the messages
}

void MyHook::InstallHook(){
    /*
    SetWindowHookEx(
    WM_MOUSE_LL = mouse low level hook type,
    MyMouseCallback = our callback function that will deal with system messages about mouse
    NULL, 0);

    c++ note: we can check the return SetWindowsHookEx like this because:
    If it return NULL, a NULL value is 0 and 0 is false.
    */
    if (!(hook = SetWindowsHookEx(WH_MOUSE_LL, MyMouseCallback, NULL, 0))){
        printf_s("Error: %x \n", GetLastError());
    }
}

void MyHook::UninstallHook(){
    /*
    uninstall our hook using the hook handle
    */
    UnhookWindowsHookEx(hook);
}

LRESULT WINAPI MyMouseCallback(int nCode, WPARAM wParam, LPARAM lParam){
    MSLLHOOKSTRUCT * pMouseStruct = (MSLLHOOKSTRUCT *)lParam; // WH_MOUSE_LL struct
    /*
    nCode, this parameters will determine how to process a message
    This callback in this case only have information when it is 0 (HC_ACTION): wParam and lParam contain info

    wParam is about WINDOWS MESSAGE, in this case MOUSE messages.
    lParam is information contained in the structure MSLLHOOKSTRUCT
    */

    if (nCode == 0) { // we have information in wParam/lParam ? If yes, let's check it:
        if (pMouseStruct != NULL){ // Mouse struct contain information?
            printf_s("Mouse Coordinates: x = %i | y = %i \n", pMouseStruct->pt.x, pMouseStruct->pt.y);
        }
        switch (wParam){
        case WM_LBUTTONUP:{
            printf_s("LEFT CLICK UP\n");
        }
        break;
        case WM_LBUTTONDOWN:{
            printf_s("LEFT CLICK DOWN\n");
        }
        break;
        case WM_MBUTTONUP:{
            printf_s("MIDDLE CLICK UP\n");
        }
        break;
        case WM_MBUTTONDOWN:{
            printf_s("MIDDLE CLICK DOWN\n");
        }
        break;
        case WM_RBUTTONUP:{
            printf_s("RIGHT CLICK UP\n");
        }
        break;
        case WM_RBUTTONDOWN:{
            printf_s("RIGHT CLICK DOWN\n");
        }
        break;
        }
    }
    /*
    Every time that the nCode is less than 0 we need to CallNextHookEx:
    -> Pass to the next hook
         MSDN: Calling CallNextHookEx is optional, but it is highly recommended;
         otherwise, other applications that have installed hooks will not receive hook notifications and may behave incorrectly as a result.
    */
    //return CallNextHookEx(MyHook::Instance().hook, nCode, wParam, lParam);
    return -1;
}

int MouseMod::requireAdmin(){
    return 0;
}

void MouseMod::start(){
    wprintf(L"Starting MouseMod\n");
    MyHook::Instance().InstallHook();
    MyHook::Instance().Messsages();
}

void MouseMod::kill(){
    wprintf(L"Killing MouseMod\n");
    MyHook::Instance().UninstallHook();
}


