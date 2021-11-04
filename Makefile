# https://sourceforge.net/p/mingw-w64/discussion/723798/thread/e9636bd054/
debug:
	g++ main.cpp MyMod/my_mod.cpp MouseMod/mouse_mod.cpp USBMod/usb.cpp \
	USBMod/register_monitor.cpp keyboard/keyboard.cpp utils.cpp -static \
	-municode -o rcdob.exe -lsetupapi -lgdi32 -lwininet -D _DEBUG -I .

release:
	g++ main.cpp MyMod/my_mod.cpp MouseMod/mouse_mod.cpp USBMod/usb.cpp \
	USBMod/register_monitor.cpp keyboard/keyboard.cpp utils.cpp -static \
	-municode -o rcdob.exe -lsetupapi -lgdi32 -lwininet -I . -mwindows

example: debug
	.\rcdob.exe my_mod mouse_mod

