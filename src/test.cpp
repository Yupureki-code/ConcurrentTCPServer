#include "timewheel.hpp"
#include "buffer.hpp"
#include <unistd.h>


int main()
{
    Buffer b(1024);
    b.Write("hello world");
    std::cout<<b.Read(1024)<<std::endl;
    std::cout<<b.Read(1024)<<std::endl;
    return 0;
}