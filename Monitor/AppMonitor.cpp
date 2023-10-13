#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <string>
#include "AppMonitor.h"
#include "helpers/Process.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"
#include "ctime"
#include <fileapi.h>
#include <iostream>
#include <fstream>
#include <timezoneapi.h>
#include <cstdint>
#include <filesystem>

static Process sServer;
static std::string sPort = "57822";

bool Monitor::init()
{
    m_console.handleCtrlC(Monitor::reset); // if Monitor's execution is aborted via Ctrl+C, reset() cleans up its internal state
    char cmd[256] = {};
    sprintf(cmd, "Server.exe %s", sPort.c_str());
    remove("resources/CREATED");
    bool ok = sServer.create(cmd); // launching Server
    printf(ok ? "monitoring \"%s\"\n" : "error: cannot monitor \"%s\"\n", cmd);

    return ok;
}

bool Monitor::check()
{
    sServer.wait(1000);

    const char* filename = (std::string("resources/ALIVE") + sServer.pid()).c_str();
    std::ifstream ifs(filename);

    HANDLE handle = CreateFileA((std::string("resources/ALIVE") + sServer.pid()).c_str(),0,FILE_SHARE_READ,NULL, OPEN_EXISTING, NULL, NULL);
    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME st, st2;

    if (!GetFileTime(handle, &ftCreate, &ftAccess, &ftWrite))
        return false;

    FileTimeToSystemTime(&ftCreate, &st2);
    GetSystemTime(&st);
    int systemTime = st.wMinute * 60 + st.wSecond;
    int fileAccessTime = st2.wMinute * 60 + st2.wSecond;
    std::cout << systemTime << std::endl;
    std::cout << fileAccessTime << std::endl;

    if (systemTime - fileAccessTime > 3)
    {
        printf("Restarting server");
        return false;
    }
    SYSTEMTIME st3;
    FILETIME ft3;
    GetSystemTime(&st3);
    SystemTimeToFileTime(&st3, &ft3);
    SetFileTime(handle, &ft3, &ftAccess, &ftWrite);
    return true;
}

void Monitor::reset()
{
    remove("resources/CREATED");
    const char* filename = (std::string("resources/ALIVE") + sServer.pid()).c_str();
    if (remove(filename) != 0)
    {
        printf("Can't remove file %s\n", std::string(filename));
    }

    sServer.terminate();
}
