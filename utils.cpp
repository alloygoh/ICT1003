#include "windows.h"
#include "WinInet.h"
#include <iostream>
#include <string>
#include <iterator>

#pragma comment (lib, "Wininet.lib")

#define BUFSIZE 512

std::string readFromServer(){
    HINTERNET hInternet = InternetOpenW(L"PLACEHOLDER",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
    HINTERNET hConnect = InternetConnectW(hInternet,L"127.0.0.1",5000,NULL,NULL,INTERNET_SERVICE_HTTP,0,0);

    // GET Request
    HINTERNET hHttpFile = HttpOpenRequestW(hConnect, NULL, L"/api/cankillmyself.html",NULL,NULL,NULL,INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_NO_CACHE_WRITE,0);
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
    for(;;) {
        std::string content = readFromServer();
        if (content.compare(0,4,"TRUE") == 0){
            return;
        }
        Sleep(5000);
    }
}

int main(){
    pollKillSwitch();
    return 0; 
}