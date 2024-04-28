#include "fd_event_group.h"
#include "log.h"
namespace rpc
{
    static FdEventGroup *g_fd_event_group = nullptr;
    FdEventGroup *FdEventGroup::GetFdEventGroup()
    {
        if (g_fd_event_group == nullptr)
        {
            g_fd_event_group = new FdEventGroup(128);
        }
        return g_fd_event_group;
    }
    FdEventGroup::FdEventGroup(int size)
    {
        m_size = size;
        for (int i = 0; i < m_size; i++)
        {
            m_fd_group.push_back(new FdEvent(i));
        }
    }

    FdEventGroup::~FdEventGroup()
    {
        for (int i = 0; i < m_size; i++)
        {
            if (m_fd_group[i] != nullptr)
            {
                delete m_fd_group[i];
                m_fd_group[i] = nullptr;
            }
        }
    }

    FdEvent *FdEventGroup::getFdEvent(int fd)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if ((size_t)fd < m_fd_group.size())
        {
            return m_fd_group[fd];
        }
        int new_szie = int(fd * 1.5);
        for (int i = m_fd_group.size(); i < new_szie; i++)
        {
            m_fd_group.push_back(new FdEvent(i));
        }
        return m_fd_group[fd];
    }

} // namespace rpc
  // 48