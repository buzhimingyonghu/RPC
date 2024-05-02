#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "rpc_dispatcher.h"
#include "tinypb_protocol.h"
#include "error_code.h"
#include "log.h"
#include "rpc_controller.h"
#include "tcp_connection.h"
#include "net_addr.h"
namespace rpc
{
    static RpcDispatcher *g_rpc_dispatcher = nullptr;
    RpcDispatcher *RpcDispatcher::GetRpcDispatcher()
    {
        if (g_rpc_dispatcher == nullptr)
        {
            g_rpc_dispatcher = new RpcDispatcher;
        }
        return g_rpc_dispatcher;
    }

    void RpcDispatcher::dispatch(AbstractProtocol::s_ptr request, AbstractProtocol::s_ptr response, TcpConnection *connection)
    {
        /*类型转换：函数开始时，尝试将传入的 request 和 response 指针从基类类型 AbstractProtocol 转换为 TinyPBProtocol 类型。
        TinyPBProtocol 是一个用于序列化和反序列化 Protocol Buffers 消息的协议实现。*/
        std::shared_ptr<TinyPBProtocol> req_protocol = std::dynamic_pointer_cast<TinyPBProtocol>(request);
        std::shared_ptr<TinyPBProtocol> rsp_protocol = std::dynamic_pointer_cast<TinyPBProtocol>(response);

        /*提取方法全名：从请求协议中提取方法的全名（通常包含服务名和方法名），并将其存储在 method_full_name 变量中。*/
        std::string method_full_name = req_protocol->m_method_name;
        std::string service_name;
        std::string method_name;
        // 问题
        rsp_protocol->m_req_id = req_protocol->m_req_id;
        rsp_protocol->m_method_name = req_protocol->m_method_name;

        // 解析服务名和方法名：通过调用 parseServiceFullName 函数，将 method_full_name 解析为服务名 service_name 和方法名 method_name。
        if (!parseServiceFullName(method_full_name, service_name, method_name))
        {
            setTinyPBError(rsp_protocol, ERROR_PARSE_SERVICE_NAME, "parse service name error");
            return;
        }

        // 查找服务：通过服务名在 m_service_map 中查找对应的服务实例。
        auto it = m_service_map.find(service_name);
        if (it == m_service_map.end())
        {
            ERRORLOG("%s | sericve neame[%s] not found", req_protocol->m_req_id.c_str(), service_name.c_str());
            setTinyPBError(rsp_protocol, ERROR_SERVICE_NOT_FOUND, "service not found");
            return;
        }

        service_s_ptr service = (*it).second;
        // 查找方法：在找到的服务中查找对应的方法。
        const google::protobuf::MethodDescriptor *method = service->GetDescriptor()->FindMethodByName(method_name);
        if (method == NULL)
        {
            ERRORLOG("%s | method neame[%s] not found in service[%s]", req_protocol->m_req_id.c_str(), method_name.c_str(), service_name.c_str());
            setTinyPBError(rsp_protocol, ERROR_SERVICE_NOT_FOUND, "method not found");
            return;
        }
        // 反序列化请求消息：尝试将请求协议中的 Protocol Buffers 数据反序列化为 Protocol Buffers 消息对象 req_msg。
        google::protobuf::Message *req_msg = service->GetRequestPrototype(method).New();

        // 反序列化，将 pb_data 反序列化为 req_msg
        if (!req_msg->ParseFromString(req_protocol->m_pb_data))
        {
            ERRORLOG("%s | deserilize error", req_protocol->m_req_id.c_str(), method_name.c_str(), service_name.c_str());
            setTinyPBError(rsp_protocol, ERROR_FAILED_DESERIALIZE, "deserilize error");
            if (req_msg != NULL)
            {
                delete req_msg;
                req_msg = NULL;
            }
            return;
        }
        INFOLOG("%s | get rpc request[%s]", req_protocol->m_req_id.c_str(), req_msg->ShortDebugString().c_str());

        // 创建响应消息：创建一个响应消息对象 rsp_msg
        google::protobuf::Message *rsp_msg = service->GetResponsePrototype(method).New();

        RpcController rpcController;
        rpcController.SetLocalAddr(connection->getLocalAddr());
        rpcController.SetPeerAddr(connection->getPeerAddr());
        rpcController.SetReqId(req_protocol->m_req_id);
        // 调用服务方法：使用 RpcController 对象，调用服务的相应方法，传入请求消息和响应消息对象。
        service->CallMethod(method, &rpcController, req_msg, rsp_msg, NULL);
        // 序列化响应消息：尝试将响应消息对象序列化为 Protocol Buffers 数据，并存储在响应协议中。
        if (!rsp_msg->SerializeToString(&(rsp_protocol->m_pb_data)))
        {
            ERRORLOG("%s | serilize error, origin message [%s]", req_protocol->m_req_id.c_str(), rsp_msg->ShortDebugString().c_str());
            setTinyPBError(rsp_protocol, ERROR_SERVICE_NOT_FOUND, "serilize error");
            return;

            if (req_msg != NULL)
            {
                delete req_msg;
                req_msg = NULL;
            }
            if (rsp_msg != NULL)
            {
                delete rsp_msg;
                rsp_msg = NULL;
            }
        }

        rsp_protocol->m_err_code = 0;
        INFOLOG("%s | dispatch success, requesut[%s], response[%s]", req_protocol->m_req_id.c_str(), req_msg->ShortDebugString().c_str(), rsp_msg->ShortDebugString().c_str());
        // 清理：最后，删除之前创建的 Protocol Buffers 消息对象，以释放资源。
        delete req_msg;
        delete rsp_msg;
        req_msg = NULL;
        rsp_msg = NULL;
    }
    void RpcDispatcher::registerService(service_s_ptr service)
    {
        std::string service_name = service->GetDescriptor()->full_name();
        m_service_map[service_name] = service;
    }
    void RpcDispatcher::setTinyPBError(std::shared_ptr<TinyPBProtocol> msg, int32_t err_code, const std::string err_info)
    {
        msg->m_err_code = err_code;
        msg->m_err_info = err_info;
        msg->m_err_info_len = err_info.length();
    }
    bool RpcDispatcher::parseServiceFullName(const std::string &full_name, std::string &service_name, std::string &method_name)
    {

        if (full_name.empty())
        {
            ERRORLOG("full name empty");
            return false;
        }
        size_t i = full_name.find_first_of(".");
        if (i == full_name.npos)
        {
            ERRORLOG("not find . in full name [%s]", full_name.c_str());
            return false;
        }
        service_name = full_name.substr(0, i);
        method_name = full_name.substr(i + 1, full_name.length() - i - 1);

        INFOLOG("parse sericve_name[%s] and method_name[%s] from full name [%s]", service_name.c_str(), method_name.c_str(), full_name.c_str());

        return true;
    }
} // namespace rpc
