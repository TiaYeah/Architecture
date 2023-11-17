#define _CRT_SECURE_NO_WARNINGS
#include "AppServer.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"
#include <string>
#include <process.h>
#include <windows.h>
#include <iostream>
#include <fstream>

bool Server::init(int port)
{
    if (!m_socket.init(1000) || !m_socket.listen(port))
        return false;

    CreateDirectory("resources", NULL);

    if (!fileWriteExclusive("resources\\CREATED", toStr(m_socket.port()) + "," + toStr(_getpid())))
        return false;

    printf("server started: port %d, pid %d\n", m_socket.port(), _getpid());

    char* state = fileReadStr("resources\\STATE"); // load state from previous run
    if (state)
    {
        for (std::string& line : split(state, "\n"))
            if (!line.empty())
                m_data.push_back(line);
        delete[] state;
    }

    return true;
}

void Server::run()
{
    while (1)
    {
        fileWriteStr(std::string("resources\\ALIVE") + toStr(_getpid()), ""); // pet the watchdog
        std::shared_ptr<Socket> client = m_socket.accept(); // accept incoming connection
        if (!client->isValid())
            continue;

        int n = client->recv(); // receive data from the connection, if any
        char* data = client->data();

        printf("-----RECV-----\n%s\n--------------\n", n > 0 ? data : "Error");
        const std::vector<std::string>& tokens = split(data, " ");
        if (tokens.size() >= 2) {// this is browser's request 
            if (tokens[0] == "GET")
            {
                // convert URL to file system path, e.g. request to img/1.png resource becomes request to .\img\1.png file in Server's directory tree
                const std::string& filename = join(split(tokens[1], "/"), "\\");
                std::string fileExtension = filename.substr(filename.find_last_of('.') + 1);

                if (filename == "\\")
                { // main entry point (e.g. http://localhost:12345/)
                    std::string payload = "";
                    for (auto s : m_data) {
                        std::string fileExtension = s.substr(s.find_last_of('.') + 1);
                        if (fileExists("resources/" + s) && (fileExtension == "jpg" || fileExtension == "png")) {
                            payload += "<p><img src=\"/" + s + "\"></p>\n";
                        }
                        else {
                            payload += (s + "<br>"); // collect all the feed and send it back to browser
                        }
                    }
                    client->sendStr("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + toStr(payload.length()) + "\r\n\r\n" + payload);
                }
                else if (fileExists("resources/" + filename) && (fileExtension == "jpg" || fileExtension == "png"))
                {
                    std::ifstream file("resources" + filename, std::ios::binary);
                    if (file.is_open())
                    {
                        file.seekg(0, std::ios::end);
                        size_t size = file.tellg();
                        std::string stringFile(size, ' ');
                        file.seekg(0);
                        file.read(&stringFile[0], size);
                        client->sendStr("HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: " + std::to_string(size) + "\r\n\r\n" + stringFile);
                    }
                    else
                    {
                        std::cout << "Can't open file\n";
                    }
                }
                else
                {
                    client->sendStr("HTTP/1.1 404 Not Found\r\n\r\n");
                }
            }
            else if (tokens[0] == "FILE") {
                std::string filename = tokens[1];
                std::ofstream file("resources/" + filename, std::ios_base::binary);
                if (file.is_open()) {
                    int gap = tokens[0].length() + tokens[1].length() + 2;
                    int dataSize = n - gap;

                    file.write(&data[gap], dataSize);
                    file.flush();
                }
                else {
                    std::cout << "Can't open file\n";
                }
                file.close();

                m_data.push_back(filename); // store it in the feed
                fileAppend("resources\\STATE", m_data.back() + "\n"); // store it in the file for subsequent runs
            }
            else if (tokens[0] == "TEXT") // this is Client's request who wants to upload some data
            {
                m_data.push_back(tokens[1]); // store it in the feed
                fileAppend("resources\\STATE", m_data.back() + "\n"); // store it in the file for subsequent runs
            }
        }
    }
}
