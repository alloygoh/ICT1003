#ifndef MOUSEMOD_H
#define MOUSEMOD_H

#include <stdio.h>
#include "../RCDOMod.h"

class MouseMod: public RCDOMod{
    public:
        int requireAdmin();
        void start();
        void kill();
	private:
		//single ton
		static MouseMod& Instance(){
			static MouseMod mouseMod;
			return mouseMod;
		}
		HHOOK hook; // handle to the hook
		void InstallHook(); // function to install our hook
		void UninstallHook(); // function to uninstall our hook

		MSG msg; // struct with information about all messages in our queue
		int Messsages(); // function to "deal" with our messages
};

#endif
