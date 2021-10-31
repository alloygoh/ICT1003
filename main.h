#ifndef MAIN_H
#define MAIN_H

#include <windows.h>
#include <stdio.h>
#include <processthreadsapi.h>
#include <securitybaseapi.h>
#include <Shlobj.h>
#include <winbase.h>
#include <winnt.h>
#include <wtsapi32.h>

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "RCDOMod.h"
#include "MyMod/my_mod.h"
#include "MouseMod/mouse_mod.h"
#include "USBMod/usb.h"

int elevate();

#endif
