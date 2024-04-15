#include "wakeup_fd_event.h"
#include <unistd.h>
#include "log.h"
namespace rpc
{
    WakeUp_Fd_Event::WakeUp_Fd_Event(int fd) : FdEvent(fd)
    {
    }
    WakeUp_Fd_Event::~WakeUp_Fd_Event()
    {
    }
    void WakeUp_Fd_Event::wakeup()
    {
        char buf[8]{"wakeup1"};
        int rt = write(m_fd, buf, 8);
        if (rt != 8)
        {
            ERRORLOG("write to wakeup fd less than 8 bytes,fd[%d]", m_fd);
        }
        DEBUGLOG("success wakeup read 8 bytes");
    }
}
