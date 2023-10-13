#define _CRT_SECURE_NO_WARNINGS
#include "AppClient.h"
#include "helpers/SocketClient.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"
#include <iostream>
#include <fstream>
#include <map>

bool Client::send(const std::string& url, const std::string& msg)
{
    SocketClient s;
    if(!s.init() || !s.connect(url))
        return false;

    std::ifstream file;
    file.open("d:\\18.jpg", std::ios::binary);
    printf("file was opened\n");

    //file.open("asd.txt", std::ios::binary);
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    std::cout << size << std::endl;
    file.seekg(0, std::ios::beg);

    printf("Create buff");
    unsigned char* buff = new unsigned char[size + 1];
    printf("Reading");

    char ch;
    int i = 0;
    while (!file.eof()) {
        ch = file.get();
        buff[i++] = ch;
    }

    //file.read((char*)buff, size);
    printf("Reading");
    file.close();

    int len = s.send(reinterpret_cast<const char*>(buff), size);

    delete[] buff;

    printf("sending text message \"%s\"\n", msg.c_str());
    //int len = s.sendStr(msg);

    printf("sent %d bytes\n", len);
    return len > 0;
}
