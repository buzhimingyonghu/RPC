#pragma once
#include <string>
#include <map>
#include <tinyxml.h>
#include "tcp/net_addr.h"
namespace rpc
{

    // 结构体定义了RPC服务的基本信息
    struct RpcStub
    {
        std::string name;    // 服务名称
        NetAddr::s_ptr addr; // 服务地址
        int timeout{2000};   // 超时时间，默认2000毫秒
    };

    class Config
    {
    public:
        // 构造函数，加载指定的XML配置文件
        Config(const char *xmlfile);

        // 默认构造函数
        Config();

        // 析构函数，释放资源
        ~Config();

    public:
        // 获取全局配置对象
        static Config *GetGlobalConfig();
        // 设置全局配置对象
        static void SetGlobalConfig(const char *xmlfile);

    public:
        std::string m_log_level;     // 日志级别
        std::string m_log_file_name; // 日志文件名
        std::string m_log_file_path; // 日志文件路径
        int m_log_max_file_size{0};  // 日志最大文件大小
        int m_log_sync_inteval{0};   // 日志同步间隔，毫秒

        int m_port{0};       // 服务器端口号
        int m_io_threads{0}; // IO线程数

        TiXmlDocument *m_xml_document{nullptr}; // XML文档对象指针

        std::map<std::string, RpcStub> m_rpc_stubs; // RPC服务信息映射
    };
}