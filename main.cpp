#include "main.h"

MyMod myMod = MyMod();
MouseMod mouseMod = MouseMod();
std::map<std::wstring, RCDOMod*> modules = {
    { L"my_mod", &myMod }, { L"mouse_mod", &mouseMod }
};

int wmain(int argc, wchar_t * argv[]){
    // Argument parsing
    int requireAdmin = 0;
    std::vector<std::wstring> modulesToRun;
    for(int i = 1; i < argc; ++i){
        std::wstring moduleName(argv[i]);
        modulesToRun.push_back(moduleName);
        requireAdmin |= (*modules.at(moduleName)).requireAdmin;
    }

    printf("RequireAdmin: %d\n", requireAdmin);

    // Obtain administrator if required
    if(!IsUserAnAdmin() && requireAdmin){
        elevate();
        return 0;
    }

    // Look through activated modules, and do it
    // Start modules
    for(std::wstring moduleName: modulesToRun){
        // Start module
        (*modules.at(moduleName)).start();
    }

    // TODO: Call polling module
    Sleep(1000);

    // Kill modules
    for(std::wstring moduleName: modulesToRun){
        // Start module
        (*modules.at(moduleName)).kill();
    }

    return 0;
}

/* Spawns current process as admin */
int elevate() {
    wchar_t PathProg[MAX_PATH];

    if (!GetModuleFileNameW(NULL, PathProg, MAX_PATH))
    {
        printf("GetModuleFileName %lu\n", GetLastError());
    }

    SHELLEXECUTEINFOW shExInfo = {sizeof(shExInfo)};
    shExInfo.lpVerb = L"runas";
    shExInfo.lpFile = PathProg;
    shExInfo.hwnd = NULL;
    shExInfo.nShow = SW_NORMAL;

    return ShellExecuteExW(&shExInfo);
}
