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
static std::string sPort = "21788";

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
    std::string filename = std::string("./resources/ALIVE" + sServer.pid());
    struct stat buff;
    if (fileExists(filename)) {
        stat(filename.c_str(), &buff);
        //std::cout << "Difference " << time(NULL) - buff.st_mtime << std::endl;
        if (time(NULL) - buff.st_mtime > 5)
        {
            printf("Restarting server\n");
            return false;
        }
        else {
            return true;
        }
    }
    sServer.wait(3000);

    return false;
}

void Monitor::reset()
{
    const char* createdFileName = "resources/CREATED";
    sPort = strtok(fileReadStr(createdFileName), ",");

    remove("resources/CREATED");
    std::string aliveFileName = std::string("./resources/ALIVE" + sServer.pid());
    if (remove(aliveFileName.c_str()) != 0)
    {
        printf("Can't remove file %s\n", std::string(aliveFileName));
    }

    sServer.terminate();
}
