#include <arpa/inet.h>
#include <memory>
#include <string.h>
#include "util.h"
#include "log.h"
#include "net_addr.h"
#include "tcp_client.h"
#include "abstract_protocol.h"
#include "string_coder.h"
void test_tcp_client()
{
    rpc::IPNetAddr::s_ptr addr = std::make_shared<rpc::IPNetAddr>("127.0.0.1", 12345);
    rpc::TcpClient client(addr);
    client.connect([addr, &client]()
                   {
                          DEBUGLOG("conenct to [%s] success", addr->toString().c_str());
                          std::shared_ptr<rpc::StringProtocol> message = std::make_shared<rpc::StringProtocol>();
                          message->info = "hello rpc";
                          message->setReqId("123456");
                          client.writeMessage(message, [](rpc::AbstractProtocol::s_ptr msg_ptr)
                                              { DEBUGLOG("send message success"); });
                          client.readMessage("123456", [](rpc::AbstractProtocol::s_ptr msg_ptr) {
                          std::shared_ptr<rpc::StringProtocol> message = std::dynamic_pointer_cast<rpc::StringProtocol>(msg_ptr);
                          DEBUGLOG("req_id[%s], get response %s", message->getReqId().c_str(), message->info.c_str());
                          });

                          client.writeMessage(message, [](rpc::AbstractProtocol::s_ptr msg_ptr) {
                          DEBUGLOG("send message 22222 success");
                          }); });
}
int main()
{
    rpc::Config::SetGlobalConfig("../conf/rpc.xml");
    rpc::Logger::InitGlobalLogger();

    test_tcp_client();
    return 0;
}