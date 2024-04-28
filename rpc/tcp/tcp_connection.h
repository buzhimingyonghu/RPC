#pragma once

#include <memory>
#include "net_addr.h"
#include "io_thread.h"
#include "tcp_buffer.h"
namespace rpc
{
    enum TcpState
    {
        NotConnected = 1,
        Connected = 2,
        HalfClosing = 3,
        Closed = 4,
    };
    class TcpConnection
    {
    public:
        typedef std::shared_ptr<TcpConnection> s_ptr;

    public:
        TcpConnection(IOThread *io_thread, int fd, int buffer_size, NetAddr::s_ptr peer_addr);
        ~TcpConnection();

        void onRead();
        void excute();
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

    private:
        IOThread *m_io_thread = nullptr;
        NetAddr::s_ptr m_local_addr;
        NetAddr::s_ptr m_peer_addr;

        TcpBuffer::s_ptr m_in_buffer;
        TcpBuffer::s_ptr m_out_buffer;

        FdEvent *m_fd_event = nullptr;

        TcpState m_state;

        int m_fd = 0;
    };

}
// 66