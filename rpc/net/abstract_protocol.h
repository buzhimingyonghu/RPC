#pragma once
#include <memory>
#include <string>
namespace rpc
{
    /*
    作用：用于表示通信协议中的消息或请求
    该抽象协议类为具体的通信协议提供了一个基础框架，派生类可以根据具体的通信需求实现不同的协议。
    例如，可以创建具体的RPC请求和响应类，并根据需要实现相应的功能，如序列化、反序列化、验证等。*/
    class AbstractProtocol : public std::enable_shared_from_this<AbstractProtocol>
    {
    public:
        typedef std::shared_ptr<AbstractProtocol> s_ptr;
        std::string getReqId()
        {
            return m_req_id;
        }
        void setReqId(const std::string &req_id)
        {
            m_req_id = req_id;
        }
        virtual ~AbstractProtocol() {}

    protected:
        std::string m_req_id;
    };

} // namespace rpc