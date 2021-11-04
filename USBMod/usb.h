#ifndef USB_H
#define USB_H

#include <windows.h>
#include "RCDOMod.h"
#include "utils.h"

class USBMod: public RCDOMod{
    public:
        int requireAdmin();
        void start();
        void kill();
};

void setUSBState(BOOL bEnable);

// register_monitor.cpp
int monitorAndBlockNewConnection();

#endif
