#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "rpc/rpc_channel.h"
#include "rpc/rpc_controller.h"
#include "coder/tinypb_protocol.h"
#include "common/msg_id_util.h"
#include "common/error_code.h"
#include "common/log.h"

namespace rpc
{

#define WRITEDONE()                                                                                                                       \
    [req_protocol, channel, my_controller](AbstractProtocol::s_ptr) mutable                                                               \
    {                                                                                                                                     \
        /*写消息成功后的回调*/                                                                                                   \
        /* 记录发送RPC请求成功的日志*/                                                                                         \
        INFOLOG("%s | send rpc request success. call method name[%s], peer addr[%s], local addr[%s]",                                     \
                req_protocol->m_msg_id.c_str(), req_protocol->m_method_name.c_str(),                                                      \
                channel->getTcpClient()->getPeerAddr()->toString().c_str(), channel->getTcpClient()->getLocalAddr()->toString().c_str()); \
        /* 读取TCP客户端的响应消息*/                                                                                            \
        channel->getTcpClient()->readMessage(req_protocol->m_msg_id, READDONE());                                                         \
    }

#define READDONE()                                                                                                                        \
    [channel, my_controller](AbstractProtocol::s_ptr msg) mutable {                                                                       \
        /*读取消息成功后的回调*/                                                                                                \
        /*将响应消息转换为TinyPBProtocol类型*/                                                                                  \
        std::shared_ptr<TinyPBProtocol> rsp_protocol = std::dynamic_pointer_cast<TinyPBProtocol>(msg);                                    \
        /*记录成功获取RPC响应的日志*/                                                                                          \
        INFOLOG("%s | success get rpc response, call method name[%s], peer addr[%s], local addr[%s]",                                     \
                rsp_protocol->m_msg_id.c_str(), rsp_protocol->m_method_name.c_str(),                                                      \
                channel->getTcpClient()->getPeerAddr()->toString().c_str(), channel->getTcpClient()->getLocalAddr()->toString().c_str()); \
        /*当成功读取到回包后， 取消定时任务*/                                                                             \
        channel->getTimeEvent()->setCancled(true);                                                                                        \
        /*反序列化*/                                                                                                                  \
        if (!(channel->getResponse()->ParseFromString(rsp_protocol->m_pb_data)))                                                          \
        {                                                                                                                                 \
            /*如果解析出错，记录错误日志并设置控制器错误*/                                                           \
            ERRORLOG("%s | serialize error", rsp_protocol->m_msg_id.c_str());                                                             \
            my_controller->SetError(ERROR_FAILED_SERIALIZE, "serialize error");                                                           \
            return;                                                                                                                       \
        }                                                                                                                                 \
        /*检查响应协议中是否有错误码*/                                                                                       \
        if (rsp_protocol->m_err_code != 0)                                                                                                \
        {                                                                                                                                 \
            /* 如果有错误码，记录错误日志并设置控制器错误*/                                                          \
            ERRORLOG("%s | call rpc method[%s] failed, error code[%d], error info[%s]",                                                   \
                     rsp_protocol->m_msg_id.c_str(), rsp_protocol->m_method_name.c_str(),                                                 \
                     rsp_protocol->m_err_code, rsp_protocol->m_err_info.c_str());                                                         \
            my_controller->SetError(rsp_protocol->m_err_code, rsp_protocol->m_err_info);                                                  \
            return;                                                                                                                       \
        }                                                                                                                                 \
        /* 如果一切正常，记录成功的日志*/                                                                                   \
        INFOLOG("%s | call rpc success, call method name[%s], peer addr[%s], local addr[%s]",                                             \
                rsp_protocol->m_msg_id.c_str(), rsp_protocol->m_method_name.c_str(),                                                      \
                channel->getTcpClient()->getPeerAddr()->toString().c_str(), channel->getTcpClient()->getLocalAddr()->toString().c_str()); \
        /* 如果存在完成回调，执行它*/                                                                                         \
        if (!my_controller->IsCanceled() && channel->getClosure())                                                                        \
        {                                                                                                                                 \
            channel->getClosure()->Run();                                                                                                 \
        }                                                                                                                                 \
        channel.reset();                                                                                                                  \
    }
    RpcChannel::RpcChannel(NetAddr::s_ptr peer_addr)
    {
        m_peer_addr = peer_addr;
        m_client = std::make_shared<TcpClient>(m_peer_addr);
    }
    RpcChannel::~RpcChannel()
    {
        INFOLOG("~RpcChannel");
    }
    void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method, google::protobuf::RpcController *controller, const google::protobuf::Message *request, google::protobuf::Message *response, google::protobuf::Closure *done)
    {
        std::shared_ptr<TinyPBProtocol> req_protocol = std::make_shared<TinyPBProtocol>();
        RpcController *my_controller = dynamic_cast<RpcController *>(controller);
        if (my_controller == NULL)
        {
            ERRORLOG("failed callmethod, RpcController convert error");
            return;
        }
        if (my_controller->GetMsgId().empty())
        {
            req_protocol->m_msg_id = MsgIDUtil::GenMsgID();
            my_controller->SetMsgId(req_protocol->m_msg_id);
        }
        else
        {
            req_protocol->m_msg_id = my_controller->GetMsgId();
        }

        req_protocol->m_method_name = method->full_name();
        INFOLOG("%s | call method name [%s]", req_protocol->m_msg_id.c_str(), req_protocol->m_method_name.c_str());
        // 如果RpcChannel没有初始化，则记录错误并返回。
        if (!m_is_init)
        {

            std::string err_info = "RpcChannel not init";
            my_controller->SetError(ERROR_RPC_CHANNEL_INIT, err_info);
            ERRORLOG("%s | %s, RpcChannel not init ", req_protocol->m_msg_id.c_str(), err_info.c_str());
            return;
        }

        // requeset 的序列化
        if (!request->SerializeToString(&(req_protocol->m_pb_data)))
        {
            std::string err_info = "failde to serialize";
            my_controller->SetError(ERROR_FAILED_SERIALIZE, err_info);
            ERRORLOG("%s | %s, origin requeset [%s] ", req_protocol->m_msg_id.c_str(), err_info.c_str(), request->ShortDebugString().c_str());
            return;
        }
        // 获取当前对象的智能指针：使用shared_from_this()获取当前对象的智能指针，以确保在回调中对象的生命周期得到管理
        // 问题
        s_ptr channel = shared_from_this();

        m_timer_event = std::make_shared<TimerEvent>(
            my_controller->GetTimeout(), false, [my_controller, channel]() mutable
            {
            my_controller->StartCancel();
            my_controller->SetError(ERROR_RPC_CALL_TIMEOUT, "rpc call timeout " + std::to_string(my_controller->GetTimeout()));
            if (channel->getClosure())
            {
            channel->getClosure()->Run();
            }
            channel.reset(); },
            "rpc timeout");
        m_client->addTimeEvent(m_timer_event);
        // 异步连接到远程服务器
        m_client->connect([req_protocol, channel]() mutable
                          {


            // 尝试将channel中的控制器转换为RpcController类型
            RpcController *my_controller = dynamic_cast<RpcController *>(channel->getController());

            // 检查连接是否出错
            if (channel->getTcpClient()->getConnectErrorCode() != 0)
            {
                // 设置错误码和错误信息到控制器
                my_controller->SetError(channel->getTcpClient()->getConnectErrorCode(), channel->getTcpClient()->getConnectErrorInfo());
                // 记录连接错误日志
                ERRORLOG("%s | connect error, error code[%d], error info[%s], peer addr[%s]",
                         req_protocol->m_msg_id.c_str(), my_controller->GetErrorCode(),
                         my_controller->GetErrorInfo().c_str(), channel->getTcpClient()->getPeerAddr()->toString().c_str());
                return; // 如果连接出错，直接返回
            }

            // 记录连接成功的日志
            INFOLOG("%s | connect success, peer addr[%s], local addr[%s]",
                    req_protocol->m_msg_id.c_str(),
                    channel->getTcpClient()->getPeerAddr()->toString().c_str(),
                    channel->getTcpClient()->getLocalAddr()->toString().c_str());

            // 写消息到TCP客户端，发送RPC请求

            channel->getTcpClient()->writeMessage(req_protocol, WRITEDONE()); });
    }
    // 问题
    void RpcChannel::Init(controller_s_ptr controller, message_s_ptr req, message_s_ptr res, closure_s_ptr done)
    {
        if (m_is_init)
        {
            return;
        }
        m_controller = controller;
        m_request = req;
        m_response = res;
        m_closure = done;
        m_is_init = true;
    }
} // namespace rpc
