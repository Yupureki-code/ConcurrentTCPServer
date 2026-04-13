#include "../../ConcurrentTCPServer/include/socket.hpp"

int main()
{
    Socket* svr = new TcpSocket;
    svr->InitTcpClient("127.0.0.1",8080);
    while(1)
    {
        std::string s;
        std::cout<<"Please Enter->";
        std::cin>>s;
        svr->Send(s,0);
    }
}