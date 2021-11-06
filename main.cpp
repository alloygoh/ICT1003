#include "main.h"

#include <userenv.h>
#include <winbase.h>

BOOL elevate(wchar_t **envp);

MyMod* myMod = new MyMod();
MouseMod* mouseMod = new MouseMod();
USBMod* usbMod = new USBMod();
KeyboardMod* keyboardMod = new KeyboardMod();
std::map<std::wstring, RCDOMod*> modules = {
    { L"my_mod", myMod },
    { L"mouse", mouseMod },
    { L"keyboard", keyboardMod },
    { L"usb", usbMod }
};

void runModule(std::wstring moduleName){
    (*modules.at(moduleName)).start();
}

void parseAndSetEnvVar(std::wstring environment){
    int pos;
    std::wstring delimiter = L"\n";
    while ((pos = environment.find(delimiter)) != std::string::npos) {
        std::wstring envLine = environment.substr(0, pos);

        std::wstring innerDelimiter = L"=";

        _wputenv(envLine.c_str());

        std::wcout << "Env line: " << envLine << std::endl;
        environment.erase(0, pos + delimiter.length());
    }
}

int wmain(int argc, wchar_t * argv[], wchar_t **envp){
    /* if(!checkRCDOKey()){ */
    /*     MessageBoxW(NULL, L"RCDO_KEY not defined", L"Error", MB_OK); */
    /*     return 1; */
    /* } */
    int firstModPos = 1;

#ifdef _DEBUG
    for(int i = 0; i < argc; i++){
        printf("argv[%d]: %ls\n", i, argv[i]);
    }
#endif

    if(argc >= 3 && wcsncmp(argv[1],L"-e",2) == 0){
        parseAndSetEnvVar(argv[2]);
        firstModPos = 3;
    }

    // Arguments
    int requireAdmin = 0;
    std::vector<std::wstring> modulesToRun;
    for(int i = firstModPos; i < argc; ++i){
        std::wstring moduleName(argv[i]);
        modulesToRun.push_back(moduleName);

        requireAdmin |= modules.at(moduleName)->requireAdmin();
    }

    // Admin
    if(!IsUserAnAdmin() && requireAdmin){
        elevate(envp);
        return 0;
    }

    // Start
    for(std::wstring moduleName: modulesToRun){
        std::thread t(runModule, moduleName);
        t.detach();
    }

    pollKillSwitch();

    for(std::wstring moduleName: modulesToRun){
        (*modules.at(moduleName)).kill();
    }

    sendRequest(L"POST", L"/api/report/unblock", NULL);
    return 0;
}

/* Spawns current process as admin */
BOOL elevate(wchar_t **envp) {
    wchar_t PathProg[MAX_PATH];

    if (!GetModuleFileNameW(NULL, PathProg, MAX_PATH))
    {
        printf("GetModuleFileName %lu\n", GetLastError());
    }

    // Prepare environment variables
    std::wstring parameters;
    for (; *envp != 0; envp++)
    {
        if(wcsncmp(*envp, L"RCDO_", 5) != 0){
            continue;
        }
        parameters.append(*envp);
        parameters.append(L"\n");
    }
    if(!parameters.empty()){
        parameters = L"-e " + parameters;
    }

    // Include the two quotes + 1 space
    int numArgs;
    LPWSTR* arguments = CommandLineToArgvW(GetCommandLineW(), &numArgs);
    wchar_t * modules = GetCommandLineW() + wcslen(*arguments) +
        (sizeof(wchar_t)*1); // No need free
    LocalFree(arguments); // Why? Because the docs tells me so:
    // https://docs.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-commandlinetoargvw
    parameters += L" ";
    parameters += modules;

    // Parameters:
    // [ -e "envStringHere" ] module1 module2 ...
    // Env string is optional
    SHELLEXECUTEINFOW shExInfo = {sizeof(shExInfo)};
    shExInfo.lpVerb = L"runas";
    shExInfo.lpFile = PathProg;
    shExInfo.lpParameters = parameters.c_str();
    shExInfo.hwnd = NULL;
    shExInfo.nShow = SW_NORMAL;


    return ShellExecuteExW(&shExInfo);
}
