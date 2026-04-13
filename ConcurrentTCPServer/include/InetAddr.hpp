#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>

class InetAddr{
public:
    InetAddr()
    {}
    InetAddr(struct sockaddr_in & addr)
    :_addr(addr)
    {
        _port = ntohs(addr.sin_port);
        char buffer[64];
        inet_ntop(AF_INET,&addr.sin_addr,buffer,sizeof(_addr));
        _ip = buffer;
    }
    InetAddr(std::string ip,uint16_t port)
    :_port(port),_ip(ip)
    {
        memset(&_addr,0,sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_port = htons(_port);
        inet_pton(AF_INET,_ip.c_str(),&_addr.sin_addr);
    }
    InetAddr(uint16_t port)
    :_port(port),_ip("00.00.00.00")
    {
        memset(&_addr,0,sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_port = htons(_port);
        _addr.sin_addr.s_addr = INADDR_ANY;
    }
    InetAddr(InetAddr& addr)
    {
        *this = addr;
    }
    InetAddr(const InetAddr& addr)
    {
        _port = addr.get_port();
        _ip = addr.get_ip();
        _addr = addr.get_addr();
    }
    InetAddr& operator=(const InetAddr& addr)
    {
        _port = addr.get_port();
        _ip = addr.get_ip();
        _addr = addr.get_addr();
        return *this;
    }
    void set_len(socklen_t len)
    {
        _len = len;
    }
    const struct sockaddr_in& get_addr()const
    {
        return _addr;
    }
    struct sockaddr_in* get_addr_ptr()
    {
        return &_addr;
    }
    socklen_t* get_len_ptr()
    {
        return &_len;
    }
    const socklen_t& get_len()
    {
        return _len;
    }
    std::string get_string()
    {
        return _ip + ":" + std::to_string(_port);
    }
    std::string get_ip()const
    {
        return _ip;
    }
    uint16_t get_port()const
    {
        return _port;
    }
private:
    uint16_t _port;
    std::string _ip;
    struct sockaddr_in _addr;
    socklen_t _len;
};