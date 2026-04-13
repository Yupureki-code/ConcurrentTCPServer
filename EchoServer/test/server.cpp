#include "../output/include/EchoServer.hpp"

int main()
{
    EchoServer Svr(8080);
    Svr.Start();
}