#include"Progress_Bar.h"


void Progress()
{
    char bar[101];
    memset(bar, '\0', 100);

    char signal[4] = {'-','/','|','\\'};

    int i = 0;
    for(i = 0;i <= 100;i++)
    {
        bar[i] = '=';
        bar[i + 1] = '>';
        printf("\033[1;32;40m[%-101s][%d%%][%c]\033[0m\r",bar, i, signal[i % 4]);
        fflush(stdout);

       usleep(25000);
    }
    printf("\n");
}
