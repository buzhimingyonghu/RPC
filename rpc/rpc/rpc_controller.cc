#include "rpc/rpc_controller.h"
namespace rpc
{

    void RpcController::Reset()
    {
        m_error_code = 0;
        m_error_info = "";
        m_msg_id = "";
        m_is_failed = false;
        m_is_cancled = false;
        m_local_addr = nullptr;
        m_peer_addr = nullptr;
        m_timeout = 1000; // ms
    }
    void RpcController::NotifyOnCancel(google::protobuf::Closure *callback)
    {
    }

} // namespace rpc
