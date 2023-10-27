#include <windows.h>
#include "AppMonitor.h"
#include <time.h>

int main()
{
    Monitor m;
    m.init();
    Sleep(3000);
    /*clock_t start = clock();
    clock_t end = clock();*/

    while (true)
    {   
        /*printf("Tick %f\n", (double)(end - start) / CLOCKS_PER_SEC);
        start = end;
        end = clock();*/
        Sleep(1000);

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