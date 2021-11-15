# https://sourceforge.net/p/mingw-w64/discussion/723798/thread/e9636bd054/
SHELL=cmd

debug:
	g++ main.cpp MouseMod/mouse.cpp USBMod/usb.cpp \
	USBMod/monitor_usb_port.cpp KeyboardMod/keyboard.cpp utils.cpp -static \
	-municode -o rcdob.exe -lsetupapi -lgdi32 -lwininet -D _DEBUG -I .  -Wall \
	-pedantic

release:
	g++ main.cpp MouseMod/mouse.cpp USBMod/usb.cpp \
	USBMod/monitor_usb_port.cpp KeyboardMod/keyboard.cpp utils.cpp -static \
	-municode -o rcdob.exe -lsetupapi -lgdi32 -lwininet -I . -mwindows -Wall \
	-pedantic

deploy: release
	move rcdob.exe ..\rcdon\bin\watch_binary.exe
