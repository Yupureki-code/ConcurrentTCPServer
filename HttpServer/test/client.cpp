#include "socket.hpp"


int main()
{
    TcpSocket sock;
    sock.InitTcpClient();
    std::string s = "GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\nConnection: close\r\n\r\nhello";
    sock.Send(s);
    sleep(3);
    sock.Send("helloworld");
    return 0;
}