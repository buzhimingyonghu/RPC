#include "fd_event.h"
#include <string.h>
namespace rpc
{
    FdEvent::FdEvent(int fd) : m_fd(fd)
    {
        memset(&m_listen_events, 0, sizeof(m_listen_events));
    }

    rpc::FdEvent::~FdEvent()
    {
    }

    void FdEvent::setListenCallBack(TriggerEvent event_type, std::function<void()> callback)
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
    }
}
