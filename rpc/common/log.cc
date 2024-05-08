#include <sys/time.h>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include "common/log.h"
#include "common/util.h"
#include "net/eventloop.h"
#include "common/run_time.h"
namespace rpc
{
    static Logger *g_logger = nullptr;
    Logger *Logger::GetGlobalLogger()
    {
        return g_logger;
    }
    void CoredumpHandler(int signal_no)
    {
        ERRORLOG("progress received invalid signal, will exit");
        g_logger->flush();
        g_logger->getAsyncLopger()->m_thread->join();
        g_logger->getAsyncAppLopger()->m_thread->join();
        signal(signal_no, SIG_DFL);
        raise(signal_no);
    }
    std::string LogLevelToString(LogLevel level)
    {
        switch (level)
        {
        case Debug:
            return "DEBUG";
        case Info:
            return "INFO";
        case Error:
            return "ERROE";

        default:
            return "UNKNOWN";
        }
    }
    LogLevel StringToLogLevel(const std::string &log_level)
    {
        if (log_level == "DEBUG")
        {
            return Debug;
        }
        else if (log_level == "INFO")
        {
            return Info;
        }
        else if (log_level == "ERROR")
        {
            return Error;
        }
        else
        {
            return Unknown;
        }
    }

    Logger::Logger(LogLevel level, int type)
    {
        m_set_level = level;
        m_type = type;

        if (m_type == 0)
        {
            return;
        }
        m_asnyc_logger = std::make_shared<AsyncLogger>(
            Config::GetGlobalConfig()->m_log_file_name + "_rpc",
            Config::GetGlobalConfig()->m_log_file_path,
            Config::GetGlobalConfig()->m_log_max_file_size);

        m_asnyc_app_logger = std::make_shared<AsyncLogger>(
            Config::GetGlobalConfig()->m_log_file_name + "_app",
            Config::GetGlobalConfig()->m_log_file_path,
            Config::GetGlobalConfig()->m_log_max_file_size);
    }
    void Logger::flush()
    {

        syncLoop();
        m_asnyc_logger->stop();
        m_asnyc_logger->flush();

        m_asnyc_app_logger->stop();
        m_asnyc_app_logger->flush();
    }

    void Logger::init()
    {
        if (m_type == 0)
        {
            return;
        }
        m_timer_event = std::make_shared<TimerEvent>(Config::GetGlobalConfig()->m_log_sync_inteval, true, std::bind(&Logger::syncLoop, this), "syncloop log push");
        Eventloop::GetCurrentEventloop()->addTimerEvent(m_timer_event);
    }
    void Logger::syncLoop()
    {
        // 同步 m_buffer 到 async_logger 的buffer队尾
        // printf("sync to async logger\n");

        // 临时存储日志字符串。
        std::vector<std::string> tmp_vec;
        m_mutex.lock();
        tmp_vec.swap(m_buffer);
        m_mutex.unlock();

        if (!tmp_vec.empty())
        {
            m_asnyc_logger->pushLogBuffer(tmp_vec);
        }
        tmp_vec.clear();

        // 同步 m_app_buffer 到 app_async_logger 的buffer队尾
        m_mutex.lock();
        tmp_vec.swap(m_app_buffer);
        m_mutex.unlock();

        if (!tmp_vec.empty())
        {
            m_asnyc_app_logger->pushLogBuffer(tmp_vec);
        }
    }
    void Logger::InitGlobalLogger(int type)
    {
        LogLevel global_log_level = StringToLogLevel(Config::GetGlobalConfig()->m_log_level);
        printf("Init log level [%s]\n", LogLevelToString(global_log_level).c_str());
        g_logger = new Logger(global_log_level, type);
        g_logger->init();
    }

    void Logger::pushLog(const std::string &msg)
    {
        if (m_type == 0)
        {
            printf((msg + "\n").c_str());
            return;
        }
        m_mutex.lock();
        m_buffer.push_back(msg);
        m_mutex.unlock();
    }
    void Logger::pushAppLog(const std::string &msg)
    {
        m_mutex.lock();
        m_app_buffer.push_back(msg);
        m_mutex.unlock();
    }
    void Logger::log()
    {
    }

    std::string LogEvent::toString()
    {
        struct timeval now_time;

        gettimeofday(&now_time, nullptr);

        struct tm now_time_t;
        localtime_r(&(now_time.tv_sec), &now_time_t);

        char buf[128];
        strftime(&buf[0], 128, "%y-%m-%d %H:%M:%S", &now_time_t);
        std::string time_str(buf);
        int ms = now_time.tv_usec / 1000;
        time_str = time_str + "." + std::to_string(ms);

        m_pid = getPid();
        m_thread_id = getThreadId();

        std::stringstream ss;

        ss << "[" << LogLevelToString(m_level) << "]\t"
           << "[" << time_str << "]\t"
           << "[" << m_pid << ":" << m_thread_id << "]\t";
        // 获取当前线程处理的请求的 msgid

        std::string msgid = RunTime::GetRunTime()->m_msgid;
        std::string method_name = RunTime::GetRunTime()->m_method_name;
        if (!msgid.empty())
        {
            ss << "[" << msgid << "]\t";
        }

        if (!method_name.empty())
        {
            ss << "[" << method_name << "]\t";
        }
        return ss.str();
    }
    AsyncLogger::AsyncLogger(const std::string &file_name, const std::string &file_path, int max_size)
    {
        m_file_name = file_name;
        m_file_path = file_path;
        m_max_file_size = max_size;
        sem_init(&m_sempahore, 0, 0);

        m_thread = new std::thread(&AsyncLogger::Loop, this);
        assert(m_thread->joinable());
        sem_wait(&m_sempahore);
    }
    void AsyncLogger::Loop()
    {
        sem_post(&m_sempahore);
        while (1)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            while (m_buffer.empty())
            {
                m_condition.wait(lock);
            }
            // 交换队列中第一条日志数据到临时vector
            std::vector<std::string> tmp;
            tmp.swap(m_buffer.front());
            m_buffer.pop();
            lock.unlock();
            // 获取当前时间
            timeval now;
            gettimeofday(&now, NULL);

            // 转换为本地时间
            struct tm now_time;
            localtime_r(&(now.tv_sec), &now_time);
            // 格式化日期 存入data
            const char *format = "%Y%m%d";
            char date[32];
            strftime(date, sizeof(date), format, &now_time);
            // 检查日期是否变化，如果变化则重置计数器和标志
            if (std::string(date) != m_date)
            {
                m_no = 0;
                m_reopen_flag = true;
                m_date = std::string(date);
            }
            // 检查是否需要重新打开日志文件
            if (m_file_hanlder == NULL)
            {
                m_reopen_flag = true;
            }
            // 创建一个字符串流，用于构造日志文件的名称。
            std::stringstream ss;
            ss << m_file_path << m_file_name << "_"
               << std::string(date) << "_log.";
            std::string log_file_name = ss.str() + std::to_string(m_no);

            // 如果需要重新打开文件，则关闭旧文件并打开新文件。
            if (m_reopen_flag)
            {
                if (m_file_hanlder)
                {
                    fclose(m_file_hanlder);
                }
                m_file_hanlder = fopen(log_file_name.c_str(), "a");
                m_reopen_flag = false;
            }
            // 检查文件大小，如果超过了最大文件大小，则关闭当前文件并打开新文件。
            if (ftell(m_file_hanlder) > m_max_file_size)
            {
                fclose(m_file_hanlder);

                log_file_name = ss.str() + std::to_string(m_no++);
                m_file_hanlder = fopen(log_file_name.c_str(), "a");
                m_reopen_flag = false;
            }
            // 遍历临时std::vector中的日志数据，并将它们写入到文件中。
            for (auto &i : tmp)
            {
                if (!i.empty())
                {
                    fwrite(i.c_str(), 1, i.length(), m_file_hanlder);
                }
            }
            // 清空文件缓冲区，确保所有数据都写入到磁盘。
            fflush(m_file_hanlder);

            if (m_stop_flag)
            {
                return;
            }
        }
    }
    void AsyncLogger::stop()
    {
        m_stop_flag = true;
    }
    void AsyncLogger::flush()
    {
        if (m_file_hanlder)
        {
            fflush(m_file_hanlder);
        }
    }
    void AsyncLogger::pushLogBuffer(std::vector<std::string> &vec)
    {
        m_mutex.lock();
        m_buffer.push(vec);
        m_condition.notify_all();

        m_mutex.unlock();
    }
}