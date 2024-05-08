#include "common/config.h"
#include "common/log.h"
#include "rpc/rpc_channel.h"
#include "rpc/rpc_closure.h"
#include "rpc/rpc_controller.h"
#include "order.pb.h"
void test_tcp_client()
{
  NEWRPCCHANNEL("127.0.0.1:12345", channel);
  NEWMESSAGE(makeOrderRequest, request);
  NEWMESSAGE(makeOrderResponse, response);

  request->set_price(100);
  request->set_goods("apple");

  NEWRPCCONTROLLER(controller);
  controller->SetMsgId("99998888");

  std::shared_ptr<rpc::RpcClosure> closure = std::make_shared<rpc::RpcClosure>([request, response, channel, controller]() mutable
                                                                               {
    if (controller->GetErrorCode() == 0) {
      APPINFOLOG("call rpc success, request[%s], response[%s]", request->ShortDebugString().c_str(), response->ShortDebugString().c_str());
      // 执行业务逻辑
      if (response->order_id() == "xxx") {
        // xx
      }
    } else {
      APPERRORLOG("call rpc failed, request[%s], error code[%d], error info[%s]", 
        request->ShortDebugString().c_str(), 
        controller->GetErrorCode(), 
        controller->GetErrorInfo().c_str());
    }
  
    INFOLOG("now exit eventloop");
    channel.reset(); });

  CALLRPRC(channel, Order_Stub, makeOrder, controller, request, response, closure);
}

int main()
{

  rpc::Config::SetGlobalConfig("../conf/rpc_client.xml");

  rpc::Logger::InitGlobalLogger();

  test_tcp_client();
  INFOLOG("test_rpc_channel end");

  return 0;
}