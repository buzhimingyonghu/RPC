#pragma once

#include <string>
#include "abstract_protocol.h"

namespace rpc
{
    // TinyPBProtocol结构体，用于表示TinyPB协议的消息格式
    struct TinyPBProtocol : public AbstractProtocol
    {
    public:
        // 构造函数
        TinyPBProtocol() {}
        // 析构函数
        ~TinyPBProtocol() {}

    public:
        // PB消息起始标志
        static char PB_START;
        // PB消息结束标志
        static char PB_END;

    public:
        // PK长度
        int32_t m_pk_len{0};
        // 请求ID长度
        int32_t m_msg_id_len{0};
        // 请求ID，继承自父类

        // 方法名长度
        int32_t m_method_name_len{0};
        // 方法名
        std::string m_method_name;
        // 错误码
        int32_t m_err_code{0};
        // 错误信息长度
        int32_t m_err_info_len{0};
        // 错误信息
        std::string m_err_info;
        // PB数据
        std::string m_pb_data;
        // 校验和
        int32_t m_check_sum{0};
        // 解析是否成功的标志
        bool parse_success = false;
    };
} // namespace rpc
