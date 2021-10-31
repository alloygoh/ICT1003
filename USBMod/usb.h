#ifndef USB_H
#define USB_H

#include "../RCDOMod.h"
class USBMod: public RCDOMod{
    public:
        int requireAdmin();
        void start();
        void kill();
};

#endif
