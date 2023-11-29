#include <iostream>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main()
{
    while(true)
    {
        cout << "i'm processing! pid:" << getpid() << endl;
        sleep(1);
    }
    return 0;
}