#pragma once
#include <unistd.h>
#include <thread>
#include <sys/types.h>
namespace rpc
{
    pid_t getPid();
    pid_t getThreadId();
    int64_t getNowMs();
    int32_t getInt32FromNetByte(const char *buf);
}