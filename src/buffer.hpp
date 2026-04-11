#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <algorithm>

#define DEFAULT_BUFFER_SIZE 1024

class Buffer
{
public:
    Buffer(int capacity = DEFAULT_BUFFER_SIZE)
    :_capacity(capacity),_buffer(capacity)
    {}
    std::string Read(unsigned long long len)
    {
        len = std::min(len,_write_index - _read_index);
        std::string s;
        s.resize(len);
        std::copy(_buffer.begin() + _read_index,_buffer.begin() + _read_index + len + 1,s.begin());
        _read_index += len;
        return s;
    }
    void Write(const std::string& s)
    {
        if(_capacity - _write_index < s.size())
        {
            if(_read_index > s.size())
            {
                std::copy(_buffer.begin() + _read_index,_buffer.begin() + _write_index,_buffer.begin());
                _write_index -=_read_index;
                _read_index = 0;
            }
            else
            {
                _capacity *= 2;
                _buffer.resize(_capacity);
            }
        }
        std::copy(s.begin(),s.end(),_buffer.begin() + _write_index);
        _write_index += s.size();
    }
private:
    std::vector<char> _buffer;
    unsigned long long _capacity;
    unsigned long long _read_index = 0;
    unsigned long long _write_index = 0;
};