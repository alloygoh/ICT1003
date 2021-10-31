build:
	g++ main.cpp MyMod/my_mod.cpp -static -municode -o rcdob.exe

example: build
	.\rcdob.exe my_mod

