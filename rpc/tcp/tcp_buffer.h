#pragma once
#include <vector>
#include <memory>
namespace rpc
{
    class TcpBuffer
    {

    public:
        typedef std::shared_ptr<TcpBuffer> s_ptr;

    public:
        TcpBuffer(int size);

        ~TcpBuffer();
        // 还有多少可读内容
        inline int readAble()
        {
            return m_write_index - m_read_index;
        }
        // 还有多少空间可写
        inline int writeAble()
        {
            return m_buffer.size() - m_write_index;
        }
        inline int readIndex()
        {
            return m_read_index;
        }
        inline int writeIndex()
        {
            return m_write_index;
        }
        void writeToBuffer(const char *buf, int size);

        void readFromBuffer(std::vector<char> &re, int size);

        void resizeBuffer(int new_size);
        // 优化已读的空间
        void adjustBuffer();

        void moveReadIndex(int size);

        void moveWriteIndex(int size);

    private:
        int m_read_index{0};
        int m_write_index{0};
        int m_size{0};

    public:
        std::vector<char> m_buffer;
    };

}
// 55