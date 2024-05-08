#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "common/msg_id_util.h"
#include "common/log.h"

/*这个函数的目的是确保每个消息ID都是唯一的，即使在高并发的情况下也能保持唯一性。
它通过随机生成和递增的方式来生成ID，同时利用线程局部变量避免了多线程环境下的竞态条件。*/
namespace rpc
{
    // 定义消息ID的长度，这里设置为20。
    static int g_msg_id_length = 20;
    // 一个全局文件描述符，用于打开/dev/urandom设备，以便获取随机数。
    static int g_random_fd = -1;

    // 用于存储当前线程生成的最新消息ID。
    static thread_local std::string t_msg_id_no;
    // 用于存储当前线程生成的最大消息ID。
    static thread_local std::string t_max_msg_id_no;

    std::string MsgIDUtil::GenMsgID()
    {
        if (t_msg_id_no.empty() || t_msg_id_no == t_max_msg_id_no)
        {
            if (g_random_fd == -1)
            {
                g_random_fd = open("/dev/urandom", O_RDONLY);
            }
            std::string res(g_msg_id_length, 0);
            // 从/dev/urandom读取随机数据到res中。如果读取的字节数不等于g_msg_id_length，则记录错误并返回空字符串。
            if ((read(g_random_fd, &res[0], g_msg_id_length)) != g_msg_id_length)
            {
                ERRORLOG("read form /dev/urandom error");
                return "";
            }
            // 读到的有的是不可见字符，转换成常见的数字
            //  遍历res中的每个字符，将其转换为0到9之间的数字，并更新res。
            for (int i = 0; i < g_msg_id_length; ++i)
            {
                uint8_t x = ((uint8_t)(res[i])) % 10;
                res[i] = x + '0';
                t_max_msg_id_no += "9";
            }
            t_msg_id_no = res;
        }
        // 如果t_msg_id_no不为空且不等于t_max_msg_id_no，说明可以基于上次生成的ID进行递增：
        else
        {
            // 从t_msg_id_no的最后一个字符开始向前查找，直到找到一个不是'9'的字符。
            // 将找到的字符加1，并将它之后的所有字符设置为'0'，从而实现增量。
            size_t i = t_msg_id_no.length() - 1;
            while (t_msg_id_no[i] == '9' && i >= 0)
            {
                i--;
            }
            if (i >= 0)
            {
                t_msg_id_no[i] += 1;
                for (size_t j = i + 1; i < t_msg_id_no.length(); ++j)
                {
                    t_msg_id_no[j] = '0';
                }
            }
        }

        return t_msg_id_no;
    }

}