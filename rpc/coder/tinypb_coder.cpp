#include <string.h>
#include <arpa/inet.h>
#include "log.h"
#include "tinypb_coder.h"
#include "util.h"
namespace rpc
{
    void TinyPBCoder::encode(std::vector<AbstractProtocol::s_ptr> &messages, TcpBuffer::s_ptr out_buffer)
    {
        for (auto &it : messages)
        {
            std::shared_ptr<TinyPBProtocol> msg = std::dynamic_pointer_cast<TinyPBProtocol>(it);
            int len = 0;
            const char *buff = encodeTinyPB(msg, len);
            if (buff != NULL && len != 0)
            {
                out_buffer->writeToBuffer(buff, len);
            }
            if (buff)
            {
                free((void *)buff);
                buff = nullptr;
            }
        }
    }
    void TinyPBCoder::decode(std::vector<AbstractProtocol::s_ptr> &out_messages, TcpBuffer::s_ptr buffer)
    {
        while (1)
        {
            std::vector<char> temp = buffer->m_buffer;
            int start_index = buffer->readIndex();
            int end_index = -1;

            int pk_len = 0;
            bool parse_success = false;
            int i = 0;
            for (i = start_index; i < buffer->writeIndex(); ++i)
            {
                if (temp[i] == TinyPBProtocol::PB_START)
                {
                    // 读下去四个字节。由于是网络字节序，需要转为主机字节序
                    if (i + 1 < buffer->writeIndex())
                    {
                        pk_len = getInt32FromNetByte(&temp[i + 1]);
                        DEBUGLOG("get pk_len = %d", pk_len);

                        // 结束符的索引
                        int j = i + pk_len - 1;
                        if (j >= buffer->writeIndex())
                        {
                            continue;
                        }
                        if (temp[j] == TinyPBProtocol::PB_END)
                        {
                            start_index = i;
                            end_index = j;
                            parse_success = true;
                            break;
                        }
                    }
                }
            }
            // 如果遍历完消息流仍未找到完整的消息，则退出循环
            if (i >= buffer->writeIndex())
            {
                DEBUGLOG("decode end, read all buffer data");
                return;
            }
            if (parse_success)
            {
                buffer->moveReadIndex(end_index - start_index + 1);
                // 创建一个智能指针，指向解析出的消息对象
                std::shared_ptr<TinyPBProtocol> message = std::make_shared<TinyPBProtocol>();
                // 将解析得到的消息长度存储到消息对象中
                message->m_pk_len = pk_len;
                // 解析请求 ID 长度的索引
                int req_id_len_index = start_index + sizeof(char) + sizeof(message->m_pk_len);
                // 如果请求 ID 长度索引超出了消息末尾，则记录错误并继续下一轮循环
                if (req_id_len_index >= end_index)
                {
                    message->parse_success = false;
                    ERRORLOG("parse error, req_id_len_index[%d] >= end_index[%d]", req_id_len_index, end_index);
                    continue;
                }
                // 解析请求 ID 长度并存储到消息对象中
                message->m_msg_id_len = getInt32FromNetByte(&temp[req_id_len_index]);
                DEBUGLOG("parse req_id_len=%d", message->m_msg_id_len);
                // 计算请求 ID 的起始索引
                int req_id_index = req_id_len_index + sizeof(message->m_msg_id_len);

                // 创建缓冲区用于存储请求 ID
                char req_id[100] = {0};
                // 从消息流中复制请求 ID 数据到缓冲区中，并存储到消息对象中
                memcpy(&req_id[0], &temp[req_id_index], message->m_msg_id_len);
                message->m_msg_id = std::string(req_id);
                DEBUGLOG("parse req_id=%s", message->m_msg_id.c_str());

                int method_name_len_index = req_id_index + message->m_msg_id_len;
                // 如果方法名长度索引超出了消息末尾，则记录错误并继续下一轮循环
                if (method_name_len_index >= end_index)
                {
                    message->parse_success = false;
                    ERRORLOG("parse error, method_name_len_index[%d] >= end_index[%d]", method_name_len_index, end_index);
                    continue;
                }
                message->m_method_name_len = getInt32FromNetByte(&temp[method_name_len_index]);

                int method_name_index = method_name_len_index + sizeof(message->m_method_name_len);
                char method_name[512] = {0};
                memcpy(&method_name[0], &temp[method_name_index], message->m_method_name_len);
                message->m_method_name = std::string(method_name);
                DEBUGLOG("parse method_name=%s", message->m_method_name.c_str());
                // 解析错误码的索引
                int err_code_index = method_name_index + message->m_method_name_len;
                // 如果错误码索引超出了消息末尾，则记录错误并继续下一轮循环
                if (err_code_index >= end_index)
                {
                    message->parse_success = false;
                    ERRORLOG("parse error, err_code_index[%d] >= end_index[%d]", err_code_index, end_index);
                    continue;
                }
                // 解析错误码并存储到消息对象中
                message->m_err_code = getInt32FromNetByte(&temp[err_code_index]);
                // 解析错误信息长度的索引
                int error_info_len_index = err_code_index + sizeof(message->m_err_code);
                // 如果错误信息长度索引超出了消息末尾，则记录错误并继续下一轮循环
                if (error_info_len_index >= end_index)
                {
                    message->parse_success = false;
                    ERRORLOG("parse error, error_info_len_index[%d] >= end_index[%d]", error_info_len_index, end_index);
                    continue;
                }
                // 解析错误信息长度并存储到消息对象中
                message->m_err_info_len = getInt32FromNetByte(&temp[error_info_len_index]);

                // 计算错误信息的起始索引
                int err_info_index = error_info_len_index + sizeof(message->m_err_info_len);
                // 创建缓冲区用于存储错误信息
                char error_info[512] = {0};
                // 从消息流中复制错误信息数据到缓冲区中，并存储到消息对象中
                memcpy(&error_info[0], &temp[err_info_index], message->m_err_info_len);
                message->m_err_info = std::string(error_info);
                DEBUGLOG("parse error_info=%s", message->m_err_info.c_str());
                int pb_data_len = message->m_pk_len - message->m_method_name_len - message->m_msg_id_len - message->m_err_info_len - 2 - 24;

                // 计算 PB 数据的起始索引
                int pd_data_index = err_info_index + message->m_err_info_len;
                // 从消息流中复制 PB 数据到消息对象中
                message->m_pb_data = std::string(&temp[pd_data_index], pb_data_len);

                // 校验和校验和解析，这里略去

                // 标记消息解析成功，并将消息对象添加到输出消息向量中
                message->parse_success = true;
                out_messages.push_back(message);
            }
        }
    }
    const char *TinyPBCoder::encodeTinyPB(std::shared_ptr<TinyPBProtocol> message, int &len)
    {
        if (message->m_msg_id.empty())
        {
            message->m_msg_id = "123456789";
        }
        DEBUGLOG("req_id = %s", message->m_msg_id.c_str());
        int pk_len = 2 + 24 + message->m_msg_id.length() + message->m_method_name.length() + message->m_err_info.length() + message->m_pb_data.length();
        DEBUGLOG("pk_len = %d", pk_len);

        char *buf = reinterpret_cast<char *>(malloc(pk_len));
        char *tmp = buf;

        *tmp = TinyPBProtocol::PB_START;
        tmp++;

        int32_t pk_len_net = htonl(pk_len);
        memcpy(tmp, &pk_len_net, sizeof(pk_len_net));
        tmp += sizeof(pk_len_net);

        int req_id_len = message->m_msg_id.length();
        int32_t req_id_len_net = htonl(req_id_len);
        memcpy(tmp, &req_id_len_net, sizeof(req_id_len_net));
        tmp += sizeof(req_id_len_net);

        if (!message->m_msg_id.empty())
        {
            memcpy(tmp, &(message->m_msg_id[0]), req_id_len);
            tmp += req_id_len;
        }

        int method_name_len = message->m_method_name.length();
        int32_t method_name_len_net = htonl(method_name_len);
        memcpy(tmp, &method_name_len_net, sizeof(method_name_len_net));
        tmp += sizeof(method_name_len_net);

        if (!message->m_method_name.empty())
        {
            memcpy(tmp, &(message->m_method_name[0]), method_name_len);
            tmp += method_name_len;
        }

        int32_t err_code_net = htonl(message->m_err_code);
        memcpy(tmp, &err_code_net, sizeof(err_code_net));
        tmp += sizeof(err_code_net);

        int err_info_len = message->m_err_info.length();
        int32_t err_info_len_net = htonl(err_info_len);
        memcpy(tmp, &err_info_len_net, sizeof(err_info_len_net));
        tmp += sizeof(err_info_len_net);

        if (!message->m_err_info.empty())
        {
            memcpy(tmp, &(message->m_err_info[0]), err_info_len);
            tmp += err_info_len;
        }

        if (!message->m_pb_data.empty())
        {
            memcpy(tmp, &(message->m_pb_data[0]), message->m_pb_data.length());
            tmp += message->m_pb_data.length();
        }

        int32_t check_sum_net = htonl(1);
        memcpy(tmp, &check_sum_net, sizeof(check_sum_net));
        tmp += sizeof(check_sum_net);

        *tmp = TinyPBProtocol::PB_END;

        message->m_pk_len = pk_len;
        message->m_msg_id_len = req_id_len;
        message->m_method_name_len = method_name_len;
        message->m_err_info_len = err_info_len;
        message->parse_success = true;
        len = pk_len;

        DEBUGLOG("encode message[%s] success", message->m_msg_id.c_str());

        return buf;
    }
} // namespace rpc
