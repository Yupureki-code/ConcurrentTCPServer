#include "include/timewheel.h"
#include "include/buffer.hpp"
#include <unistd.h>


int main()
{
    Buffer b(1024);
    for(int i = 1;i<=100;i++)
    {
        b.Write("hello world" + std::to_string(i) + "\n");
    }
    for(int i = 1;i<=100;i++)
    {
        std::cout<<b.GetLine()<<std::endl;
    }
    return 0;
}