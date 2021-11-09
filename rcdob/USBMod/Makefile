build:
	g++ usb.cpp -static -D MAIN -lsetupapi
	.\a.exe 0

test:
	g++ register_monitor.cpp usb.cpp -municode -lgdi32 -lsetupapi -static -D MAIN
