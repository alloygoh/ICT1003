# Remote covert defensive operations (RCDO)
Random piece of description that is being used as some form of
documentation.

## Using make
Run make
```
make
```

To run example:
```
make example
```

## Main program
Assuming we call our program `rcdob`. The modules to activate will be
placed at the back, separated by `<space>`.
```
.\rcdob.exe "module 1" module2 "module 3"
```

## Modules
Modules are all based off the interface RCDOMod (see `RCDOMod.h` for
more information on what function or variables must be present)

## Module specific configuration
My suggestion is to use **environment variables** for configuration
related to the module. The suggested naming format is
`RCDO_MODULENAME_VARIABLENAME`. [Here's how to get environment
variables](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/getenv-wgetenv?view=msvc-160)

## Example
I made an example module called `MyMod/` which can be referenced
on how to include the interface to copy from.

# How to add a module
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
