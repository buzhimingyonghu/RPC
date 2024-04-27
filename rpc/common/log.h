#pragma once
#include <string>
#include <memory>
#include <queue>
#include <iostream>
#include "config.h"
#include <mutex>

namespace rpc
{
#include <cstdio>
#include <string>

    template <typename... Args>
    std::string formatString(const char *str, Args &&...args)
    {
        int size = snprintf(nullptr, 0, str, std::forward<Args>(args)...);

        std::string result;
        if (size > 0)
        {
            result.resize(size);
            snprintf(&result[0], size + 1, str, std::forward<Args>(args)...);
        }
        return result;
    }
    /*Unknown = 0,
        Debug = 1,
        Info = 2,
        Error = 3
    设置全局日志等级，打印小于当前等级的信息
        */
    enum LogLevel
    {
        Unknown = 0,
        Debug = 1,
        Info = 2,
        Error = 3
    };

#define DEBUGLOG(str, args...)                                                                                                                                    \
    if (rpc::Logger::GetGlobalLogger()->getLogLevel() <= rpc::Debug)                                                                                              \
    {                                                                                                                                                             \
        std::string fileInfo = "[" + std::string(__FILE__) + ":" + std::string(__FUNCTION__) + ":" + std::to_string(__LINE__) + "]";                              \
        fileInfo.resize(80, ' ');                                                                                                                                 \
        rpc::Logger::GetGlobalLogger()->pushLog((new rpc::LogEvent(rpc::LogLevel::Debug))->toString() + fileInfo + "\t" + rpc::formatString(str, ##args) + "\n"); \
        rpc::Logger::GetGlobalLogger()->log();                                                                                                                    \
    }
#define INFOLOG(str, args...)                                                                                                                                    \
    if (rpc::Logger::GetGlobalLogger()->getLogLevel() <= rpc::Info)                                                                                              \
    {                                                                                                                                                            \
        std::string fileInfo = "[" + std::string(__FILE__) + ":" + std::string(__FUNCTION__) + ":" + std::to_string(__LINE__) + "]";                             \
        fileInfo.resize(80, ' ');                                                                                                                                \
        rpc::Logger::GetGlobalLogger()->pushLog((new rpc::LogEvent(rpc::LogLevel::Info))->toString() + fileInfo + "\t" + rpc::formatString(str, ##args) + "\n"); \
        rpc::Logger::GetGlobalLogger()->log();                                                                                                                   \
    }

#define ERRORLOG(str, args...)                                                                                                                                    \
    if (rpc::Logger::GetGlobalLogger()->getLogLevel() <= rpc::Error)                                                                                              \
    {                                                                                                                                                             \
        std::string fileInfo = "[" + std::string(__FILE__) + ":" + std::string(__FUNCTION__) + ":" + std::to_string(__LINE__) + "]";                              \
        fileInfo.resize(80, ' ');                                                                                                                                 \
        rpc::Logger::GetGlobalLogger()->pushLog((new rpc::LogEvent(rpc::LogLevel::Error))->toString() + fileInfo + "\t" + rpc::formatString(str, ##args) + "\n"); \
        rpc::Logger::GetGlobalLogger()->log();                                                                                                                    \
    }                                                                                                                                                             \
    /*                                                                                                                                                            \
    日志管理器                                                                                                                                               \
    1.添加日志事件信息到日志队列                                                                                                                     \
    2.打印日志                                                                                                                                                \
    3.设置日志的输出路径                                                                                                                                 \
    */
    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> s_ptr;
        Logger(LogLevel level) : m_set_level(level) {}
        // 添加日志事件信息(LogEvent)到日志队列
        void pushLog(const std::string &msg);
        void log();
        inline LogLevel getLogLevel() const
        {
            return m_set_level;
        }

        // 全局函数单独一个public
    public:
        static Logger *GetGlobalLogger();
        // 从全局配置读取日志级别
        static void InitGlobalLogger();

    public:
        LogLevel m_set_level;
        std::queue<std::string> m_buffer;
        std::mutex m_mutex;
    };
    // 日志事件信息
    class LogEvent
    {
    public:
        LogEvent(LogLevel level);
        inline std::string getFileName() const
        {
            return m_file_name;
        }
        inline LogLevel getLogLevel()
        {
            return m_level;
        }
        // 创建日志信息(时间，进程号...)并返回
        std::string toString();

    private:
        std::string m_file_name; // 文件名
        int32_t m_file_line;     // 行号
        int32_t m_pid;           // 进程号
        int32_t m_thread_id;     // 线程号
        LogLevel m_level;        // 日志级别
    };
    std::string LogLevelToString(LogLevel level);
    LogLevel StringToLogLevel(const std::string &log_level);
}
// 127-122