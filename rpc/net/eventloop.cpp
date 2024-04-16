#include <sys/eventfd.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/errno.h>
#include "eventloop.h"
#include "log.h"
#include "../common/util.h"
#include "wakeup_fd_event.h"
#define ADD_TO_EPOLL()                                                                       \
    auto it = m_listen_fds.find(event->getFd());                                             \
    int op = EPOLL_CTL_ADD;                                                                  \
    if (it != m_listen_fds.end())                                                            \
    {                                                                                        \
        op = EPOLL_CTL_MOD;                                                                  \
    }                                                                                        \
    epoll_event temp = event->get_Epoll_Event();                                             \
    INFOLOG("epoll_event.events=%d", (int)temp.events);                                      \
    int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &temp);                               \
    if (rt == -1)                                                                            \
    {                                                                                        \
        ERRORLOG("failed epoll_ctl when add fd ,errno=%d,error=%s", errno, strerror(errno)); \
    }                                                                                        \
    m_listen_fds.insert(event->getFd());                                                     \
    DEBUGLOG("add event to set_listen_fd success fd[%d]", event->getFd())
#define DELETE_TO_EPOLL()                                                                         \
    auto it = m_listen_fds.find(event->getFd());                                                  \
    if (it != m_listen_fds.end())                                                                 \
    {                                                                                             \
        epoll_event temp = event->get_Epoll_Event();                                              \
        int rt = epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, event->getFd(), &temp);                     \
        if (rt == -1)                                                                             \
        {                                                                                         \
            ERRORLOG("failed epoll_ctl when add fd, errno=%d, error=%s", errno, strerror(errno)); \
        }                                                                                         \
        DEBUGLOG("delete event success, fd[%d]", event->getFd());                                 \
    }
namespace rpc
{

    // 问题 ：call_once 单例模式
    static thread_local Eventloop *t_current_eventloop = nullptr;
    static int g_epoll_max_timeout = 10000;
    static int g_epoll_max_events = 30;
    Eventloop::Eventloop()
    {
        if (t_current_eventloop != NULL)
        {
            ERRORLOG("failed to create event loop, this thread has created event loop");
            exit(0);
        }
        m_thread_id = getThreadId();

        m_epoll_fd = epoll_create(10);

        if (m_epoll_fd == -1)
        {
            ERRORLOG("failed to create event loop, epoll_create error, error info[%d]", errno);
            exit(0);
        }

        initWakeUpFdEvent();
        initTimer();
        INFOLOG("succ create event loop in thread %d", m_thread_id);
        t_current_eventloop = this;
    }
    void Eventloop::initWakeUpFdEvent()
    {
        m_wakeup_fd = eventfd(0, EFD_NONBLOCK);
        if (m_wakeup_fd < 0)
        {
            ERRORLOG("failed to created wakeup_fd in eventloop,eventfd request failede,error info [%d]", errno);
            exit(0);
        }
        INFOLOG("m_wakeup_fd=%d", m_wakeup_fd);
        m_wakeup_fd_event = new WakeUp_Fd_Event(m_wakeup_fd);
        m_wakeup_fd_event->setListenCallBack(FdEvent::TriggerEvent::IN_EVENT, [this]()
                                             {
            char buf[8];
            while(read(m_wakeup_fd,buf,8)!=-1&&errno!=EAGAIN){}
               DEBUGLOG("read full bytes from wakeup fd[%d]",m_wakeup_fd); });
        addEpollEvent(m_wakeup_fd_event);
    }
    void Eventloop::initTimer()
    {
        m_timer = new Timer();
        addEpollEvent(m_timer);
    }
    Eventloop::~Eventloop()
    {
        close(m_epoll_fd);
        if (m_wakeup_fd_event)
        {
            delete m_wakeup_fd_event;
            m_wakeup_fd_event = nullptr;
        }
        if (m_timer)
        {
            delete m_timer;
            m_timer = nullptr;
        }
    }
    void Eventloop::run()
    {
        while (!m_stop_flag)
        {
            std::queue<std::function<void()>> tmp_taskes;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_pending_tasks.swap(tmp_taskes);
            }
            while (!tmp_taskes.empty())
            {
                std::function<void()> callback = tmp_taskes.front();
                tmp_taskes.pop();
                if (callback)
                    callback();
            }
            int timeout = g_epoll_max_timeout;
            epoll_event result_events[g_epoll_max_events];
            int rt = epoll_wait(m_epoll_fd, result_events, g_epoll_max_events, timeout);
            DEBUGLOG("now end epoll_waik, rt=%d", rt);
            if (rt < 0)
            {
                ERRORLOG("epoll_wait error ,errno=", errno);
            }
            else
            {
                for (int i = 0; i < rt; i++)
                {
                    epoll_event trigger_event = result_events[i];
                    FdEvent *fd_event = static_cast<FdEvent *>(trigger_event.data.ptr);
                    // 在FdEvent设置回调函数的时候，会在epoll_event.daat.ptr里面把Fd_event的指针设置进去，
                    if (fd_event == nullptr)
                    {
                        ERRORLOG("fd_event==nullptr,continue");
                        continue;
                    }
                    if (trigger_event.events & EPOLLIN)
                    {
                        DEBUGLOG("fd %d trigger EPOLLIN event", fd_event->getFd());
                        addTask(fd_event->getCallBackFunc(FdEvent::IN_EVENT));
                    }
                    if (trigger_event.events & EPOLLOUT)
                    {
                        DEBUGLOG("fd %d trigger EPOLLOUT event", fd_event->getFd());
                        addTask(fd_event->getCallBackFunc(FdEvent::OUT_EVENT));
                    }
                }
            }
        }
    }
    void Eventloop::wakeup()
    {
        INFOLOG("WAKE UP");
        m_wakeup_fd_event->wakeup();
    }
    void Eventloop::stop()
    {
        m_stop_flag = true;
    }
    void Eventloop::addEpollEvent(FdEvent *event)
    {
        if (isInLoopThread())
        {
            ADD_TO_EPOLL();
        }
        else
        {
            // 问题
            // 什么情况是线程不对应
            auto cb = [this, event]()
            {
                ADD_TO_EPOLL();
            };
            addTask(cb, true);
        }
    }
    void Eventloop::deleteEpollEvent(FdEvent *event)
    {
        if (isInLoopThread())
        {
            DELETE_TO_EPOLL();
        }
        else
        {
            auto cb = [this, event]()
            {
                DELETE_TO_EPOLL();
            };
            // 为什么设置true
            addTask(cb, true);
        }
    }
    bool Eventloop::isInLoopThread()
    {
        return getThreadId() == m_thread_id;
    }
    void Eventloop::addTask(std::function<void()> callback, bool is_wake_up)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_pending_tasks.push(callback);
        }
        if (is_wake_up)
            wakeup();
    }
    void Eventloop::addTimerEvent(TimerEvent::s_ptr event)
    {
        m_timer->addTimeEvent(event);
    }
    void Eventloop::dealWakeUp()
    {
    }

}
// -194