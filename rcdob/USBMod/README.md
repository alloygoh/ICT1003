# Enable/disable USB using registry
```
Key: HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\USBSTOR
Value: Start
Data: 3 for enable
      4 for disable
```
Caveat: Must restart the computer in order for this thing to work

# Alternative idea
The USB protection targets hard disk. The plan is to go to device
manager to disable the device.

Currently, the application works. However it requires elevated
privileges from the command line. While via device manager, disabling
the driver does not require admin rights. There probably is a way
without admin.

# Usage
```bash
$ gcc usb.c -lsetupapi
$ .\a.exe
```
