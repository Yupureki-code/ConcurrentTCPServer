#include <CTS/eventloop.h>
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    EventLoop loop;
    loop.AddTimeTask(1, 2, []() {
        std::cout << "timer fired" << std::endl;
        std::exit(0);
    });
    loop.RunInLoop([]() {
        std::cout << "blocking task start" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "blocking task end" << std::endl;
    });
    loop.Run();
    return 0;
}