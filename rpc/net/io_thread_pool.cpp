#include "io_thread_pool.h"

namespace rpc
{
    IOThreadPoll::IOThreadPoll(int size)
    {
        m_iothreads.resize(size);
        for (int i = 0; i < size; i++)
        {
            m_iothreads[i] = new IOThread();
        }
    }

    IOThreadPoll::~IOThreadPoll()
    {
        for (int i = 0; i < m_iothreads.size(); i++)
        {
            delete m_iothreads[i];
        }
    }
    void IOThreadPoll::start()
    {
        for (size_t i = 0; i < m_iothreads.size(); i++)
        {
            m_iothreads[i]->start();
        }
    }
    void IOThreadPoll::join()
    {
        for (size_t i = 0; i < m_iothreads.size(); i++)
        {
            m_iothreads[i]->join();
        }
    }
    IOThread *IOThreadPoll::getIOThread()
    {
        m_index = (m_index + 1) % m_iothreads.size();
        return m_iothreads[m_index];
    }
}
// 38