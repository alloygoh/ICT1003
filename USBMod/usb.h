#ifndef USB_H
#define USB_H

#include <windows.h>
#include <stdio.h>
#include <setupapi.h>
#include <initguid.h>
#include <winioctl.h>
#include <Shlobj.h>

#include "../RCDOMod.h"

class USBMod: public RCDOMod{
    public:
        int requireAdmin();
        void start();
        void kill();
};

#endif
