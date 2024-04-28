#include "tcp_server.h"

namespace rpc
{
    TcpServer::TcpServer(NetAddr::s_ptr local_addr)
    {
        m_local_addr = local_addr;
        init();
        INFOLOG("rocket TcpServer listen sucess on [%s]", m_local_addr->toString().c_str());
    }

    TcpServer::~TcpServer()
    {
        if (m_main_eventloop)
        {
            delete m_main_eventloop;
            m_main_eventloop = nullptr;
        }
    }
    void TcpServer::init()
    {
        m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr);

        m_main_eventloop = Eventloop::GetCurrentEventloop();
        m_io_thread_pool = new IOThreadPoll(2);
        m_listen_fd_event = new FdEvent(m_acceptor->getListenFd());
        m_listen_fd_event->setListenCallBack(FdEvent::IN_EVENT, std::bind(&TcpServer::onAccept, this));
        m_main_eventloop->addEpollEvent(m_listen_fd_event);
    }
    void TcpServer::start()
    {
        m_io_thread_pool->start();
        m_main_eventloop->run();
    }
    /*
    @brief lfd的回调函数， 接受客户端连接，分配线程和tcpconnection去处理事件。
    */
    void TcpServer::onAccept()
    {
        auto re = m_acceptor->accept();
        int client_fd = re.first;
        NetAddr::s_ptr peer_addr = re.second;

        m_client_counts++;

        // 把 cleintfd 添加到任意 IO 线程里面
        IOThread *io_thread = m_io_thread_pool->getIOThread();
        TcpConnection::s_ptr connetion = std::make_shared<TcpConnection>(io_thread, client_fd, 128, peer_addr);
        connetion->setState(Connected);

        m_client.insert(connetion);

        INFOLOG("TcpServer succ get client, fd=%d", client_fd);
    }
} // namespace rpc
  // 62