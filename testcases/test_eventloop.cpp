#include "eventloop.h"
#include "log.h"
#include "config.h"
#include "fd_event.h"
#include <arpa/inet.h>
#include <string.h>
int main()
{
    rpc::Config::SetGlobalConfig("../conf/rocket.xml");
    rpc::Logger::InitGlobalLogger();
    rpc::Eventloop *eventloop = new rpc::Eventloop();
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        DEBUGLOG("lfd==-1");
        exit(0);
    }
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_port = htons(9999);
    addr.sin_family = AF_INET;
    // inet_aton函数将IP地址字符串"127.0.0.1"转换为二进制表示，并将结果存储在addr.sin_addr中。
    inet_aton("127.0.0.1", &addr.sin_addr);

    int ret = bind(lfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (ret == -1)
    {
        ERRORLOG("bind error");
        exit(0);
    }
    ret = listen(lfd, 100);
    if (ret != 0)
    {
        ERRORLOG("listen error");
        exit(1);
    }
    rpc::FdEvent event(lfd);
    event.setListenCallBack(rpc::FdEvent::IN_EVENT, [lfd]()
                            {
                                sockaddr_in peer_addr;
                                socklen_t peer_addr_len = sizeof(peer_addr);
                                memset(&peer_addr, 0, sizeof(peer_addr));
                                // 这行代码的作用是接受客户端的连接请求，创建一个新的套接字 cfd，并将客户端的地址信息存储在 peer_addr 中。
                                int cfd = accept(lfd, reinterpret_cast<sockaddr *>(&peer_addr), &peer_addr_len);
                                DEBUGLOG("success get client fd[%d], peer addr: [%s:%d]", cfd, inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port)); });
    eventloop->addEpollEvent(&event);
    eventloop->run();
    return 0;
}