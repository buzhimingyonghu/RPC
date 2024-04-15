#include "config.h"
#include <tinyxml.h>
#include <iostream>
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
#define READ_STR_FROM_XML_NODE(name, parent)                                 \
    TiXmlElement *name##_node = parent->FirstChildElement(#name);            \
    if (!name##_node || !name##_node->GetText())                             \
    {                                                                        \
        std::cout << "Start rocket server error, failed to read config file" \
                  << "[" << #name << "]" << std::endl;                       \
        exit(0);                                                             \
    }                                                                        \
    std::string name##_str = std::string(name##_node->GetText());

namespace rpc
{
    // 全局配置对象
    static Config *g_config = nullptr;
    Config::Config(const char *xmlfile)
    {
        TiXmlDocument *xml_document = new TiXmlDocument();
        bool rt = xml_document->LoadFile(xmlfile);
        if (!rt)
        {
            std::cout << "Start rocket server error,failed to read config file" << xmlfile
                      << ",error info[" << xml_document->ErrorDesc() << "]" << std::endl;
            exit(0);
        }
        READ_XML_NODE(root, xml_document);
        READ_XML_NODE(log, root_node);
        READ_STR_FROM_XML_NODE(log_level, log_node);

        m_log_level = log_level_str;
    }

    Config *Config::GetGlobalConfig()
    {
        return g_config;
    }
    void Config::SetGlobalConfig(const char *xmlfile)
    {
        if (!g_config)
            g_config = new Config(xmlfile);
    }
}
