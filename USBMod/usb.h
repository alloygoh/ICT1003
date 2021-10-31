#ifndef USB_H
#define USB_H

#include <windows.h>
#include "../RCDOMod.h"

class USBMod: public RCDOMod{
    public:
        int requireAdmin();
        void start();
        void kill();
};

void setUSBState(BOOL bEnable);

#endif
