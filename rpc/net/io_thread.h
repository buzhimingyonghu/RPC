#pragma once
#include <thread>
#include <condition_variable>
#include <semaphore.h>
#include "eventloop.h"
namespace rpc
{
    class IOThread
    {
    public:
        IOThread();
        ~IOThread();
        inline Eventloop *getEventLoop()
        {
            return m_eventloop;
        }
        void start();

        void join();

        void Main();

    private:
        Eventloop *m_eventloop{nullptr};
        sem_t m_init_semaphore;
        sem_t m_start_semaphore;
        pid_t m_thread_id;
        std::thread *m_thread{nullptr};
    };

}
// 40