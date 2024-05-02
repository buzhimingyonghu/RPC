#pragma once
#include <google/protobuf/service.h>
#include <memory>
#include "net_addr.h"
#include "tcp_client.h"

namespace rpc
{

    // RpcChannel 类继承自 google::protobuf::RpcChannel, 提供 RPC 通信的功能。
    class RpcChannel : public google::protobuf::RpcChannel, public std::enable_shared_from_this<RpcChannel>
    {

    public:
        // 定义智能指针类型，方便在类内部和外部使用。
        typedef std::shared_ptr<RpcChannel> s_ptr;
        typedef std::shared_ptr<google::protobuf::RpcController> controller_s_ptr;
        typedef std::shared_ptr<google::protobuf::Message> message_s_ptr;
        typedef std::shared_ptr<google::protobuf::Closure> closure_s_ptr;

    public:
        // 构造函数，接收一个远程地址的智能指针。
        RpcChannel(NetAddr::s_ptr peer_addr);

        // 析构函数，用于在对象生命周期结束时执行清理工作。
        ~RpcChannel();

        // 初始化方法，设置 RPC 通道所需的各种组件。
        void Init(controller_s_ptr controller, message_s_ptr req, message_s_ptr res, closure_s_ptr done);

        // 核心的 RPC 调用方法，根据提供的方法描述符，执行远程调用。
        void CallMethod(const google::protobuf::MethodDescriptor *method,
                        google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                        google::protobuf::Message *response, google::protobuf::Closure *done);

        // 获取当前的 RpcController 对象。
        inline google::protobuf::RpcController *getController()
        {
            return m_controller.get();
        }

        // 获取当前的请求 Message 对象。
        inline google::protobuf::Message *getRequest()
        {
            return m_request.get();
        }

        // 获取当前的响应 Message 对象。
        inline google::protobuf::Message *getResponse()
        {
            return m_response.get();
        }

        // 获取当前的 Closure 对象，用于回调。
        inline google::protobuf::Closure *getClosure()
        {
            return m_closure.get();
        }

        // 获取当前的 TcpClient 对象，用于网络通信。
        inline TcpClient *getTcpClient()
        {
            return m_client.get();
        }

    private:
        // 远程地址的智能指针。
        NetAddr::s_ptr m_peer_addr{nullptr};
        // 本地地址的智能指针。
        NetAddr::s_ptr m_local_addr{nullptr};

        // RpcController 的智能指针，用于控制 RPC 调用。
        controller_s_ptr m_controller{nullptr};
        // 请求消息的智能指针。
        message_s_ptr m_request{nullptr};
        // 响应消息的智能指针。
        message_s_ptr m_response{nullptr};
        // 完成回调的智能指针。
        closure_s_ptr m_closure{nullptr};

        // 标志位，指示 RpcChannel 是否已经初始化。
        bool m_is_init{false};

        // TcpClient 的智能指针，用于建立和管理 TCP 连接。
        TcpClient::s_ptr m_client{nullptr};
    };

} // namespace rpc
