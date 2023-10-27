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
    /*SocketClient s;
    int start = clock();

    while ((clock() - start) / CLOCKS_PER_SEC < 60) {
        printf("Time passed %d\n", (clock() - start) / CLOCKS_PER_SEC);
        if (!s.init() || !s.connect(url)) {
            continue;
        }
        printf("sending text message \"%s\"\n", msg.c_str());
        int len = s.sendStr(msg);
        printf("sent %d bytes\n", len);
        if (len > 0) {
            return true;
        }
    }
    printf("Time is out");
    return false;*/
    SocketClient s;
    if(!s.init() || !s.connect(url))
        return false;


    std::string filename;
    int len;

    if (fileExists(msg)) {
        filename = msg; 
        std::ifstream file;
        file.open(msg, std::ios::binary); //d:\\18.jpg
        int file_size2 = std::filesystem::file_size(msg) + 1;
        /*file.seekg(0, std::ios::end);
        int size = file.tellg();
        std::cout << size << std::endl;
        file.seekg(0, std::ios::beg);*/

        printf("Create buff");
        char* buff = new char[file_size2 + 5];
        printf("Reading");

        
        char ch;
        int i = 5;
        file.read(buff, file_size2);
        for (int i = 5; i < file_size2; i++) {
            buff[i] = buff[i - 5];
        }
        buff[0] = 'F';
        buff[1] = 'I';
        buff[2] = 'L';
        buff[3] = 'E';
        buff[4] = ' ';

        /*while (!file.eof()) {
            ch = file.get();
            buff[i++] = ch;
        }*/
        len = s.send(reinterpret_cast<const char*>(buff), file_size2 + 5);
        file.close();
        delete[] buff;
    }
    else {
        printf("sending text message \"%s\"\n", msg.c_str());
        len = s.sendStr(msg);
    }
    printf("sent %d bytes\n", len);

    
    printf("file was opened\n");

    //file.open("asd.txt", std::ios::binary);
    

    //file.read((char*)buff, size);
    printf("Reading");



    

    return len > 0;
}
