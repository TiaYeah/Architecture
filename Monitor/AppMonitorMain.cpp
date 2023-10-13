#include <windows.h>
#include "AppMonitor.h"

int main()
{
    Monitor m;
    while (true)
    {
        printf("new iter\n");
        //Sleep(5000);
        if (!m.check())
        {
            printf("Reset server\n");
            m.reset();
            if (!m.init()) // in practice some sort of alarm should be raised here
            {
                printf("Can't initialize monitor\n");
                return -1;
            }
        }
    }
}