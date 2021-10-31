#ifndef MOUSEMOD_H
#define MOUSEMOD_H

#include <stdio.h>
#include "../RCDOMod.h"

class MouseMod: public RCDOMod{
    public:
        int requireAdmin = 0;
        void start();
        void kill();
};

#endif
