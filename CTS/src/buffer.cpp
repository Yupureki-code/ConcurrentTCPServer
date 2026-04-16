#include "../include/buffer.h"

#include "../include/logstrategy.hpp"
#include <algorithm>

using namespace ns_log;

Buffer::Buffer(int capacity)
    : _capacity(capacity), _buffer(capacity)
{}

std::string Buffer::Read(unsigned long long len)
{
    len = std::min(len, _write_index - _read_index);
    std::string s;
    s.resize(len);
    std::copy(_buffer.begin() + _read_index, _buffer.begin() + _read_index + len, s.begin());
    _read_index += len;
    logger(ns_log::INFO) << "读取Buffer:" << s;
    return s;
}

std::string Buffer::Read()
{
    return Read(_write_index - _read_index);
}

void Buffer::Write(const std::string& s)
{
    if (_capacity - _write_index < s.size())
    {
        if (_read_index > 0)
        {
            std::copy(_buffer.begin() + _read_index, _buffer.begin() + _write_index, _buffer.begin());
            _write_index -= _read_index;
            _read_index = 0;
        }
        while (_capacity - _write_index < s.size())
        {
            _capacity *= 2;
            _buffer.resize(_capacity);
        }
    }
    std::copy(s.begin(), s.end(), _buffer.begin() + _write_index);
    _write_index += s.size();
}

std::string Buffer::GetLine(const std::string& sep, bool keep_sep)
{
    size_t pos = std::search(_buffer.begin() + _read_index, _buffer.begin() + _write_index, sep.begin(), sep.end()) - _buffer.begin();
    if (pos == _write_index)
        return "";
    std::string line;
    line.resize(pos - _read_index);
    std::copy(_buffer.begin() + _read_index, _buffer.begin() + pos, line.begin());
    _read_index = pos + sep.size();
    if (keep_sep)
        line += sep;
    logger(ns_log::INFO) << "读取Buffer行:" << line;
    return line;
}

unsigned long long Buffer::Size()
{
    return _write_index - _read_index;
}