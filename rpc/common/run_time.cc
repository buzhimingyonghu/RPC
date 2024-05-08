#include "common/run_time.h"

namespace rpc
{
    thread_local RunTime *t_run_time = nullptr;
    RunTime *RunTime::GetRunTime()
    {
        if (t_run_time == nullptr)
        {
            t_run_time = new RunTime();
        }
        return t_run_time;
    }

} // namespace rpc
