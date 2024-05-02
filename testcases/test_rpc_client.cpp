
#include <assert.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <memory>
#include <unistd.h>
#include "log.h"
#include "net_addr.h"
#include "config.h"
#include "tcp_server.h"
#include "order.pb.h"
#include "rpc_dispatcher.h"
#include "tcp_client.h"
#include "rpc_channel.h"
#include "rpc_controller.h"
#include "rpc_closure.h"
void test_tcp_client()
{
  rpc::IPNetAddr::s_ptr addr = std::make_shared<rpc::IPNetAddr>("127.0.0.1", 12345);
  std::shared_ptr<rpc::RpcChannel> channel = std::make_shared<rpc::RpcChannel>(addr);

  std::shared_ptr<makeOrderRequest> request = std::make_shared<makeOrderRequest>();
  request->set_price(100);
  request->set_goods("apple");

  std::shared_ptr<makeOrderResponse> response = std::make_shared<makeOrderResponse>();

  std::shared_ptr<rpc::RpcController> controller = std::make_shared<rpc::RpcController>();
  controller->SetMsgId("99998888");

  std::shared_ptr<rpc::RpcClosure> closure = std::make_shared<rpc::RpcClosure>([request, response, channel]() mutable
                                                                               {
    INFOLOG("call rpc success, request[%s], response[%s]", request->ShortDebugString().c_str(), response->ShortDebugString().c_str());
    INFOLOG("now exit eventloop");
    channel->getTcpClient()->stop();
    channel.reset(); });

  channel->Init(controller, request, response, closure);

  Order_Stub stub(channel.get());

  stub.makeOrder(controller.get(), request.get(), response.get(), closure.get());
}

int main()
{

  rpc::Config::SetGlobalConfig("../conf/rpc.xml");

  rpc::Logger::InitGlobalLogger();

  test_tcp_client();
  INFOLOG("test_rpc_channel end");

  return 0;
}