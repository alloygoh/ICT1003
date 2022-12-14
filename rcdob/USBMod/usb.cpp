#include "usb.h"

#include <stdio.h>
#include <setupapi.h>
#include <initguid.h>
#include <Usbiodef.h>
#include <winioctl.h>
#include <Shlobj.h>

int USBMod::requireAdmin(){
    return 1;
}

void USBMod::start(){
    setDeviceState(GUID_DEVINTERFACE_DISK, false);
    setDeviceState(GUID_DEVINTERFACE_USB_DEVICE, false);
    Sleep(5000);
    monitorAndBlockNewConnection();
}

void USBMod::kill(){
    toKillMutex.lock();
    toKill=1;
    toKillMutex.unlock();
    setDeviceState(GUID_DEVINTERFACE_DISK, true);
    setDeviceState(GUID_DEVINTERFACE_USB_DEVICE, true);
}

/* Enable/disable storage devices that are already installed */
void setDeviceState(GUID devClass, BOOL bEnable){
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&devClass, NULL, NULL, DIGCF_DEVICEINTERFACE);
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
