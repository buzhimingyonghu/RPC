#include "rpc/rpc_dispatcher.h"
#include "common/log.h"
#include "common/config.h"
#include "order.pb.h"
#include <unistd.h>
#include "tcp/tcp_server.h"
#include "tcp/net_addr.h"
class OrderImpl : public Order
{
public:
    void makeOrder(google::protobuf::RpcController *controller,
                   const ::makeOrderRequest *request,
                   ::makeOrderResponse *response,
                   ::google::protobuf::Closure *done)
    {
        APPDEBUGLOG("start sleep 5s");
        sleep(5);
        APPDEBUGLOG("end sleep 5s");
        if (request->price() < 10)
        {
            response->set_ret_code(-1);
            response->set_res_info("short balance");
            return;
        }
        response->set_order_id("20230514");
        APPDEBUGLOG("call makeOrder success");
    }
};

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Start test_rpc_server error, argc not 2 \n");
        printf("Start like this: \n");
        printf("./test_rpc_server ../conf/rpc.xml \n");
        return 0;
    }
    rpc::Config::SetGlobalConfig(argv[1]);

    rpc::Logger::InitGlobalLogger();

    std::shared_ptr<OrderImpl> service = std::make_shared<OrderImpl>();
    rpc::RpcDispatcher::GetRpcDispatcher()->registerService(service);

    rpc::IPNetAddr::s_ptr addr = std::make_shared<rpc::IPNetAddr>("127.0.0.1", rpc::Config::GetGlobalConfig()->m_port);

    rpc::TcpServer tcp_server(addr);

    tcp_server.start();
    return 0;
}