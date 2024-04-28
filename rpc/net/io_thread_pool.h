#pragma once
#include <vector>
#include "log.h"
#include "io_thread.h"
namespace rpc
{
    class IOThreadPoll
    {
    public:
        IOThreadPoll(int size);
        ~IOThreadPoll();
        void start();
        void join();
        IOThread *getIOThread();

    private:
        int m_size = 0;
        std::vector<IOThread *> m_iothreads;
        int m_index = -1;
    };
}
// 37