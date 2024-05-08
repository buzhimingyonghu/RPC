#include "net/time_event.h"
#include "common/util.h"
#include "common/log.h"
namespace rpc
{
    TimerEvent::TimerEvent(int interval, bool is_repeated, std::function<void()> callback, const char *info)
    {
        m_interval = interval;
        m_is_repeated = is_repeated;
        m_task = callback;
        resetArriveTime();
        m_help_info = std::string(info);
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
// 20