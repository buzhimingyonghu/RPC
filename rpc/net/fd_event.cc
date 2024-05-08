#include "net/fd_event.h"
#include <string.h>
#include <fcntl.h>
namespace rpc
{
    FdEvent::FdEvent(int fd) : m_fd(fd)
    {
        memset(&m_listen_events, 0, sizeof(m_listen_events));
    }

    FdEvent::FdEvent()
    {
        memset(&m_listen_events, 0, sizeof(m_listen_events));
    }

    rpc::FdEvent::~FdEvent()
    {
    }

    void FdEvent::setNonBlock()
    {
        int flag = fcntl(m_fd, F_GETFL, 0);
        if (flag & O_NONBLOCK)
        {
            return;
        }
        fcntl(m_fd, F_SETFL, flag | O_NONBLOCK);
    }

    void FdEvent::setListenCallBack(TriggerEvent event_type, std::function<void()> callback, std::function<void()> error_callback)
    {

        if (event_type == TriggerEvent::IN_EVENT)
        {
            m_listen_events.events |= EPOLLIN;
            m_read_callback = callback;
        }
        else if (event_type == TriggerEvent::OUT_EVENT)
        {
            m_listen_events.events |= EPOLLOUT;
            m_write_callback = callback;
        }
        if (m_error_callback == nullptr)
        {
            m_error_callback = error_callback;
        }
        else
        {
            m_error_callback = nullptr;
        }
        // 问题
        m_listen_events.data.ptr = this;
    }

    std::function<void()> FdEvent::getCallBackFunc(TriggerEvent event_type)
    {
        if (event_type == TriggerEvent::IN_EVENT)
        {
            return m_read_callback;
        }
        else if (event_type == TriggerEvent::OUT_EVENT)
        {
            return m_write_callback;
        }
        else if (event_type == TriggerEvent::ERROR_EVENT)
        {
            return m_error_callback;
        }
        return nullptr;
    }
    void FdEvent::cancle(TriggerEvent event_type)
    {
        if (event_type == TriggerEvent::IN_EVENT)
        {
            m_listen_events.events &= (~EPOLLIN);
        }
        else
        {
            m_listen_events.events &= (~EPOLLOUT);
        }
    }
}
// 66