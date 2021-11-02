# Remote covert defensive operations (RCDO)
Random piece of description that is being used as some form of
documentation.

## Build
Run make to build debug mode
```
make
```

To run release mode
```
make release
```

## Differences in modes
Debug mode has a console window popup, while release does not.

## Usage
To run (each module is separated by space)
```
.\rcdob.exe module1 "module2"
```

To find out the module names, go to `main.cpp` and find it. (It's near
the top of the file)

## Configuration
All configuration are set using environmental variables. [Here's how to get environment
variables](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/getenv-wgetenv?view=msvc-160).

All environmental variables should look like this:
`RCDO_MODULENAME_VARIABLENAME`

### Connection to remote endpoint
- `RCDO_USERAGENT` - User agent when making web requests.
- `RCDO_SERVERNAME` - Server name to query for when making web requests.
- `RCDO_SERVERPORT` - Server port to connect to on host `RCDO_SERVERNAME`.
- `RCDO_ENDPOINT` - Endpoint for the binary to poll

### Keyboard
- `RCDO_KBLOCK` - Determines whether to block keyboard input or not

## Modules
Modules are all based off the interface RCDOMod (see `RCDOMod.h` for
more information on what function or variables must be present)

## Example
I made an example module called `MyMod/` which can be referenced
on how to include the interface to copy from.

## How to add a module
It should work if u do these steps.
1. Add your module to the `main.h` file
1. Make sure the module is in the map in `main.cpp`
1. Add your file to the file liste within `Makefile`

## Header files (.h)
To prevent redeclaration of header files, it is recommended to use this
structure for them. The variable names should end with `_H` to prevent
collision with other variables used in other programs.
```cpp
#ifndef VARIABLENAME_H
#define VARIABLENAME_H

// Header file content here

#endif
```

# To the person doing the keyboard input to run the computer
U can see `run.cmd` to see what commands to run, such as how to set
environment variables in cmd, detach the process from the parent
console. If the binary is built in release mode, then no window will
popup, but the program still works.
