
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <string.h>
#include "util.h"
namespace rpc
{
    // 全局变量，用于存储进程ID
    static int g_pid = 0;

    // 线程局部变量，用于存储线程ID
    static thread_local int t_thread_id = 0;

    // 获取当前进程的进程ID（PID）
    pid_t getPid()
    {
        // 如果进程ID已经被缓存，则直接返回缓存的值
        if (g_pid != 0)
        {
            return g_pid;
        }
        // 否则调用系统函数获取进程ID，并将结果缓存到全局变量中
        return getpid();
    }

    // 获取当前线程的线程ID（TID）
    pid_t getThreadId()
    {
        // 如果线程ID已经被缓存，则直接返回缓存的值
        if (t_thread_id != 0)
        {
            return t_thread_id;
        }
        // 否则调用系统函数获取线程ID，并将结果缓存到线程局部变量中
        return syscall(SYS_gettid);
    }

    // 获取当前时间的毫秒表示
    int64_t getNowMs()
    {
        // 获取当前时间
        timeval val;
        gettimeofday(&val, NULL);
        // 将秒和微秒转换为毫秒并返回
        return val.tv_sec * 1000 + val.tv_usec / 1000;
    }

    // 将网络字节序中表示的32位整数转换为主机字节序
    int32_t getInt32FromNetByte(const char *buf)
    {
        int32_t re;
        // 从网络字节序中复制数据到主机字节序中
        memcpy(&re, buf, sizeof(re));
        // 将结果转换为主机字节序并返回
        return ntohl(re);
    }

} // namespace rocket
