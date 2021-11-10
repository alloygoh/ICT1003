#include "keyboard.h"

#define NOTIFY_ENV L"RCDO_NOTIFY" // name of env to check whether to send a notifcation to the user
#define NOTICE_DELAY 600 // delay between each keystroke notification
#define NOTICE_MSG L"A user has knowingly and without authority touched your keyboard!"

// maps keys that are hidden behind the SHIFT layer
std::map<wchar_t, wchar_t> layeredKeys = {
    {'`', '~'},
    {L'1', L'!'},
    {L'2', L'@'},
    {L'3', L'#'},
    {L'4', L'$'},
    {L'5', L'%'},
    {L'6', L'^'},
    {L'7', L'&'},
    {L'8', L'*'},
    {L'9', L'('},
    {L'0', L')'},
    {L'[', L'{'},
    {L']', L'}'},
    {L'\\', L'|'},
    {L';', L':'},
    {L'\'', L'\"'},
    {L',', L'<'},
    {L'.', L'>'},
    {L'/', L'?'}
};

// map to track keystrokes that cannot be mapped with MapVirtualKeyExW
std::map<int, std::wstring> mapSpecialKeys = {
    {VK_BACK, L"[BACKSPACE]"},
    {VK_RETURN, L"[ENTER]"},
    {VK_SPACE, L"_"},
    {VK_TAB, L"[TAB]"},
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
std::wfstream logFile;
std::mutex logFileMutex;
HANDLE breachEvent = NULL;

// stores booleans for mod keys
// +-------+------+------+
// | SHIFT | CAPS | CASE |
// +-------+------+------+
// |     0 |    0 |    0 |
// |     0 |    1 |    1 |
// |     1 |    0 |    1 |
// |     1 |    1 |    0 |
// +-------+------+------+
std::map<std::wstring, bool> modKeyStates = {
    {L"SHIFT", 0},
    {L"CAPS", 0},
    {L"CTRL", 0},
    {L"ALT", 0},
    {L"WIN", 0}
};

int KeyboardMod::requireAdmin(){
    return 0;
}

void KeyboardMod::start(){

    if (setHook()){
        return;
    }

    wprintf(L"Hook successfully installed!\n");

    std::thread monitorTimeThread(setLogFile);

    breachEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    std::thread notifyThread(sendNotice);

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

    std::wstring tempPath = getEnvVar(L"Temp", L"");

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
    logFile.seekg(0, logFile.end);

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

wchar_t formatKey(wchar_t key){
    // handles the transforming of keys that are modified using the SHIFT and CapsLock keys

    if (isalpha(key))
        return (modKeyStates.at(L"SHIFT") ^ modKeyStates.at(L"CAPS")) ? key : tolower(key);


    if (modKeyStates.at(L"SHIFT")){
        auto layeredKeyEntry = layeredKeys.find(key);
        return (layeredKeyEntry == layeredKeys.end()) ? key : layeredKeyEntry->second;
    }

    return key;
}

void logKeystroke(int vkCode){
    // KBDLLHOOKSTRUCT stores the keyboard inputs as Virtual-Key codes
    // resolution and logging of the keystrokes is performed here
    // also ignores mouse inputs

    std::wstringstream output;

    output << L"[";

    for (auto modKey : modKeyStates){
        if (modKey.second && modKey.first != L"SHIFT")
            output << modKey.first + L" + ";
    }

    if (mapSpecialKeys.find(vkCode) != mapSpecialKeys.end()){
        std::wstring key = mapSpecialKeys.at(vkCode);

        output << key;
    }
    else{
        HKL kbLayout = GetKeyboardLayout(GetCurrentProcessId());

        wchar_t key = MapVirtualKeyExW(vkCode, MAPVK_VK_TO_CHAR, kbLayout);
        key = formatKey(key);

        output << key;
    }

    output << L"]";

    logFileMutex.lock();
    logFile << output.str() << '\n';
    logFile.flush();
    logFileMutex.unlock();
}

int setModKeyState(int vkCode, int event){
    std::map<int, std::wstring> vkCodeToString = {
        {VK_SHIFT, L"SHIFT"},
        {VK_LSHIFT, L"SHIFT"},
        {VK_RSHIFT, L"SHIFT"},
        {VK_CAPITAL, L"CAPS"},
        {VK_CONTROL, L"CTRL"},
        {VK_LCONTROL, L"CTRL"},
        {VK_RCONTROL, L"CTRL"},
        {VK_MENU, L"ALT"},
        {VK_RMENU, L"ALT"},
        {VK_LMENU, L"ALT"},
        {VK_LWIN, L"WIN"},
        {VK_RWIN, L"WIN"},
    };

    if (vkCodeToString.find(vkCode) == vkCodeToString.end())
        // must log, not a mod key
        return 0;

    std::wstring keyName = vkCodeToString.at(vkCode);

    if (modKeyStates.find(keyName) == modKeyStates.end())
        return 0;

    int isKeyDown = (event == WM_KEYDOWN || event == WM_SYSKEYDOWN);

    if (keyName != L"CAPS"){
        modKeyStates.at(keyName) = isKeyDown;
        return 1;
    }

    if (isKeyDown){
        int capsState = modKeyStates.at(keyName);
        modKeyStates.at(keyName) = !capsState;
    }

    return 1;
}

LRESULT __stdcall hookCallback(int nCode, WPARAM wParam, LPARAM lParam){
    // function handler for all keyboard strokes

    // If nCode is less than zero, the hook procedure must pass the
    // message to the CallNextHookEx function without further processing
    // and should return the value returned by CallNextHookEx.
    if (nCode < 0){
        return CallNextHookEx(ghHook, nCode, wParam, lParam);
    }

    kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
    int vkCode = kbdStruct.vkCode;

    // default behavior is to notify
    std::wstring buffer = getEnvVar(NOTIFY_ENV, L"1");
    long int noticeOn = wcstol(buffer.c_str(), NULL, 2);

    int isModKey = setModKeyState(vkCode, wParam);
    if (isModKey){
        return 1;
    }

    switch (wParam){
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        logKeystroke(vkCode);
        SetEvent(breachEvent);
    }

    // returns a non-zero value if the keyboard is locked to prevent the input from reaching other processes
    return 1;
}

void sendNotice(){
    // once a keyboard breach has been detected, periodically sends notifications to the user

    WaitForSingleObject(breachEvent, INFINITE);

    notify(NOTICE_MSG);

    while (1){
        time_t noticeDelay = time(0) + NOTICE_DELAY;
        tm* localTimeFuture = localtime(&noticeDelay);

        SYSTEMTIME systemTime;
        systemTime.wDay = localTimeFuture->tm_mday;
        systemTime.wDayOfWeek = localTimeFuture->tm_wday;
        systemTime.wMonth = 1 + localTimeFuture->tm_mon;
        systemTime.wYear = 1900 + localTimeFuture->tm_year;
        systemTime.wHour = localTimeFuture->tm_hour;
        systemTime.wMinute = localTimeFuture->tm_min;
        systemTime.wSecond = localTimeFuture->tm_sec;
        systemTime.wMilliseconds = 0;

        FILETIME fileTime;
        SystemTimeToFileTime(&systemTime, &fileTime);

        LARGE_INTEGER dueTime;
        dueTime.HighPart = fileTime.dwHighDateTime;
        dueTime.LowPart = fileTime.dwLowDateTime;

        HANDLE hTimer = CreateWaitableTimer(NULL, 1, NULL);

        SetWaitableTimer(hTimer, &dueTime, 0, NULL, NULL, 1);

        WaitForSingleObject(hTimer, INFINITE);

        logFileMutex.lock();

        std::streampos oldOffset = logFile.tellg();
        logFile.seekg(0, logFile.end);
        std::streampos addedCharCount = logFile.tellg() - oldOffset;

        if (!addedCharCount){
            continue;
        }

        logFile.seekg(addedCharCount, logFile.end);

        std::wstringstream noticeStream;

        noticeStream << "The following keystrokes have been recorded in the last" << NOTICE_DELAY/60 << "minutes:\n";
        noticeStream << logFile.rdbuf();

        logFileMutex.unlock();

        notify(noticeStream.str());
    }
}