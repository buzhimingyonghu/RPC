#pragma once
#include "tcp/net_addr.h"

namespace rpc
{
    class TcpAcceptor
    {
    public:
        typedef std::shared_ptr<TcpAcceptor> s_ptr;
        TcpAcceptor(NetAddr::s_ptr local_addr);
        ~TcpAcceptor();
        std::pair<int, NetAddr::s_ptr> accept();
        inline int getListenFd()
        {
            return m_lfd;
        }

    private:
        NetAddr::s_ptr m_local_addr;
        int m_family = -1;
        int m_lfd = -1;
    };
}
// 32