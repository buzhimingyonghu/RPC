#pragma once
#include <sys/epoll.h>
#include <thread>
#include <set>
#include <queue>
#include <functional>
#include <mutex>
#include "fd_event.h"
#include "timer.h"
#include "wakeup_fd_event.h"
namespace rpc
{
    class Eventloop
    {
    public:
        Eventloop();

        ~Eventloop();

        void run();

        void wakeup();

        void stop();
        // 添加事件到epoll树上
        void addEpollEvent(FdEvent *event);

        void deleteEpollEvent(FdEvent *event);
        // 问题
        bool isInLoopThread();
        // 添加需要执行任务函数到任务队列
        void addTask(std::function<void()> callback, bool is_wake_up = false);

        void addTimerEvent(TimerEvent::s_ptr event);

    private:
        void initWakeUpFdEvent();

        void initTimer();
        // 问题
        void dealWakeUp();

    private:
        pid_t m_thread_id{0};
        int m_epoll_fd{0};
        int m_wakeup_fd{0};
        WakeUp_Fd_Event *m_wakeup_fd_event{nullptr};
        bool m_stop_flag = false;
        std::set<int> m_listen_fds;                        // 存储目前epoll 树上的fd
        std::queue<std::function<void()>> m_pending_tasks; // 任务队列
        std::mutex m_mutex;
        Timer *m_timer{nullptr};
    };
}
// 62-49