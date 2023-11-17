#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include "AppClient.h"
#include <fileapi.h>
#include "helpers/SocketClient.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <map>

bool Client::send(const std::string& url, const std::string& msg)
{
    SocketClient s;
    int start = clock();

    while ((clock() - start) / CLOCKS_PER_SEC < 60) {
        
        if (!s.init() || !s.connect(url)) {
            continue;
        }
        std::string filepath;
        int len;

        if (fileExists(msg)) {

            std::string fileExtension = msg.substr(msg.find_last_of('.'));
            std::string fileName = msg.substr(msg.find_last_of('\\') + 1, msg.find_last_of('.') - msg.find_last_of('\\') - 1);

            filepath = msg;


            std::ifstream f(filepath, std::ios::binary);
            f.seekg(0, std::ios::end);
            size_t size = f.tellg();
            std::string stringFile(size, ' ');
            f.seekg(0);
            f.read(&stringFile[0], size);


            std::string res = "FILE " + fileName + fileExtension + " " + stringFile;


            len = s.sendStr(res);
            f.close();
        }
        else {
            printf("sending text message \"%s\"\n", msg.c_str());
            len = s.sendStr("TEXT " + msg);
        }
        printf("sent %d bytes\n", len);


        return len > 0;
    }
    printf("Time is out");
    return false;

    
}
