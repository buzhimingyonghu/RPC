#pragma once
#include <string>

namespace rpc
{

    class Config
    {
    public:
        // 构造函数：根据XML文件初始化配置对象
        Config(const char *xmlfile);

    public:
        static Config *GetGlobalConfig();
        static void SetGlobalConfig(const char *xmlfile);

    public:
        std::string m_log_level;
    };
}
// 27-21