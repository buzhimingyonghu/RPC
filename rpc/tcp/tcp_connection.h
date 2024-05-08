#pragma once

#include <memory>
#include "tcp/net_addr.h"
#include "net/io_thread.h"
#include "coder/abstract_coder.h"
#include "net/eventloop.h"
#include "tcp/tcp_buffer.h"
namespace rpc
{
    enum TcpState
    {
        NotConnected = 1,
        Connected = 2,
        HalfClosing = 3,
        Closed = 4,
    };

    enum TcpConnectionType
    {
        TcpConnectionByServer = 1, // 作为服务端使用，代表跟对端客户端的连接
        TcpConnectionByClient = 2, // 作为客户端使用，代表跟对赌服务端的连接
    };
    class TcpConnection
    {
    public:
        typedef std::shared_ptr<TcpConnection> s_ptr;

    public:
        TcpConnection(Eventloop *event_loop, int fd, int buffer_size, NetAddr::s_ptr peer_addr, TcpConnectionType type = TcpConnectionByServer);
        ~TcpConnection();

        void onRead();
        void excute();

        void processServerConnection();

        void processClientConnection();

        void onWrite();
        inline void setState(const TcpState state)
        {
            m_state = Connected;
        }

        inline TcpState getState()
        {
            return m_state;
        }

        void clear();

        void shutdown();
        void setConnectionType(TcpConnectionType type);

        // 启动监听可写事件
        void listenWrite();

        // 启动监听可读事件
        void listenRead();

        void pushSendMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);

        void pushReadMessage(const std::string &req_id, std::function<void(AbstractProtocol::s_ptr)> done);
        inline NetAddr::s_ptr getLocalAddr()
        {
            return m_local_addr;
        }

        inline NetAddr::s_ptr getPeerAddr()
        {
            return m_peer_addr;
        }

    private:
        Eventloop *m_event_loop = nullptr;
        NetAddr::s_ptr m_local_addr;
        NetAddr::s_ptr m_peer_addr;

        TcpBuffer::s_ptr m_in_buffer;
        TcpBuffer::s_ptr m_out_buffer;

        FdEvent *m_fd_event = nullptr;

        TcpState m_state;

        AbstractCoder *m_coder{NULL};

        int m_fd{0};

        TcpConnectionType m_connection_type{TcpConnectionByServer};

        // std::pair<AbstractProtocol::s_ptr, std::function<void(AbstractProtocol::s_ptr)>>
        // 将message消息存入，触发写事件后，写回调将处理所有的事件并触发，message对应的回调，一般是日志信息
        std::vector<std::pair<AbstractProtocol::s_ptr, std::function<void(AbstractProtocol::s_ptr)>>> m_write_dones;

        // key 为 req_id
        std::map<std::string, std::function<void(AbstractProtocol::s_ptr)>> m_read_dones;
    };

}
// 96