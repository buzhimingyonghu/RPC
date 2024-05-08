#include <assert.h>
#include "common/log.h"
#include "common/util.h"
#include "net/io_thread.h"
namespace rpc
{
    IOThread::IOThread()
    {
        int rt = sem_init(&m_init_semaphore, 0, 0);
        assert(rt == 0);

        rt = sem_init(&m_start_semaphore, 0, 0);
        assert(rt == 0);
        m_thread = new std::thread(&IOThread::Main, this);
        sem_wait(&m_init_semaphore);
        DEBUGLOG("IOThread [%d] create success", m_thread_id);
    }

    IOThread::~IOThread()
    {
        m_eventloop->stop();
        sem_destroy(&m_init_semaphore);
        sem_destroy(&m_start_semaphore);
        m_thread->join();
        if (m_eventloop)
        {
            delete m_eventloop;
            m_eventloop = nullptr;
        }
    }

    void IOThread::start()
    {
        DEBUGLOG("Now invoke IOThread %d", m_thread_id);
        sem_post(&m_start_semaphore);
    }

    void IOThread::join()
    {
        m_thread->join();
    }
    void IOThread::Main()
    {
        m_eventloop = new Eventloop();
        m_thread_id = getThreadId();
        sem_post(&m_init_semaphore);
        DEBUGLOG("IOThread %d created, wait start semaphore", m_thread_id);
        sem_wait(&m_start_semaphore);
        DEBUGLOG("IOThread %d start loop ", m_thread_id);
        m_eventloop->run();
        DEBUGLOG("IOThread %d end loop ", m_thread_id);
    }
}
// 83