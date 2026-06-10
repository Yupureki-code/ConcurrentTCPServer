#include "../output/include/EchoServer.hpp"
#include <cstdlib>

int main(int argc, char* argv[])
{
    uint16_t port = 8080;
    if (argc > 1) port = atoi(argv[1]);
    EchoServer Svr(port);
    Svr.Start();
}