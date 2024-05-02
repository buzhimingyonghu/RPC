#include <unistd.h>
#include "tcp_connection.h"
#include "fd_event_group.h"
#include "log.h"
#include "string_coder.h"
#include "tinypb_coder.h"
#include "rpc_dispatcher.h"
namespace rpc
{
    TcpConnection::TcpConnection(Eventloop *event_loop, int fd, int buffer_size, NetAddr::s_ptr peer_addr, TcpConnectionType type)
    {
        m_event_loop = event_loop;
        m_peer_addr = peer_addr;
        m_state = NotConnected,
        m_fd = fd;
        m_connection_type = type;

        m_in_buffer = std::make_shared<TcpBuffer>(buffer_size);
        m_out_buffer = std::make_shared<TcpBuffer>(buffer_size);
        m_fd_event = FdEventGroup::GetFdEventGroup()->getFdEvent(fd);
        m_fd_event->setNonBlock();
        m_coder = new TinyPBCoder();

        if (m_connection_type == TcpConnectionByServer)
        {
            listenRead();
        }
    }
    TcpConnection::~TcpConnection()
    {
        DEBUGLOG("~TcpConnection");
        if (m_coder)
        {
            delete m_coder;
            m_coder = NULL;
        }
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
                    DEBUGLOG("read all finish")
                    break;
                }
            }
            else if (rt == 0)
            {
                is_close = true;
                break;
            }
            else if (rt == -1 && errno == EAGAIN)
            {
                is_read_all = true;
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
    execute`函数的主要功能是根据连接类型执行不同操作路径，可能包括读取请求、发送响应或执行回调函数。。
    */
    void TcpConnection::excute()
    {

        if (m_connection_type == TcpConnectionByServer)
        {
            processServerConnection();
        }
        else
        {
            processClientConnection();
        }
    }
    // 从输入缓冲区中读取 RPC 请求消息
    // 将消息写入输出缓冲区
    // 执行写操作或者注册写事件
    void TcpConnection::processServerConnection()
    {
        // 将 RPC 请求执行业务逻辑，获取 RPC 响应, 再把 RPC 响应发送回去
        // 问题，是不是多此一举
        std::vector<AbstractProtocol::s_ptr> result;
        std::vector<AbstractProtocol::s_ptr> replay_messages;

        m_coder->decode(result, m_in_buffer);
        for (size_t i = 0; i < result.size(); i++)
        {
            INFOLOG("success get request[%s] from client[%s]", result[i]->m_req_id.c_str(), m_peer_addr->toString().c_str());
            std::shared_ptr<TinyPBProtocol> message = std::make_shared<TinyPBProtocol>();

            RpcDispatcher::GetRpcDispatcher()->dispatch(result[i], message, this);
            replay_messages.emplace_back(message);
        }
        m_coder->encode(replay_messages, m_out_buffer);
        listenWrite();
    }
    // 从输入缓冲区中解码消息
    // 执行消息对应的回调函数
    void TcpConnection::processClientConnection()
    {
        // 从 buffer 里 decode 得到 message 对象, 执行其回调
        std::vector<AbstractProtocol::s_ptr> result;
        m_coder->decode(result, m_in_buffer);

        for (size_t i = 0; i < result.size(); ++i)
        {
            std::string req_id = result[i]->m_req_id;
            auto it = m_read_dones.find(req_id);
            if (it != m_read_dones.end())
            {
                it->second(result[i]);
            }
        }
    }
    /*
    1. 检查连接状态，如果客户端已断开连接，则记录错误日志并返回。
    2. 循环发送输出缓冲区中的数据，直到全部发送完毕或者发送缓冲区已满。
    3. 如果发送完成，取消写事件监听，并重新添加事件监听器到事件循环中。
    */
    void TcpConnection::onWrite()
    {
        if (m_state != Connected)
        {
            ERRORLOG("onWrite error, client has already disconneced, addr[%s], clientfd[%d]", m_peer_addr->toString().c_str(), m_fd);
            return;
        }
        if (m_connection_type == TcpConnectionByClient)
        {
            std::vector<AbstractProtocol::s_ptr> messages;
            for (size_t i = 0; i < m_write_dones.size(); i++)
            {
                messages.push_back(m_write_dones[i].first);
            }
            m_coder->encode(messages, m_out_buffer);
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
                DEBUGLOG("client success send [%s]", m_peer_addr->toString().c_str());
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
            m_out_buffer->moveReadIndex(rt);
        }
        if (is_write_all)
        {
            m_fd_event->cancle(FdEvent::OUT_EVENT);
            m_event_loop->addEpollEvent(m_fd_event);
        }
        // 问题，为什么最后要执行一下
        if (m_connection_type == TcpConnectionByClient)
        {
            for (size_t i = 0; i < m_write_dones.size(); ++i)
            {
                m_write_dones[i].second(m_write_dones[i].first);
            }
            m_write_dones.clear();
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
        m_event_loop->deleteEpollEvent(m_fd_event);
        m_state = Closed;
    }

    // 问题 不理解
    void TcpConnection::shutdown()
    {
        if (m_state == Closed || m_state == NotConnected)
        {
            return;
        }
        // 问题
        //  处于半关闭
        m_state = HalfClosing;

        // 调用 shutdown 关闭读写，意味着服务器不会再对这个 fd 进行读写操作了
        // 发送 FIN 报文， 触发了四次挥手的第一个阶段
        // 当 fd 发生可读事件，但是可读的数据为0，即 对端发送了 FIN
        ::shutdown(m_fd, SHUT_RDWR);
    }

    void TcpConnection::setConnectionType(TcpConnectionType type)
    {
        m_connection_type = type;
    }
    void TcpConnection::listenWrite()
    {

        m_fd_event->setListenCallBack(FdEvent::OUT_EVENT, std::bind(&TcpConnection::onWrite, this));
        m_event_loop->addEpollEvent(m_fd_event);
    }

    void TcpConnection::listenRead()
    {

        m_fd_event->setListenCallBack(FdEvent::IN_EVENT, std::bind(&TcpConnection::onRead, this));
        m_event_loop->addEpollEvent(m_fd_event);
    }
    void TcpConnection::pushSendMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        m_write_dones.push_back(std::make_pair(message, done));
    }

    void TcpConnection::pushReadMessage(const std::string &req_id, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        m_read_dones.insert(std::make_pair(req_id, done));
    }
}
// 243