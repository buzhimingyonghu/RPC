#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <memory>

namespace rpc
{
     // 这个类用于封装和管理 IP 网络地址，提供了一系列方法来操作和获取地址信息。
    class NetAddr
    {

    public:
        typedef std::shared_ptr<NetAddr> s_ptr;
        virtual sockaddr *getSockAddr() = 0;
        virtual socklen_t getSockLen() = 0;
        virtual int getFamily() = 0;
        virtual std::string toString() = 0;
        virtual bool checkValid() = 0;
    };
    // 这个类用于封装和管理 IP 网络地址，提供了一系列方法来操作和获取地址信息。
    class IPNetAddr : public NetAddr
    {
    public:
        // 构造函数，接受点分十进制 IP 地址和端口号
        IPNetAddr(const std::string &ip, uint16_t port);

        // 构造函数，接受形如 "IP:Port" 格式的地址字符串
        IPNetAddr(const std::string &addr);

        // 构造函数，接受一个 sockaddr_in 结构体
        IPNetAddr(sockaddr_in addr);

        // 获取保存地址信息的 sockaddr 结构体的指针
        sockaddr *getSockAddr();

        // 获取保存地址信息的 sockaddr 结构体的长度
        socklen_t getSockLen();

        // 获取地址的协议族
        int getFamily();

        // 将地址信息转换成字符串表示
        std::string toString();

        // 检查地址是否有效
        bool checkValid();

        // 析构函数
        ~IPNetAddr();

    private:
        // IP 地址
        std::string m_ip;

        // 端口号
        uint16_t m_port{0};

        // 保存地址信息的 sockaddr_in 结构体
        sockaddr_in m_addr;
    };

}
// 58