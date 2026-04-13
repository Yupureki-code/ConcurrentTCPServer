#pragma once

#include <vector>
#include <string>
#include <cassert>

#define DEFAULT_BUFFER_SIZE 1024

class Buffer
{
public:
    Buffer(int capacity = DEFAULT_BUFFER_SIZE);
    std::string Read();
    void Write(const std::string& s);
    std::string GetLine();
    unsigned long long Size();
private:
    std::vector<char> _buffer;
    unsigned long long _capacity;
    unsigned long long _read_index = 0;
    unsigned long long _write_index = 0;
};