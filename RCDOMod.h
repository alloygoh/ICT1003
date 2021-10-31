#ifndef RCDOMod_H
#define RCDOMod_H

class RCDOMod{
    public:
        int requireAdmin;
        virtual void start() = 0;
        virtual void kill() = 0;
};

#endif
