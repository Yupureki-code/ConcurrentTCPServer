#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <algorithm>
#include "logstrategy.hpp"

using namespace ns_log;

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
        logger(INFO)<<"读取Buffer:"<<s;
        return s;
    }
    std::string Read()
    {
        return Read(_write_index - _read_index);
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
        logger(INFO)<<"写入Buffer:"<<s;
    }
    std::string GetLine()
    {
        std::string s;
         while(_read_index < _write_index && _buffer[_read_index] != '\n')
         {
            s += _buffer[_read_index++];
         }
         if(_buffer[_read_index] == '\n')
            _read_index++;
        logger(INFO)<<"获取BUffer一行:"<<s;
        return s;
    }
    unsigned long long Size()
    {
        return _write_index - _read_index;
    }
private:
    std::vector<char> _buffer;
    unsigned long long _capacity;
    unsigned long long _read_index = 0;
    unsigned long long _write_index = 0;
};