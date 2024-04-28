#pragma once
#include <mutex>
#include <vector>
#include "fd_event.h"

namespace rpc
{
    class FdEventGroup
    {
    public:
        FdEventGroup(int size);
        ~FdEventGroup();

        FdEvent *getFdEvent(int fd);

    public:
        static FdEventGroup *GetFdEventGroup();

    private:
        int m_size = 0;
        std::vector<FdEvent *> m_fd_group;
        std::mutex m_mutex;
    };
} // namespace rpc
  // 31