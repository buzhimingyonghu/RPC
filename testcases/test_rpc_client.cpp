
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
void test_tcp_client()
{

    rpc::IPNetAddr::s_ptr addr = std::make_shared<rpc::IPNetAddr>("127.0.0.1", 12346);
    rpc::TcpClient client(addr);
    client.connect([addr, &client]()
                   {
    DEBUGLOG("conenct to [%s] success", addr->toString().c_str());
    std::shared_ptr<rpc::TinyPBProtocol> message = std::make_shared<rpc::TinyPBProtocol>();
    message->m_req_id = "99998888";
    message->m_pb_data = "test pb data";

    makeOrderRequest request;
    request.set_price(100);
    request.set_goods("apple");
    
    if (!request.SerializeToString(&(message->m_pb_data))) {
      ERRORLOG("serilize error");
      return;
    }

    message->m_method_name = "Order.makeOrder";

    client.writeMessage(message, [request](rpc::AbstractProtocol::s_ptr msg_ptr) {
      DEBUGLOG("send message success, request[%s]", request.ShortDebugString().c_str());
    });


    client.readMessage("99998888", [](rpc::AbstractProtocol::s_ptr msg_ptr) {
      std::shared_ptr<rpc::TinyPBProtocol> message = std::dynamic_pointer_cast<rpc::TinyPBProtocol>(msg_ptr);
      DEBUGLOG("req_id[%s], get response %s", message->m_req_id.c_str(), message->m_pb_data.c_str());
      makeOrderResponse response;

      if(!response.ParseFromString(message->m_pb_data)) {
        ERRORLOG("deserialize error");
        return;
      }
      DEBUGLOG("get response success, response[%s]", response.ShortDebugString().c_str());
    }); });
}

int main()
{

    rpc::Config::SetGlobalConfig("../conf/rpc.xml");

    rpc::Logger::InitGlobalLogger();

    test_tcp_client();

    return 0;
}