
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
#include <google/protobuf/service.h>
#include "log.h"
#include "net_addr.h"
#include "config.h"
#include "tcp_server.h"
#include "order.pb.h"
#include "rpc_dispatcher.h"
class OrderImpl : public Order
{
public:
    void makeOrder(google::protobuf::RpcController *controller,
                   const ::makeOrderRequest *request,
                   ::makeOrderResponse *response,
                   ::google::protobuf::Closure *done)
    {
        DEBUGLOG("start 3");
        sleep(3);
        DEBUGLOG("end 3");
        if (request->price() < 10)
        {
            response->set_ret_code(-1);
            response->set_res_info("short balance");
            return;
        }
        response->set_order_id("20230514");
    }
};

void test_tcp_server()
{

    rpc::IPNetAddr::s_ptr addr = std::make_shared<rpc::IPNetAddr>("127.0.0.1", 12345);

    DEBUGLOG("create addr %s", addr->toString().c_str());

    rpc::TcpServer tcp_server(addr);

    tcp_server.start();
}

int main()
{

    rpc::Config::SetGlobalConfig("../conf/rpc.xml");

    rpc::Logger::InitGlobalLogger();

    std::shared_ptr<OrderImpl> service = std::make_shared<OrderImpl>();
    rpc::RpcDispatcher::GetRpcDispatcher()->registerService(service);

    test_tcp_server();

    return 0;
}