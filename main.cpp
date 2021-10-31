#include "main.h"

BOOL elevate();

MyMod* myMod = new MyMod();
MouseMod* mouseMod = new MouseMod();
USBMod* usbMod = new USBMod();
std::map<std::wstring, RCDOMod*> modules = {
    { L"my_mod", myMod },
    { L"mouse", mouseMod },
    { L"usb", usbMod }
};

void runModule(std::wstring moduleName){
    (*modules.at(moduleName)).start();
}

int wmain(int argc, wchar_t * argv[]){
    // Arguments
    int requireAdmin = 0;
    std::vector<std::wstring> modulesToRun;
    for(int i = 1; i < argc; ++i){
        std::wstring moduleName(argv[i]);
        modulesToRun.push_back(moduleName);

        requireAdmin |= modules.at(moduleName)->requireAdmin();
    }

    // Admin
    if(!IsUserAnAdmin() && requireAdmin){
        elevate();
        return 0;
    }

    // Start
    for(std::wstring moduleName: modulesToRun){
        std::thread t(runModule, moduleName);
        t.detach();
    }

    // TODO: Change to polling function
    Sleep(5000);

    for(std::wstring moduleName: modulesToRun){
        (*modules.at(moduleName)).kill();
    }

    return 0;
}

/* Spawns current process as admin */
BOOL elevate() {
    wchar_t PathProg[MAX_PATH];

    if (!GetModuleFileNameW(NULL, PathProg, MAX_PATH))
    {
        printf("GetModuleFileName %lu\n", GetLastError());
    }

    wchar_t * parameters = GetCommandLineW() + wcslen(PathProg) + 3; // Include the two quotes + 1 space

    SHELLEXECUTEINFOW shExInfo = {sizeof(shExInfo)};
    shExInfo.lpVerb = L"runas";
    shExInfo.lpFile = PathProg;
    shExInfo.lpParameters = parameters;
    shExInfo.hwnd = NULL;
    shExInfo.nShow = SW_NORMAL;

    return ShellExecuteExW(&shExInfo);
}
