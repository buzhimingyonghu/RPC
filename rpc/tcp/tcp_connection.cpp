#include <unistd.h>
#include "tcp_connection.h"
#include "fd_event_group.h"
#include "log.h"
namespace rpc
{
    TcpConnection::TcpConnection(IOThread *io_thread, int fd, int buffer_size, NetAddr::s_ptr peer_addr)
    {
        m_io_thread = io_thread;
        m_peer_addr = peer_addr;
        m_state = NotConnected;
        m_fd = fd;

        m_in_buffer = std::make_shared<TcpBuffer>(buffer_size);

        m_out_buffer = std::make_shared<TcpBuffer>(buffer_size);

        m_fd_event = FdEventGroup::GetFdEventGroup()->getFdEvent(fd);
        m_fd_event->setNonBlock();
        m_fd_event->setListenCallBack(FdEvent::IN_EVENT, std::bind(&TcpConnection::onRead, this));
        io_thread->getEventLoop()->addEpollEvent(m_fd_event);
    }
    TcpConnection::~TcpConnection()
    {
    }
    void TcpConnection::onRead()
    {
        if (m_state != Connected)
        {
            ERRORLOG("onRead error, client has already disconneced, addr[%s], clientfd[%d]", m_peer_addr->toString().c_str(), m_fd);
            return;
        }
        // 判断是否读完缓冲区
        bool is_read_all = false;
        bool is_close = false;

        while (!is_read_all)
        {
            if (m_in_buffer->writeAble() == 0)
            {
                m_in_buffer->resizeBuffer(2 * m_in_buffer->m_buffer.size());
            }
            int read_count = m_in_buffer->writeAble();
            int write_index = m_in_buffer->writeIndex();

            int rt = read(m_fd, &(m_in_buffer->m_buffer[write_index]), read_count);
            DEBUGLOG("success read %d bytes from addr[%s], client fd[%d]", rt, m_peer_addr->toString().c_str(), m_fd);
            if (rt > 0)
            {
                m_in_buffer->moveWriteIndex(rt);
                // 有可能越界，还没有读完
                if (rt == read_count)
                {
                    continue;
                }
                // 读完了
                else if (rt < read_count)
                {
                    is_read_all = true;
                    break;
                }
            }
            else if (rt == 0)
            {
                is_close = true;
                break;
            }
        }
        if (is_close)
        {
            // TODO:
            INFOLOG("peer closed, peer addr [%s], clientfd [%d]", m_peer_addr->toString().c_str(), m_fd);
            clear();
            return;
        }
        if (!is_read_all)
        {
            ERRORLOG("not read all data");
        }

        // TODO: 简单的 echo, 后面补充 RPC 协议解析
        excute();
    }
    /*
    1. 从输入缓冲区中读取RPC请求数据。
    2. 将读取到的RPC请求数据处理为字符串形式。
    3. 记录日志，记录成功获取到的请求内容以及客户端地址信息。
    4. 将处理后的RPC请求数据写入输出缓冲区。
    5. 将写入操作注册到文件描述符（m_fd_event）对应的事件监听器中，以便在写入就绪时调用回调函数（onWrite）。
    6. 将文件描述符的事件监听器添加到事件循环中，以便在事件发生时进行处理。
    */
    void TcpConnection::excute()
    {
        // 问题，是不是多此一举
        std::vector<char> temp;
        int size = m_in_buffer->readAble();
        temp.resize(size);
        m_in_buffer->readFromBuffer(temp, size);

        std::string msg;
        for (char &c : temp)
        {
            msg += c;
        }
        INFOLOG("success get request[%s] from client[%s]", msg.c_str(), m_peer_addr->toString().c_str());
        m_out_buffer->writeToBuffer(msg.c_str(), msg.size());
        m_fd_event->setListenCallBack(FdEvent::OUT_EVENT, std::bind(&TcpConnection::onWrite, this));
        m_io_thread->getEventLoop()->addEpollEvent(m_fd_event);
    }
    /*
    1. 检查连接状态，如果客户端已断开连接，则记录错误日志并返回。
    2. 循环发送输出缓冲区中的数据，直到全部发送完毕或者发送缓冲区已满。
    3. 如果发送完成，取消写事件监听，并重新添加事件监听器到事件循环中。
    */
    void TcpConnection::onWrite()
    {
        // 将当前 out_buffer 里面的数据全部发送给 client

        if (m_state != Connected)
        {
            ERRORLOG("onWrite error, client has already disconneced, addr[%s], clientfd[%d]", m_peer_addr->toString().c_str(), m_fd);
            return;
        }

        bool is_write_all = false;
        while (true)
        {
            if (m_out_buffer->readAble() == 0)
            {
                DEBUGLOG("no data need to send to client [%s]", m_peer_addr->toString().c_str());
                is_write_all = true;
                break;
            }
            int write_size = m_out_buffer->readAble();
            int read_index = m_out_buffer->readIndex();

            int rt = write(m_fd, &(m_out_buffer->m_buffer[read_index]), write_size);

            if (rt >= write_size)
            {
                DEBUGLOG("no data need to send to client [%s]", m_peer_addr->toString().c_str());
                is_write_all = true;
                break;
            }
            if (rt == -1 && errno == EAGAIN)
            {
                // 发送缓冲区已满，不能再发送了。
                // 这种情况我们等下次 fd 可写的时候再次发送数据即可
                ERRORLOG("write data error, errno==EAGIN and rt == -1");
                break;
            }
        }
        if (is_write_all)
        {
            m_fd_event->cancle(FdEvent::OUT_EVENT);
            m_io_thread->getEventLoop()->addEpollEvent(m_fd_event);
        }
    }
    void TcpConnection::clear()
    {
        // 处理一些关闭连接后的清理动作
        if (m_state == Closed)
        {
            return;
        }
        m_fd_event->cancle(FdEvent::IN_EVENT);
        m_fd_event->cancle(FdEvent::OUT_EVENT);
        m_io_thread->getEventLoop()->deleteEpollEvent(m_fd_event);
        m_state = Closed;
    }
    // 问题 不理解
    void TcpConnection::shutdown()
    {
        if (m_state == Closed || m_state == NotConnected)
        {
            return;
        }

        // 处于半关闭
        m_state = HalfClosing;

        // 调用 shutdown 关闭读写，意味着服务器不会再对这个 fd 进行读写操作了
        // 发送 FIN 报文， 触发了四次挥手的第一个阶段
        // 当 fd 发生可读事件，但是可读的数据为0，即 对端发送了 FIN
        ::shutdown(m_fd, SHUT_RDWR);
    }
}
// 200