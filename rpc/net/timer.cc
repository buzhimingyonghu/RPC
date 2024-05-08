#include <sys/timerfd.h>
#include <string.h>
#include "net/timer.h"
#include "common/log.h"
#include "common/util.h"
namespace rpc
{
    Timer::Timer() : FdEvent()
    {
        m_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
        DEBUGLOG("timer fd=%d", m_fd);
        // 把 fd 可读事件放到了 eventloop 上监听
        setListenCallBack(FdEvent::IN_EVENT, std::bind(&Timer::onTimer, this));
    }
    Timer::~Timer()
    {
    }
    void Timer::addTimeEvent(TimerEvent::s_ptr event)
    {
        bool is_reset_timerfd = false;
        m_mutex.lock();
        if (m_pending_events.empty())
        {
            is_reset_timerfd = true;
        }
        else
        {
            auto it = m_pending_events.begin();
            if ((*it).second->getArriveTime() > event->getArriveTime())
            {
                is_reset_timerfd = true;
            }
        }
        m_pending_events.emplace(event->getArriveTime(), event);
        m_mutex.unlock();
        if (is_reset_timerfd)
        {
            resetArriveTime();
        }
    }
    void Timer::deleteTimerEvent(TimerEvent::s_ptr event)
    {
        event->setCancled(true);
        m_mutex.lock();
        auto begin = m_pending_events.lower_bound(event->getArriveTime());
        auto end = m_pending_events.upper_bound(event->getArriveTime());
        for (; begin != end; begin++)
        {
            if (begin->second == event)
            {
                m_pending_events.erase(begin);
            }
        }
        m_mutex.unlock();
        DEBUGLOG("success delete TimerEvent at arrive time %lld", event->getArriveTime());
    }
    void Timer::onTimer()
    {
        char buf[8];
        while (1)
        {
            if ((read(m_fd, buf, 8) == -1) && errno == EAGAIN)
            {
                break;
            }
        }
        int64_t now = getNowMs();
        std::vector<TimerEvent::s_ptr> repeated_event;                        // 设置重复定时器事件，下一次执行
        std::vector<std::pair<int64_t, std::function<void()>>> execute_tasks; // 执行的事件
        m_mutex.lock();
        auto it = m_pending_events.begin();
        for (; it != m_pending_events.end(); ++it)
        {
            if ((*it).first <= now)
            {
                if (!(*it).second->get_isCancled())
                {
                    repeated_event.push_back((*it).second);
                    execute_tasks.push_back({(*it).second->getArriveTime(), (*it).second->getCallBack()});
                    DEBUGLOG("trigger time_event=%s", (*it).second->m_help_info.c_str());
                }
            }
            else
            {
                break;
            }
        }
        m_pending_events.erase(m_pending_events.begin(), it);

        m_mutex.unlock();
        for (auto &it : repeated_event)
        {
            if (it->get_isRepeated())
            {
                DEBUGLOG("reset repeated time_event=%s", it->m_help_info.c_str())
                it->resetArriveTime();
                addTimeEvent(it);
            }
        }
        for (auto &it : execute_tasks)
        {
            if (it.second)
            {
                it.second();
            }
        }
    }

    void Timer::resetArriveTime()
    {
        m_mutex.lock();
        auto temp = m_pending_events;
        m_mutex.unlock();

        if (temp.size() == 0)
            return;
        int64_t now = getNowMs();
        auto it = temp.begin();

        int64_t inteval = 0; // 毫秒计数
        int64_t garrtime = it->second->getArriveTime();
        if (garrtime > now)
        {
            inteval = garrtime - now;
        }
        else
        {
            inteval = 100;
        }

        timespec ts;
        memset(&ts, 0, sizeof(ts));
        ts.tv_sec = inteval / 1000;
        ts.tv_nsec = (inteval % 1000) * 1000000;

        itimerspec value;
        memset(&value, 0, sizeof(value));
        value.it_value = ts;
        int ret = timerfd_settime(m_fd, 0, &value, NULL);
        if (ret != 0)
        {
            ERRORLOG("timerfd_settime error, errno=%d, error=%s", errno, strerror(errno));
        }
        DEBUGLOG("timer reset to %lld", now + inteval);
    }
}
// 163
