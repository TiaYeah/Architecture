#define _CRT_SECURE_NO_WARNINGS
#include "AppClient.h"
#include "helpers/SocketClient.h"
#include "helpers/UtilString.h"
#include "helpers/UtilFile.h"
#include <ctime>
#include <map>

bool Client::send(const std::string& url, const std::string& msg)
{
    SocketClient s;
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
    return false;
}
