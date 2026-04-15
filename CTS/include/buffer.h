#pragma once

#include <string>
#include <vector>

#define DEFAULT_BUFFER_SIZE 1024

class Buffer
{
public:
	Buffer(int capacity = DEFAULT_BUFFER_SIZE);
	std::string Read(unsigned long long len);
	std::string Read();
	void Write(const std::string& s);
	std::string GetLine(const std::string& sep = "\r\n", bool keep_sep = false);
	unsigned long long Size();

private:
	std::vector<char> _buffer;
	unsigned long long _capacity;
	unsigned long long _read_index = 0;
	unsigned long long _write_index = 0;
};
