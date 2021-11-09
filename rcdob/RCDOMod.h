#ifndef RCDOMod_H
#define RCDOMod_H

class RCDOMod{
    public:
        virtual int requireAdmin() = 0;
        virtual void start() = 0;
        virtual void kill() = 0;
};

#endif
