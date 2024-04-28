#include <memory>
#include "log.h"
#include "net_addr.h"
#include "config.h"
#include "tcp_server.h"
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
    test_tcp_server();
}