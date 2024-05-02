#pragma once

#include <google/protobuf/stubs/callback.h>
#include <functional>

namespace rpc
{
    class RpcClosure : public google::protobuf::Closure
    {
    public:
        RpcClosure(std::function<void()> cb) : m_cb(cb) {}
        void Run() override
        {
            if (m_cb != nullptr)
            {
                m_cb();
            }
        }

    private:
        std::function<void()> m_cb{nullptr};
    };

}