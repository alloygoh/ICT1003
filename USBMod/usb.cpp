#include <windows.h>
#include <stdio.h>
#include <setupapi.h>
#include <initguid.h>
#include <winioctl.h>
#include <Shlobj.h>

#include "usb.h"

void setUSBState(BOOL bEnable);
int USBMod::requireAdmin(){
    return 1;
}

void USBMod::start(){
    setUSBState(false);
}

void USBMod::kill(){
    setUSBState(true);
}

/* Enable/disable storage devices that are already installed */
void setUSBState(BOOL bEnable){
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, NULL, DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE){
        printf("SetupDiGetClassDevsW error: %lu\n", GetLastError());
        return;
    }

    // https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/3f812f32-3f97-4b35-b878-be5f5327a4e8/enabledisable-usb-device-with-win-api?forum=windowssdk
    SP_PROPCHANGE_PARAMS param;

    param.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
    param.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;

    if(bEnable) {
        printf("ENABLE");
        param.StateChange = DICS_ENABLE;;
    }
    else {
        printf("DISABLE");
        param.StateChange = DICS_DISABLE;
    }

    param.Scope = DICS_FLAG_CONFIGSPECIFIC;
    param.HwProfile = 0;

    //do disable/enable action
    SP_DEVINFO_DATA devInfoData = {sizeof(SP_DEVINFO_DATA)};;
    int memberIndex = 0;
    while(1){
        printf("I AM HERE\n");
        if(!SetupDiEnumDeviceInfo(hDevInfo, memberIndex++, &devInfoData)){
            DWORD err = GetLastError();
            if(err == ERROR_NO_MORE_ITEMS){
                break;
            }else{
                printf("SetupDiEnumDeviceInfo: %lu\n", GetLastError());
                return;
            }

        }

        if(!SetupDiSetClassInstallParams(hDevInfo, &devInfoData, &param.ClassInstallHeader,sizeof(param))){
            printf("SetupDiSetClassInstallParams: %lu\n", GetLastError());
            continue;
        }

        if(!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &devInfoData)){
            printf("SetupDiCallClassInstaller: 0x%x\n", (unsigned int)GetLastError());
            printf("0x%x\n", ERROR_IN_WOW64);
            continue;
        }
    }
}

#ifdef MAIN

int main(int argc, char * argv[]){
    if(argc <= 1){
        printf("WRONG");
        return 1;
    }

    if(!IsUserAnAdmin()){
        printf("Admin access required");
        return 1;
    }
    int enable = atoi(argv[1]);
    doSomething(enable);
    return 0;
}

#endif
