build:
	g++ main.cpp MyMod/my_mod.cpp MouseMod/mouse_mod.cpp -static -municode -o rcdob.exe

example: build
	.\rcdob.exe my_mod mouse_mod

