debug:
# https://sourceforge.net/p/mingw-w64/discussion/723798/thread/e9636bd054/
	g++ main.cpp MyMod/my_mod.cpp MouseMod/mouse_mod.cpp USBMod/usb.cpp \
	USBMod/register_monitor.cpp keyboard/keyboard.cpp utils.cpp -static \
	-municode -o rcdob.exe -lsetupapi -lgdi32 -lwininet -specs=msvcr120.spec

release:
	g++ main.cpp MyMod/my_mod.cpp MouseMod/mouse_mod.cpp USBMod/usb.cpp \
	USBMod/register_monitor.cpp keyboard/keyboard.cpp utils.cpp -static \
	-municode -o rcdob.exe -lsetupapi -lgdi32 -lwininet -specs=msvcr120.spec \
	-mwindows

example: debug
	.\rcdob.exe my_mod mouse_mod

