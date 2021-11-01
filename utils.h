#ifndef UTILS_H
#define UTILS_H

#include <string>

void pollKillSwitch();
std::wstring getEnvVar(const wchar_t * envVarName, const wchar_t * defaultValue);

#endif
