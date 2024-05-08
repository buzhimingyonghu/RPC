#pragma once
#include <functional>
#include <memory>
#include <string>
namespace rpc
{
    class TimerEvent // 基本定时器单位
    {

    public:
        typedef std::shared_ptr<TimerEvent> s_ptr;
        TimerEvent(int interval, bool is_repeated, std::function<void()> callback, const char *info);
        ~TimerEvent();
        inline int64_t getArriveTime() const
        {
            return m_arrive_time;
        }
        inline void setCancled(bool is_cancled)
        {
            m_is_cancled = is_cancled;
        }
        inline bool get_isCancled()
        {
            return m_is_cancled;
        }
        inline bool get_isRepeated()
        {
            return m_is_repeated;
        }
        inline std::function<void()> getCallBack()
        {
            return m_task;
        }
        void resetArriveTime();
        std::string m_help_info;

    private:
        int64_t m_arrive_time;
        int64_t m_interval;
        bool m_is_repeated = (false);
        bool m_is_cancled = (false);
        std::function<void()> m_task;
    };

}
// 52