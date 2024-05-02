#pragma once

#include <map>
#include <memory>
#include <google/protobuf/service.h>
#include "abstract_protocol.h"
#include "tinypb_protocol.h"
namespace rpc
{
    class TcpConnection;

    class RpcDispatcher
    {

    public:
        static RpcDispatcher *GetRpcDispatcher();

    public:
        typedef std::shared_ptr<google::protobuf::Service> service_s_ptr;
        // 这个成员函数是调度器的核心，它接收一个请求协议和响应协议的智能指针，以及一个 TcpConnection 类型的指针。
        // 它负责解析请求，找到对应的服务和方法，然后调用该方法，并用返回值填充响应协议。
        void dispatch(AbstractProtocol::s_ptr request, AbstractProtocol::s_ptr response, TcpConnection *connection);
        /*这个成员函数用于向调度器注册一个新的服务。服务是以 std::shared_ptr<google::protobuf::Service> 类型的对象表示的，
        这个类型是 Google Protobuf 库中定义的，用于处理 RPC 服务。*/
        void registerService(service_s_ptr service);
        /* 这个成员函数可能用于设置一个错误信息，当使用 Tiny Protocol Buffers (TinyPB) 协议时，如果发生错误，会调用这个函数来设置错误代码和错误信息。*/
        void setTinyPBError(std::shared_ptr<TinyPBProtocol> msg, int32_t err_code, const std::string err_info);

    private:
        /*这是一个辅助成员函数，用于解析完整的服务名称（如 "Service.Method"），将其拆分为服务名和服务方法名。*/
        bool parseServiceFullName(const std::string &full_name, std::string &service_name, std::string &method_name);

    private:
        /*它存储了服务的完整名称到服务对象的映射。这个映射允许调度器根据服务的名称找到对应的服务实例。*/
        std::map<std::string, service_s_ptr> m_service_map;
    };

}