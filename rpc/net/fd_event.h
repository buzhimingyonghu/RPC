#pragma once
#include <functional>
#include <sys/epoll.h>

namespace rpc
{
    class FdEvent
    {
    public:
        enum TriggerEvent
        {
            IN_EVENT = EPOLLIN,
            OUT_EVENT = EPOLLOUT,
            ERROR_EVENT = EPOLLERR,
        };
        FdEvent(int fd);
        FdEvent();
        ~FdEvent();
        // 设置文件描述符的非阻塞模式
        void setNonBlock();
        void setListenCallBack(TriggerEvent event_type, std::function<void()> callback, std::function<void()> error_callback = nullptr);
        // 获得对应的回调函数
        std::function<void()> getCallBackFunc(TriggerEvent event_type);
        // 取消当前文件描述符的读or写属性
        void cancle(TriggerEvent event_type);
        inline int getFd()
        {
            return m_fd;
        }
        inline epoll_event get_Epoll_Event()
        {
            return m_listen_events;
        }
        // 问题，为什么用保护成员
    protected:
        int m_fd = -1;
        // 问题
        epoll_event m_listen_events;
        std::function<void()> m_read_callback;
        std::function<void()> m_write_callback;
        std::function<void()> m_error_callback{nullptr};
    };

}
// 52