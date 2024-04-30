#pragma once
#include "abstract_coder.h"
#include "abstract_protocol.h"
namespace rpc
{
    class StringProtocol : public AbstractProtocol
    {
    public:
        std::string info;
    };
    class StringCoder : public AbstractCoder
    {
        // 将 message 对象转化为字节流，写入到 buffer
        void encode(std::vector<AbstractProtocol::s_ptr> &messages, TcpBuffer::s_ptr out_buffer)
        {
            for (size_t i = 0; i < messages.size(); i++)
            {
                /*
                dynamic_pointer_cast 是 C++ 标准库中 std::dynamic_pointer_cast 的一部分。
                它用于在智能指针之间进行动态类型转换，特别是在使用 std::shared_ptr 或 std::weak_ptr 等智能指针时。
                与 dynamic_cast 类似，dynamic_pointer_cast 会在运行时执行类型检查，确保安全地将一个指针转换为另一个相关类型的指针。
                比如说，如果你有一个指向基类的 std::shared_ptr，而你知道这个指针实际上指向的是一个派生类的对象，
                你可以使用 dynamic_pointer_cast 来将这个指针转换为指向派生类的 std::shared_ptr，并且在转换失败时返回空指针。*/
                std::shared_ptr<StringProtocol> msg = std::dynamic_pointer_cast<StringProtocol>(messages[i]);
                out_buffer->writeToBuffer(msg->info.c_str(), msg->info.length());
            }
        };
        // 将 buffer 里面的字节流转换为 message 对象
        void decode(std::vector<AbstractProtocol::s_ptr> &out_messages, TcpBuffer::s_ptr buffer)
        {
            std::vector<char> re;
            buffer->readFromBuffer(re, buffer->readAble());
            std::string info;
            for (size_t i = 0; i < re.size(); ++i)
            {
                info += re[i];
            }

            std::shared_ptr<StringProtocol> msg = std::make_shared<StringProtocol>();
            msg->info = info;
            msg->setReqId("123456");
            out_messages.push_back(msg);
        }
    };
} // namespace rpc
