#pragma once
#include <memory>
#include "tcp/net_addr.h"
#include "coder/abstract_protocol.h"
#include "net/eventloop.h"
#include "net/fd_event.h"
#include "tcp/tcp_connection.h"
#include "net/time_event.h"
#include "common/log.h"
namespace rpc
{

    class TcpClient
    {
    public:
        typedef std::shared_ptr<TcpClient> s_ptr;
        /*接收远程服务器的地址信息作为参数，创建一个TCP客户端对象。
        创建套接字并设置为非阻塞模式。
        初始化TCP连接对象*/
        TcpClient(NetAddr::s_ptr peer_addr);

        ~TcpClient();

        /*异步地与远程服务器建立连接。
        如果连接成功，执行回调函数 done。
        如果连接过程中出现错误，处理错误并记录日志。*/
        void connect(std::function<void()> done);

        /*异步地向服务器发送消息。
        将消息对象写入到连接的发送缓冲区，并注册监听连接可写事件。
        如果消息发送成功，执行回调函数 done*/
        void writeMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);

        /*异步地从服务器读取消息。
        注册监听连接可读事件，并将请求ID和相应的回调函数添加到连接对象中。
        当接收到服务器发送的消息并且消息的请求ID与指定的请求ID相同时，执行相应的回调函数 done。*/
        void readMessage(const std::string &req_id, std::function<void(AbstractProtocol::s_ptr)> done);

        void stop();

        inline int getConnectErrorCode()
        {
            return m_connect_error_code;
        }

        inline std::string getConnectErrorInfo()
        {
            return m_connect_error_info;
        }

        inline NetAddr::s_ptr getPeerAddr()
        {
            return m_peer_addr;
        }

        inline NetAddr::s_ptr getLocalAddr()
        {
            return m_local_addr;
        }

        void initLocalAddr();

        inline void addTimeEvent(TimerEvent::s_ptr timer_event)
        {

            m_event_loop->addTimerEvent(timer_event);
        }

    private:
        NetAddr::s_ptr m_peer_addr;
        NetAddr::s_ptr m_local_addr;

        Eventloop *m_event_loop{NULL};

        int m_fd{-1};
        FdEvent *m_fd_event{NULL};

        TcpConnection::s_ptr m_connection;

        int m_connect_error_code{0};
        std::string m_connect_error_info;
    };
}
// 44