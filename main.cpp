#include "main.h"

MyMod* myMod = new MyMod();
MouseMod* mouseMod = new MouseMod();
USBMod* usbMod = new USBMod();
std::map<std::wstring, RCDOMod*> modules = {
    { L"my_mod", myMod },
    { L"mouse", mouseMod },
    { L"usb", usbMod }
};

int wmain(int argc, wchar_t * argv[]){
    // Argument parsing
    int requireAdmin = 0;
    std::vector<std::wstring> modulesToRun;
    for(int i = 1; i < argc; ++i){
        std::wstring moduleName(argv[i]);
        modulesToRun.push_back(moduleName);

        requireAdmin |= modules.at(moduleName)->requireAdmin();
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
    Sleep(5000);

    // Kill modules
    for(std::wstring moduleName: modulesToRun){
        // Start module
        (*modules.at(moduleName)).kill();
    }

    return 0;
}

/* Spawns current process as admin */
int elevate() {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "powershell -c Start-Process -Verb RunAs %s", GetCommandLineA());
    system(buffer);
    return 0;
}
