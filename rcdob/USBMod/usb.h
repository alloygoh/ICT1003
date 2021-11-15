#ifndef USB_H
#define USB_H

#include <windows.h>
#include <mutex>
#include <thread>

#include "RCDOMod.h"
#include "utils.h"

class USBMod: public RCDOMod{
    public:
        int requireAdmin();
        void start();
        void kill();
};

void setDeviceState(GUID devClass, BOOL bEnable);

// register_monitor.cpp
int monitorAndBlockNewConnection();

extern int toKill;
extern std::mutex toKillMutex;
#endif
