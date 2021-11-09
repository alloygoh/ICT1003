#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <wininet.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <iterator>
#include <map>

#pragma comment (lib, "Wininet.lib")

void pollKillSwitch();
std::wstring getEnvVar(const wchar_t * envVarName, const wchar_t * defaultValue);
std::string sendRequest(std::wstring verb, std::wstring endpoint,
        std::map<std::wstring,std::wstring>* requestBody);
void notify(std::wstring data);
bool checkRCDOKey();

#endif
