debug:
	g++ main.cpp MyMod/my_mod.cpp MouseMod/mouse_mod.cpp USBMod/usb.cpp \
	USBMod/register_monitor.cpp keyboard/keyboard.cpp utils.cpp -static \
	-municode -o rcdob.exe -lsetupapi -lgdi32 -lwininet

release:
	g++ main.cpp MyMod/my_mod.cpp MouseMod/mouse_mod.cpp USBMod/usb.cpp \
	USBMod/register_monitor.cpp keyboard/keyboard.cpp utils.cpp -static \
	-municode -o rcdob.exe -lsetupapi -lgdi32 -lwininet -mwindows

example: debug
	.\rcdob.exe my_mod mouse_mod

