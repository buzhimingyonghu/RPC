#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <assert.h>
#include "tcp_acceptor.h"
#include "log.h"

namespace rpc
{
    TcpAcceptor::TcpAcceptor(NetAddr::s_ptr local_addr)
    {
        if (!local_addr->checkValid())
        {
            ERRORLOG("invalid local addr %s", local_addr->toString().c_str());
            exit(0);
        }
        m_local_addr = local_addr;
        m_family = m_local_addr->getFamily();
        m_lfd = socket(m_family, SOCK_STREAM, 0);
        if (m_lfd < 0)
        {
            ERRORLOG("invalid lfd %d", m_lfd);
            exit(0);
        }
        int val = 1;
        if (setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0)
        {
            ERRORLOG("setsockopt reuseaddr error ,errno=%d,error=%s", errno, strerror(errno));
        }
        socklen_t len = m_local_addr->getSockLen();
        if (bind(m_lfd, m_local_addr->getSockAddr(), len) != 0)
        {
            ERRORLOG("bind error ,errno=%d,error=%s", errno, strerror(errno));
            exit(0);
        }
        if (listen(m_lfd, 1000) != 0)
        {
            ERRORLOG("listen error, errno=%d, error=%s", errno, strerror(errno));
            exit(0);
        }
    }

    TcpAcceptor::~TcpAcceptor()
    {
    }
    /*
    @brief 用于接受客户端连接。
    1. 如果地址族为 IPv4（AF_INET）：
    - 创建并初始化一个 sockaddr_in 结构体用于存储客户端地址信息。
    - 调用 accept() 接受客户端连接，获取客户端的文件描述符。
    - 如果 accept() 返回值小于 0，则记录错误日志。
    - 创建一个 IPv4 网络地址对象（IPNetAddr），用客户端地址信息初始化。
    - 记录成功接受客户端连接的日志

    @return 户端文件描述符和地址信息的 pair 对象。如果地址族不是 IPv4，则返回一个错误的 pair 对象（包含文件描述符 -1 和空指针）。*/
    std::pair<int, NetAddr::s_ptr> TcpAcceptor::accept()
    {
        if (m_family == AF_INET)
        {
            sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(client_addr));
            socklen_t clien_addr_len = sizeof(clien_addr_len);

            int client_fd = ::accept(m_lfd, reinterpret_cast<sockaddr *>(&client_addr), &clien_addr_len);
            if (client_fd < 0)
            {
                ERRORLOG("accept error, errno=%d, error=%s", errno, strerror(errno));
            }
            IPNetAddr::s_ptr peer_addr = std::make_shared<IPNetAddr>(client_addr);
            INFOLOG("A client have accpeted succ, peer addr [%s]", peer_addr->toString().c_str());

            return std::make_pair(client_fd, peer_addr);
        }
        else
        {
            // ...
            return std::make_pair(-1, nullptr);
        }
    }

}
// 74