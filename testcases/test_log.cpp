#include <thread>
#include "log.h"
#include "config.h"
#include <stdlib.h>
void *fun()
{
    int i = 20;
    while (i--)
    {
        DEBUGLOG("debug this is thread in func %d", i);
        INFOLOG("info this is thread in func %d", i);
    }
    return NULL;
}

int main()
{

    int *temp = (int *)malloc(sizeof(int) * 1);
    rpc::Config::SetGlobalConfig("../conf/rpc.xml");

    rpc::Logger::InitGlobalLogger();

    std::thread t(fun);

    int i = 20;
    while (i--)
    {
        DEBUGLOG("test debug log %d", i);
        INFOLOG("test info log %d", i);
    }
    t.join();
    return 0;
}