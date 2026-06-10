#include "../include/buffer.h"

#include "../include/logstrategy.hpp"
#include <algorithm>
#include <cstring>

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
    return s;
}

std::string Buffer::Read()
{
    return Read(_write_index - _read_index);
}

void Buffer::Write(const std::string& s)
{
    EnsureWritable(s.size());
    std::copy(s.begin(), s.end(), _buffer.begin() + _write_index);
    _write_index += s.size();
}

void Buffer::Write(const char* data, size_t len)
{
    EnsureWritable(len);
    std::memcpy(_buffer.data() + _write_index, data, len);
    _write_index += len;
}

std::string Buffer::GetLine(const std::string& sep, bool keep_sep)
{
    size_t readable = _write_index - _read_index;
    if (readable == 0) return "";
    
    const char* start = _buffer.data() + _read_index;
    const char* found = nullptr;
    size_t sep_len = sep.size();
    
    // Fast path for single character separator (most common case)
    if (sep.size() == 1) {
        found = static_cast<const char*>(std::memchr(start, sep[0], readable));
    } else if (sep.size() == 2 && sep == "\r\n") {
        // Fast path for CRLF: search for '\n'
        const char* lf = static_cast<const char*>(std::memchr(start, '\n', readable));
        if (lf) {
            // Check if preceded by '\r'
            if (lf > start && *(lf - 1) == '\r') {
                found = lf - 1;  // Point to '\r'
            } else {
                found = lf;  // Just '\n', treat as separator
                sep_len = 1;
            }
        }
    } else {
        // Generic path using std::search
        auto it = std::search(_buffer.begin() + _read_index, _buffer.begin() + _write_index, 
                             sep.begin(), sep.end());
        if (it != _buffer.begin() + _write_index) {
            found = _buffer.data() + (it - _buffer.begin());
        }
    }
    
    if (!found) return "";
    
    size_t pos = found - _buffer.data();
    size_t line_len = pos - _read_index;
    
    std::string line(line_len, '\0');
    std::memcpy(&line[0], start, line_len);
    
    _read_index = pos + sep_len;
    if (keep_sep)
        line += sep;
    
    return line;
}

unsigned long long Buffer::Size()
{
    return _write_index - _read_index;
}

// 零拷贝接口实现
char* Buffer::WriteBegin()
{
    return _buffer.data() + _write_index;
}

const char* Buffer::ReadBegin() const
{
    return _buffer.data() + _read_index;
}

size_t Buffer::WritableBytes() const
{
    return _capacity - _write_index;
}

size_t Buffer::ReadableBytes() const
{
    return _write_index - _read_index;
}

void Buffer::CommitWrite(size_t n)
{
    _write_index += n;
}

void Buffer::CommitRead(size_t n)
{
    _read_index += n;
    if (_read_index == _write_index) {
        // All data read, reset indices to avoid unnecessary compaction
        _read_index = 0;
        _write_index = 0;
    }
}

void Buffer::EnsureWritable(size_t len)
{
    if (_capacity - _write_index < len)
    {
        if (_read_index > 0)
        {
            // Compact: move unread data to beginning
            size_t readable = _write_index - _read_index;
            std::memmove(_buffer.data(), _buffer.data() + _read_index, readable);
            _write_index = readable;
            _read_index = 0;
        }
        while (_capacity - _write_index < len)
        {
            _capacity *= 2;
            _buffer.resize(_capacity);
        }
    }
}