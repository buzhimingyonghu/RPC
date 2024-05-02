#include <string.h>
#include <unistd.h>
#include "tcp_client.h"
#include "log.h"
#include "fd_event_group.h"
#include "error_code.h"
#include <fcntl.h>
namespace rpc
{
    TcpClient::TcpClient(NetAddr::s_ptr peer_addr)
    {
        m_peer_addr = peer_addr;
        m_event_loop = Eventloop::GetCurrentEventloop();
        m_fd = socket(peer_addr->getFamily(), SOCK_STREAM, 0);

        if (m_fd < 0)
        {
            ERRORLOG("TcpClient::TcpClient() error, failed to create fd");
            return;
        }
        m_fd_event = FdEventGroup::GetFdEventGroup()->getFdEvent(m_fd);
        m_fd_event->setNonBlock();

        m_connection = std::make_shared<TcpConnection>(m_event_loop, m_fd, 128, peer_addr, TcpConnectionByClient);
        m_connection->setConnectionType(TcpConnectionByClient);
    }
    TcpClient::~TcpClient()
    {
        DEBUGLOG("TcpClient::~TcpClient()");
        if (m_fd > 0)
        {
            close(m_fd);
        }
    }

    void TcpClient::connect(std::function<void()> done)
    {
        int ret = ::connect(m_fd, m_peer_addr->getSockAddr(), m_peer_addr->getSockLen());
        if (ret == 0)
        {
            DEBUGLOG("connect [%s] sussess", m_peer_addr->toString().c_str());
            m_connection->setState(Connected);
            initLocalAddr();
            if (done)
            {
                done();
            }
        }
        else if (ret == -1)
        {
            /*如果在非阻塞套接字上调用 connect()，连接操作可能会立即返回，并且连接过程会在后台进行。
            这种情况下，套接字可能会在连接尚未完成时变为可写状态，
            因为操作系统认为此时可以向目标主机发送连接请求了。*/
            if (errno == EINPROGRESS)
            {
                m_fd_event->setListenCallBack(FdEvent::OUT_EVENT, [this, done]()
                                              {
                                                  int rt = ::connect(m_fd, m_peer_addr->getSockAddr(), m_peer_addr->getSockLen());
                                                  if ((rt < 0 && errno == EISCONN) || (rt == 0))
                                                  {
                                                      DEBUGLOG("connect [%s] sussess", m_peer_addr->toString().c_str());
                                                      initLocalAddr();
                                                      m_connection->setState(Connected);
                                                  }
                                                  else
                                                  {
                                                      if (errno == ECONNREFUSED)
                                                      {
                                                          m_connect_error_code = ERROR_PEER_CLOSED;
                                                          m_connect_error_info = "connect refused, sys error = " + std::string(strerror(errno));
                                                      }
                                                      else
                                                      {
                                                          m_connect_error_code = ERROR_FAILED_CONNECT;
                                                          m_connect_error_info = "connect unkonwn error, sys error = " + std::string(strerror(errno));
                                                      }
                                                      ERRORLOG("connect errror, errno=%d, error=%s", errno, strerror(errno));
                                                      close(m_fd);
                                                      m_fd = socket(m_peer_addr->getFamily(), SOCK_STREAM, 0);
                                                      
                                                  }
                                                  // 连接完后需要去掉可写事件的监听，不然会一直触发
                                                  m_event_loop->deleteEpollEvent(m_fd_event);
                                                  DEBUGLOG("now begin to done");
                                                  
                                                 

                                                  // 如果连接完成，才会执行回调函数
                                                  if (done)
                                                  {
                                                      done();
                                                  } });
                m_event_loop->addEpollEvent(m_fd_event);
                if (!m_event_loop->getIsLooping())
                {
                    m_event_loop->run();
                }
            }
            else
            {
                ERRORLOG("connect errror, errno=%d, error=%s", errno, strerror(errno));
                m_connect_error_code = ERROR_FAILED_CONNECT;
                m_connect_error_info = "connect error, sys error = " + std::string(strerror(errno));
                if (done)
                {
                    done();
                }
            }
        }
    }
    // 异步的发送 message
    // 如果发送 message 成功，会调用 done 函数， 函数的入参就是 message 对象
    void TcpClient::writeMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        // 1. 把 message 对象写入到 Connection 的 buffer, done 也写入
        // 2. 启动 connection 可写事件
        m_connection->pushSendMessage(message, done);
        m_connection->listenWrite();
    }

    // 异步的读取 message
    // 如果读取 message 成功，会调用 done 函数， 函数的入参就是 message 对象
    void TcpClient::readMessage(const std::string &req_id, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        // 1. 监听可读事件
        // 2. 从 buffer 里 decode 得到 message 对象, 判断是否 req_id 相等，相等则读成功，执行其回调
        m_connection->pushReadMessage(req_id, done);
        m_connection->listenRead();
    }
    void TcpClient::stop()
    {
        if (m_event_loop->getIsLooping())
        {
            m_event_loop->stop();
        }
    }
    void TcpClient::initLocalAddr()
    {
        sockaddr_in local_addr;
        socklen_t len = sizeof(local_addr);
        int ret = getsockname(m_fd, reinterpret_cast<sockaddr *>(&local_addr), &len);
        if (ret != 0)
        {
            ERRORLOG("initLocalAddr error, getsockname error. errno=%d, error=%s", errno, strerror(errno));
            return;
        }

        m_local_addr = std::make_shared<IPNetAddr>(local_addr);
    }
}
// 103