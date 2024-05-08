#pragma once
#include <string>
#include <memory>

#include <queue>
#include <iostream>
#include <mutex>
#include <cstdio>
#include <string>
#include <semaphore.h>
#include <thread>
#include <condition_variable>
#include "common/config.h"

#include "net/time_event.h"
namespace rpc
{

    template <typename... Args>
    std::string formatString(const char *str, Args &&...args)
    {
        int size = std::snprintf(nullptr, 0, str, std::forward<Args>(args)...);

        std::string result;
        if (size > 0)
        {
            result.resize(size);
            snprintf(&result[0], size + 1, str, std::forward<Args>(args)...);
        }
        return result;
    }

#define DEBUGLOG(str, args...)                                                                                                                               \
    if (rpc::Logger::GetGlobalLogger()->getLogLevel() <= rpc::Debug)                                                                                         \
    {                                                                                                                                                        \
        std::string fileInfo = "[" + std::string(__FILE__) + ":" + std::string(__FUNCTION__) + ":" + std::to_string(__LINE__) + "]";                         \
        fileInfo.resize(60, ' ');                                                                                                                            \
        rpc::Logger::GetGlobalLogger()->pushLog((rpc::LogEvent(rpc::LogLevel::Debug)).toString() + fileInfo + "\t" + rpc::formatString(str, ##args) + "\n"); \
    }
#define INFOLOG(str, args...)                                                                                                                               \
    if (rpc::Logger::GetGlobalLogger()->getLogLevel() <= rpc::Info)                                                                                         \
    {                                                                                                                                                       \
        std::string fileInfo = "[" + std::string(__FILE__) + ":" + std::string(__FUNCTION__) + ":" + std::to_string(__LINE__) + "]";                        \
        fileInfo.resize(60, ' ');                                                                                                                           \
        rpc::Logger::GetGlobalLogger()->pushLog((rpc::LogEvent(rpc::LogLevel::Info)).toString() + fileInfo + "\t" + rpc::formatString(str, ##args) + "\n"); \
    }

#define ERRORLOG(str, args...)                                                                                                                               \
    if (rpc::Logger::GetGlobalLogger()->getLogLevel() <= rpc::Error)                                                                                         \
    {                                                                                                                                                        \
        std::string fileInfo = "[" + std::string(__FILE__) + ":" + std::string(__FUNCTION__) + ":" + std::to_string(__LINE__) + "]";                         \
        fileInfo.resize(60, ' ');                                                                                                                            \
        rpc::Logger::GetGlobalLogger()->pushLog((rpc::LogEvent(rpc::LogLevel::Error)).toString() + fileInfo + "\t" + rpc::formatString(str, ##args) + "\n"); \
    }

    // 应用调试日志
#define APPDEBUGLOG(str, ...)                                                                                                                                                                                             \
    if (rpc::Logger::GetGlobalLogger()->getLogLevel() <= rpc::Debug)                                                                                                                                                      \
    {                                                                                                                                                                                                                     \
        rpc::Logger::GetGlobalLogger()->pushAppLog(rpc::LogEvent(rpc::LogLevel::Debug).toString() +  rpc::formatString(str, ##__VA_ARGS__) + "\n"); \
    }

    // 应用信息日志
#define APPINFOLOG(str, ...)                                                                                                                                                                                             \
    if (rpc::Logger::GetGlobalLogger()->getLogLevel() <= rpc::Info)                                                                                                                                                      \
    {                                                                                                                                                                                                                    \
        rpc::Logger::GetGlobalLogger()->pushAppLog(rpc::LogEvent(rpc::LogLevel::Info).toString() +  rpc::formatString(str, ##__VA_ARGS__) + "\n"); \
    }

    // 应用错误日志
#define APPERRORLOG(str, ...)                                                                                                                                                                                             \
    if (rpc::Logger::GetGlobalLogger()->getLogLevel() <= rpc::Error)                                                                                                                                                      \
    {                                                                                                                                                                                                                     \
        rpc::Logger::GetGlobalLogger()->pushAppLog(rpc::LogEvent(rpc::LogLevel::Error).toString() +  rpc::formatString(str, ##__VA_ARGS__) + "\n"); \
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
    // 将日志级别转换为字符串
    std::string LogLevelToString(LogLevel level);

    // 将字符串转换为日志级别
    LogLevel StringToLogLevel(const std::string &log_level);

    // 异步日志类

    class AsyncLogger
    {
    public:
        typedef std::shared_ptr<AsyncLogger> s_ptr;
        // 构造函数
        // 参数:
        //   file_name: 日志输出文件名
        //   file_path: 日志输出路径
        //   max_size: 日志单个文件最大大小（单位：字节）
        AsyncLogger(const std::string &file_name, const std::string &file_path, int max_size);
        // 停止日志记录
        void stop();

        // 刷新日志缓冲区到磁盘
        void flush();
        // 将日志消息推送到缓冲区
        // 参数:
        //   vec: 存储日志消息的向量
        void pushLogBuffer(std::vector<std::string> &vec);

        void Loop();

    public:
        std::thread *m_thread;

    private:
        // 日志缓冲区
        std::queue<std::vector<std::string>> m_buffer;
        // 日志输出文件名
        std::string m_file_name;
        // 日志输出路径
        std::string m_file_path;
        // 日志单个文件最大大小（单位：字节）

        int m_max_file_size = 0;
        // 信号量
        sem_t m_sempahore;

        std::condition_variable m_condition;

        std::mutex m_mutex;

        std::string m_date;         // 当前打印日志的文件日期
        FILE *m_file_hanlder{NULL}; // 当前打开的日志文件句柄

        bool m_reopen_flag{false}; // 重新打开日志文件的标志

        int m_no{0}; // 日志文件序号

        bool m_stop_flag{false}; // 停止标志记录的标志
    };

    /*                                                                                                                                                       \
    日志管理器类                                                                                                                                          \
    1.添加日志事件信息到日志队列                                                                                                                \
    2.打印日志                                                                                                                                           \
    3.设置日志的输出路径                                                                                                                            \
    */
    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> s_ptr;

        //   level: 日志级别
        //   type: 日志类型（默认为1）

        Logger(LogLevel level, int type = 1);

        // 将日志消息推送到一般日志缓冲区
        // 参数:
        //   msg: 日志消息
        void pushLog(const std::string &msg);

        // 将日志消息推送到应用程序日志缓冲区
        // 参数:
        //   msg: 日志消息
        void pushAppLog(const std::string &msg);

        // 初始化日志系统
        void init();

        // 记录日志
        void log();

        // 获取日志级别
        inline LogLevel getLogLevel() const
        {
            return m_set_level;
        }

        // 获取异步一般日志记录器
        inline AsyncLogger::s_ptr getAsyncLopger()
        {
            return m_asnyc_logger;
        }
        // 获取异步应用程序日志记录器
        AsyncLogger::s_ptr getAsyncAppLopger()
        {
            return m_asnyc_app_logger;
        }

        // 同步循环记录日志
        void syncLoop();

        // 刷新日志缓冲区到磁盘
        void flush();

    public:
        // 获取全局日志管理器
        static Logger *GetGlobalLogger();
        // 初始化全局日志记录器
        // 参数:
        //   type: 日志类型（默认为1）
        static void InitGlobalLogger(int type = 1);

    public:
        LogLevel m_set_level;                  // 设置的日志级别
        std::vector<std::string> m_buffer;     // 一般日志缓冲区
        std::vector<std::string> m_app_buffer; // 应用程序日志缓冲区
        std::mutex m_mutex;                    // 一般日志缓冲区互斥锁
        std::mutex m_app_mutex;                // 应用程序日志缓冲区互斥锁

        std::string m_file_name; // 日志输出文件名
        std::string m_file_path; // 日志输出路径
        int m_max_file_size{0};  // 日志单个文件最大大小

        AsyncLogger::s_ptr m_asnyc_logger;     // 异步一般日志记录器
        AsyncLogger::s_ptr m_asnyc_app_logger; // 异步应用程序日志记录器

        TimerEvent::s_ptr m_timer_event; // 计时器事件

        int m_type{0}; // 日志类型
    };

    // 日志事件类
    class LogEvent
    {
    public:
        LogEvent(LogLevel level) : m_level(level) {}

        // 获取文件名
        inline std::string getFileName() const
        {
            return m_file_name;
        }
        // 获取日志级别
        inline LogLevel getLogLevel()
        {
            return m_level;
        }
        // 将日志事件转换为字符串
        std::string toString();

    private:
        std::string m_file_name; // 文件名
        int32_t m_file_line;     // 行号
        int32_t m_pid;           // 进程号
        int32_t m_thread_id;     // 线程号
        LogLevel m_level;        // 日志级别
    };
}