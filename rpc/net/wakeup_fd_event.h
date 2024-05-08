#pragma once
#include "net/fd_event.h"
namespace rpc
{
    class WakeUp_Fd_Event : public FdEvent
    {
    public:
        WakeUp_Fd_Event(int fd);
        ~WakeUp_Fd_Event();
        void wakeup();

    private:
    };
}
// 24-15
