#pragma once

#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>

#include <string>

#include "net_addr.h"

namespace rpc
{
    /*主要作用是提供了一种机制来跟踪 RPC 调用的状态（例如成功、失败、取消），提供错误处理，
    以及管理 RPC 调用相关的一些配置，如超时时间和地址信息*/
    class RpcController : public google::protobuf::RpcController
    {

    public:
        RpcController() {}
        ~RpcController() {}
        // 重置控制器的状态，通常在重用 RPC 控制器对象时调用。
        void Reset();
        // 当 RPC 调用被取消时，调用指定的回调函数。
        void NotifyOnCancel(google::protobuf::Closure *callback);
        // 返回 RPC 调用是否失败。
        inline bool Failed() const
        {
            return m_is_failed;
        }
        // 返回错误文本信息，如果 RPC 调用失败。
        inline std::string ErrorText() const
        {
            return m_error_info;
        }
        // 开始取消 RPC 调用。
        inline void StartCancel()
        {
            m_is_cancled = true;
        }
        // 设置 RPC 调用失败，并提供失败的原因。
        inline void SetFailed(const std::string &reason)
        {
            m_error_info = reason;
        }
        // 返回 RPC 调用是否被取消
        inline bool IsCanceled() const
        {
            return m_is_cancled;
        }

        // 设置错误代码和错误信息。
        inline void SetError(int32_t error_code, const std::string error_info)
        {
            m_error_code = error_code;
            m_error_info = error_info;
        }

        // 获取错误代码。
        inline int32_t GetErrorCode()
        {
            return m_error_code;
        }
        // 获取错误信息。
        inline std::string GetErrorInfo()
        {
            return m_error_info;
        }
        // 设置请求 ID，用于标识特定的 RPC 调用。
        inline void SetReqId(const std::string &req_id)
        {
            m_req_id = req_id;
        }
        // 获取请求 ID。
        inline std::string GetReqId()
        {
            return m_req_id;
        }
        // 设置发起 RPC 调用的本地地址。
        inline void SetLocalAddr(NetAddr::s_ptr addr)
        {
            m_local_addr = addr;
        }
        // 设置 RPC 调用的对端地址。
        inline void SetPeerAddr(NetAddr::s_ptr addr)
        {
            m_peer_addr = addr;
        }
        // 获取本地地址。

        inline NetAddr::s_ptr GetLocalAddr()
        {
            return m_local_addr;
        }
        // 获取对端地址。
        inline NetAddr::s_ptr GetPeerAddr()
        {
            return m_peer_addr;
        }
        // 设置 RPC 调用的超时时间。
        inline void SetTimeout(int timeout)
        {
            m_timeout = timeout;
        }
        // 获取 RPC 调用的超时时间。
        inline int GetTimeout()
        {
            return m_timeout;
        }

    private:
        int32_t m_error_code{0};
        std::string m_error_info;
        std::string m_req_id;

        bool m_is_failed{false};
        bool m_is_cancled{false};

        NetAddr::s_ptr m_local_addr;
        NetAddr::s_ptr m_peer_addr;

        int m_timeout{1000}; // ms
    };
} // namespace rpc
