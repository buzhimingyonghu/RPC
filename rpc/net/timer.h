#pragma once
#include <map>
#include <mutex>
#include "fd_event.h"
#include "time_event.h"
namespace rpc
{
    class Timer : public FdEvent
    {
    public:
        Timer();
        ~Timer();
        // 添加定时器事件到事件队列 并设置定时器的到达时间
        void addTimeEvent(TimerEvent::s_ptr event);

        void deleteTimerEvent(TimerEvent::s_ptr event);
        // 当发送了 IO 事件后，eventloop 会执行这个回调函数
        void onTimer();

    private:
        // 设置定时器fd的触发时间
        void resetArriveTime();

    private:
        // 定时器事件队列 存储了所有的定时器事件
        std::multimap<int64_t, TimerEvent::s_ptr> m_pending_events;
        std::mutex m_mutex;
    };
}
// 37