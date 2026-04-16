#include "socket.hpp"
#include <cassert>

int main()
{
    TcpSocket sock;
#ifndef HTTP_SERVER_TEST_PORT
#define HTTP_SERVER_TEST_PORT 18080
#endif
    sock.InitTcpClient("127.0.0.1", HTTP_SERVER_TEST_PORT);
    std::string s = "GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n";
    s += s;s +=s;
    sock.Send(s);
    sleep(10);
    return 0;
}