#pragma once

#include <string>

namespace rpc
{
    class RpcInterface;
    class RunTime
    {

    public:
        inline RpcInterface *getRpcInterface()
        {
            return m_rpc_interface;
        }

    public:
        static RunTime *GetRunTime();

    public:
        std::string m_msgid;
        std::string m_method_name;
        RpcInterface *m_rpc_interface{NULL};
    };
} // namespace rpc
