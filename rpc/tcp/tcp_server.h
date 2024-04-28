#pragma once
#include <set>
#include "net_addr.h"
#include "tcp_acceptor.h"
#include "eventloop.h"
#include "io_thread_pool.h"
#include "tcp_connection.h"
namespace rpc
{
    class TcpServer
    {
    public:
        TcpServer(NetAddr::s_ptr local_addr);
        ~TcpServer();
        void start();

    private:
        void init();
        void onAccept();

    private:
        TcpAcceptor::s_ptr m_acceptor;
        NetAddr::s_ptr m_local_addr;
        Eventloop *m_main_eventloop = nullptr;
        IOThreadPoll *m_io_thread_pool = nullptr;
        FdEvent *m_listen_fd_event;
        int m_client_counts = 0;
        std::set<TcpConnection::s_ptr> m_client;
    };

} // namespace rpc
  // 48