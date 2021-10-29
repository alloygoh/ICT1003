#include <windows.h>
#include <initguid.h>
/* #include <ntddstor.h> */
#include <winioctl.h>
#include <setupapi.h>
#include <stdio.h>

void doSomething(){
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, NULL, DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE){
        printf("SetupDiGetClassDevsW error: %d\n", GetLastError());
        return;
    }

    // https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/3f812f32-3f97-4b35-b878-be5f5327a4e8/enabledisable-usb-device-with-win-api?forum=windowssdk
    SP_PROPCHANGE_PARAMS param;

    param.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
    param.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;

    boolean bEnable=TRUE;
    if(bEnable == TRUE) {
        param.StateChange = DICS_ENABLE;;
    }
    else {
        param.StateChange = DICS_DISABLE;
    }

    param.Scope = DICS_FLAG_CONFIGSPECIFIC;
    param.HwProfile = 0;

    //do disable/enable action
    SP_DEVINFO_DATA devInfoData = {sizeof(SP_DEVINFO_DATA)};;
    if(!SetupDiEnumDeviceInfo(hDevInfo, 0, &devInfoData)){
        printf("SetupDiEnumDeviceInfo: %d\n", GetLastError());
        return;
    }

    if(!SetupDiSetClassInstallParams(hDevInfo, &devInfoData, &param.ClassInstallHeader,sizeof(param))){
        printf("SetupDiSetClassInstallParams: %d\n", GetLastError());
        return;
    }

    if(!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &devInfoData)){
        printf("SetupDiCallClassInstaller: %d\n", GetLastError());
        return;
    }

}

void main(){
    doSomething();
}

