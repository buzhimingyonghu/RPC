#include "time_event.h"
#include "util.h"
#include "log.h"
namespace rpc
{
    TimerEvent::TimerEvent(int interval, bool is_repeated, std::function<void()> callback)
    {
        m_interval = interval;
        m_is_repeated = is_repeated;
        m_task = callback;
        resetArriveTime();
    }
    TimerEvent::~TimerEvent()
    {
    }
    void TimerEvent::resetArriveTime()
    {
        m_arrive_time = getNowMs() + m_interval;
        DEBUGLOG("success create timer event, will excute at [%lld]", m_arrive_time);
    }
}
