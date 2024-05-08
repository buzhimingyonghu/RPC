#include <tinyxml.h>
#include <iostream>
#include "common/config.h"
// 宏定义：读取XML节点，如果节点不存在则输出错误信息并退出程序
#define READ_XML_NODE(name, parent)                                    \
    TiXmlElement *name##_node = parent->FirstChildElement(#name);      \
    if (!name##_node)                                                  \
    {                                                                  \
        std::cout << "Start rpc server error, failed to read XML_node" \
                  << "[" << #name << "]" << std::endl;                 \
        exit(0);                                                       \
    }                                                                  \
// 宏定义：从XML节点中读取字符串，如果节点不存在或者节点的文本为空则输出错误信息并退出程序
#define READ_STR_FROM_XML_NODE(name, parent)                              \
    TiXmlElement *name##_node = parent->FirstChildElement(#name);         \
    if (!name##_node || !name##_node->GetText())                          \
    {                                                                     \
        std::cout << "Start rpc server error, failed to read config file" \
                  << "[" << #name << "]" << std::endl;                    \
        exit(0);                                                          \
    }                                                                     \
    std::string name##_str = std::string(name##_node->GetText());

namespace rpc
{
    // 全局配置对象
    static Config *g_config = nullptr;
    // 获取全局配置对象
    Config *Config::GetGlobalConfig()
    {
        return g_config;
    }
    // 设置全局配置对象
    void Config::SetGlobalConfig(const char *xmlfile)
    {
        if (g_config == NULL)
        {
            if (xmlfile != NULL)
            {
                g_config = new Config(xmlfile);
            }
            else
            {
                g_config = new Config();
            }
        }
    }
    // 默认构造函数，设置日志级别为DEBUG
    Config::Config()
    {
        m_log_level = "DEBUG";
    }
    Config::Config(const char *xmlfile)
    {
        m_xml_document = new TiXmlDocument();
        bool rt = m_xml_document->LoadFile(xmlfile);
        if (!rt)
        {
            std::cout << "Start rpc server error,failed to read config file" << xmlfile
                      << ",error info[" << m_xml_document->ErrorDesc() << "]" << std::endl;
            exit(0);
        }
        // 问题
        //  读取根节点及其子节点
        READ_XML_NODE(root, m_xml_document);
        READ_XML_NODE(log, root_node);
        READ_XML_NODE(server, root_node);

        // 读取日志配置信息
        READ_STR_FROM_XML_NODE(log_level, log_node);
        READ_STR_FROM_XML_NODE(log_file_name, log_node);
        READ_STR_FROM_XML_NODE(log_file_path, log_node);
        READ_STR_FROM_XML_NODE(log_max_file_size, log_node);
        READ_STR_FROM_XML_NODE(log_sync_interval, log_node);

        // 将读取到的日志配置信息存储到成员变量中
        m_log_level = log_level_str;
        m_log_file_name = log_file_name_str;
        m_log_file_path = log_file_path_str;
        m_log_max_file_size = std::atoi(log_max_file_size_str.c_str());
        m_log_sync_inteval = std::atoi(log_sync_interval_str.c_str());

        // 打印日志配置信息
        printf("LOG -- CONFIG LEVEL[%s], FILE_NAME[%s],FILE_PATH[%s] MAX_FILE_SIZE[%d B], SYNC_INTEVAL[%d ms]\n",
               m_log_level.c_str(), m_log_file_name.c_str(), m_log_file_path.c_str(), m_log_max_file_size, m_log_sync_inteval);

        // 读取服务器配置信息
        READ_STR_FROM_XML_NODE(port, server_node);
        READ_STR_FROM_XML_NODE(io_threads, server_node);

        // 将读取到的服务器配置信息存储到成员变量中
        m_port = std::atoi(port_str.c_str());
        m_io_threads = std::atoi(io_threads_str.c_str());

        printf("Server -- PORT[%d], IO Threads[%d]\n", m_port, m_io_threads);
    }
    Config::~Config()
    {
        if (m_xml_document)
        {
            delete m_xml_document;
            m_xml_document = nullptr;
        }
    }

}