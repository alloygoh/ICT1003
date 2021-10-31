#include "usb.h"
#include "register_monitor.h"

#include <stdio.h>
#include <setupapi.h>
#include <initguid.h>
#include <winioctl.h>
#include <Shlobj.h>


int USBMod::requireAdmin(){
    return 1;
}

void USBMod::start(){
    setUSBState(false);
    monitorAndBlockNewConnection();
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
        param.StateChange = DICS_ENABLE;;
    }
    else {
        param.StateChange = DICS_DISABLE;
    }

    param.Scope = DICS_FLAG_CONFIGSPECIFIC;
    param.HwProfile = 0;

    //do disable/enable action
    SP_DEVINFO_DATA devInfoData = {sizeof(SP_DEVINFO_DATA)};;
    int memberIndex = 0;
    while(1){
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
            continue;
        }
    }
}

#ifdef MAIN

int wmain(int argc, char * argv[]){
    if(!IsUserAnAdmin()){
        printf("Admin access required");
        return 1;
    }

    monitorAndBlockNewConnection();
    return 0;
}

#endif
