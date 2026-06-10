#pragma once

#include <string>
#include <vector>

#define DEFAULT_BUFFER_SIZE 1024

/// Buffer类用于管理连接的读写缓冲区，提供了读取、写入和获取行等功能
class Buffer
{
public:
	Buffer(int capacity = DEFAULT_BUFFER_SIZE);
	std::string Read(unsigned long long len);//指定大小读取
	std::string Read();//读取所有数据
	void Write(const std::string& s);//写入数据
	void Write(const char* data, size_t len);//写入原始数据（避免string构造）
	std::string GetLine(const std::string& sep = "\r\n", bool keep_sep = false);//读取一行数据，默认以\r\n为分隔符，keep_sep表示是否保留分隔符
	unsigned long long Size();//获取缓冲区中数据的大小
	
	// 零拷贝接口
	char* WriteBegin();       // 获取可写位置指针
	const char* ReadBegin() const;  // 获取可读位置指针
	size_t WritableBytes() const;   // 可写字节数
	size_t ReadableBytes() const;   // 可读字节数
	void CommitWrite(size_t n);     // 确认写入n字节
	void CommitRead(size_t n);      // 确认读取n字节
	void EnsureWritable(size_t len); // 确保有足够可写空间

private:
	std::vector<char> _buffer;//使用vector进行内存管理
	unsigned long long _capacity;//vector的存储大小
	unsigned long long _read_index = 0;//读取的偏移量
	unsigned long long _write_index = 0;//写取的偏移量
};
