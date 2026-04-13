#include "buffer.hpp"
#include <iostream>
#include <cassert>

void test_buffer_basic()
{
    std::cout << "Test: Buffer basic operations" << std::endl;
    Buffer buf(1024);
    buf.Write("hello");
    buf.Write(" world\n");
    std::string line = buf.GetLine();
    assert(line == "hello world");
    std::cout << "PASSED" << std::endl;
}

void test_buffer_size()
{
    std::cout << "Test: Buffer size" << std::endl;
    Buffer buf(1024);
    buf.Write("test");
    assert(buf.Size() == 4);
    std::cout << "PASSED" << std::endl;
}

int main()
{
    std::cout << "Running Buffer Tests..." << std::endl;
    test_buffer_basic();
    test_buffer_size();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}