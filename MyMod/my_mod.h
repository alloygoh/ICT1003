#ifndef MYMOD_H
#define MYMOD_H

#include <stdio.h>
#include "../RCDOMod.h"

class MyMod: public RCDOMod{
    public:
        int requireAdmin = 1;
        void start();
        void kill();
};

#endif
