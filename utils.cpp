#include <windows.h>
#include <WinInet.h>
#include <corecrt_wstdlib.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <iterator>

#pragma comment (lib, "Wininet.lib")

#include "utils.h"

#define BUFSIZE 512

// RCDO_USERAGENT
// RCDO_SERVERNAME
// RCDO_SERVERPORT
std::wstring getEnvVar(const wchar_t * envVarName, const wchar_t * defaultValue){
    wchar_t * buffer;
    size_t bufferSize;
    _wdupenv_s(&buffer, &bufferSize, envVarName);

    std::wstring output(defaultValue);
    if(buffer != NULL){
        output = buffer;
    }

    free(buffer);
    return output;
}

std::string readFromServer(){
    std::wstring RCDO_USERAGENT = getEnvVar(L"RCDO_USERAGENT", L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.54 Safari/537.36");
    std::wstring RCDO_SERVERNAME = getEnvVar(L"RCDO_SERVERNAME", L"127.0.0.1");
    int RCDO_SERVERPORT = stoi(getEnvVar(L"RCDO_SERVERPORT", L"80"));
    std::wstring RCDO_ENDPOINT = getEnvVar(L"RCDO_ENDPOINT", L"/api/cankillmyself.html");

    HINTERNET hInternet = InternetOpenW(RCDO_USERAGENT.c_str(),INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
    HINTERNET hConnect = InternetConnectW(hInternet, RCDO_SERVERNAME.c_str(), RCDO_SERVERPORT, NULL,NULL,INTERNET_SERVICE_HTTP,0,0);

    // GET Request
    HINTERNET hHttpFile = HttpOpenRequestW(hConnect, NULL, RCDO_ENDPOINT.c_str(),NULL,NULL,NULL,INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_NO_CACHE_WRITE,0);
    if (!HttpSendRequestW(hHttpFile, NULL, 0, 0, 0)){
        std::wcout << L"HttpSendRequest Failed" << std::endl;
        std::wcout << GetLastError() << std::endl;
        return "";
    }

    DWORD contentlen = 0;
    DWORD size_f = sizeof(contentlen);
    if(!(HttpQueryInfoW(hHttpFile, HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER, &contentlen, &size_f, NULL) && contentlen > 0)) {
        std::wcout << L"HttpQueryInfo Failed" << std::endl;
        std::wcout << GetLastError() << std::endl;
        return "";
    }

    BYTE* data = (BYTE*)malloc(contentlen + 1);
    BYTE* currentPtr = data;
    DWORD copied = 0;
    for(;;){
        BYTE dataArr[BUFSIZE];
        DWORD dwBytesRead;
        BOOL bRead;
        bRead = InternetReadFile(hHttpFile, dataArr, BUFSIZE, &dwBytesRead);
        if (dwBytesRead == 0){
            data[contentlen] = '\x00';
            break;
        }
        else if (dwBytesRead < BUFSIZE){
            dataArr[dwBytesRead] = '\x00';
        }
        if (!bRead)
            std::wcout << L"InternetReadFile Failed" << std::endl;
        else{
            memcpy_s(currentPtr, contentlen - copied, dataArr, dwBytesRead);
            currentPtr += dwBytesRead;
            copied += dwBytesRead;
        }
    }
    std::wcout << L"[+] Bytes Read: " << std::to_wstring(copied) << std::endl;
    std::string content((char*)data);
    std::cout << "[+] Content: " << content << std::endl;

    // process stuff here

    free(data);
    InternetCloseHandle(hInternet);
    InternetSetOptionW(NULL, INTERNET_OPTION_SETTINGS_CHANGED,NULL,0);
    return content;
}

void pollKillSwitch(){
    // isFirstLoop whether is it the first time requesting from server,
    // if the first request already failed, then stop the system after 5
    // seocnds, if not the user gonna be fucked.
    int isFirstLoop = 1;
    for(;;) {
        std::string content = readFromServer();
        if(content.empty() && isFirstLoop){
            Sleep(5000);
            return;
        }

        if (content.compare(0,4,"TRUE") == 0){
            return;
        }

        Sleep(5000);
        isFirstLoop = 0;
    }
}
