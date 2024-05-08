#pragma once
#include <unistd.h>
#include <thread>
#include <sys/types.h>
namespace rpc
{
    // 获取进程的PID
    pid_t getPid();

    // 获取线程的ID
    pid_t getThreadId();

    // 获取当前时间的毫秒表示
    int64_t getNowMs();

    // 将网络字节序的 int32_t 转换为主机字节序
    int32_t getInt32FromNetByte(const char *buf);
}
// 17