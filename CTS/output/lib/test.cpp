#include "../include/CTS.h"

int main()
{
    std::cout<<"hello"<<std::endl;
    ConcurrentTCPServer svr(8080, 0);
    svr.SetConnectedCallBack([](const PtrConnection& ptr){
        std::cout<<"connected"<<std::endl;
    });
    svr.SetMessageCallBack([](const PtrConnection& ptr,Buffer& buffer){
        std::cout<<"client say:"<<buffer.Read()<<std::endl;
    });
    svr.SetEventCallBack([](const PtrConnection& ptr){
        std::cout<<"event"<<std::endl;
    });
    svr.SetCloseCallBack([](const PtrConnection& ptr){
        std::cout<<"close"<<std::endl;
    });
    svr.Run();
}