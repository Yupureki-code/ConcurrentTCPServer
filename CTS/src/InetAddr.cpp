#include "../include/InetAddr.h"

#include <arpa/inet.h>
#include <cstring>
#include <utility>

InetAddr::InetAddr()
{}

InetAddr::InetAddr(struct sockaddr_in& addr)
    : _addr(addr)
{
    _port = ntohs(addr.sin_port);
    char buffer[64];
    inet_ntop(AF_INET, &addr.sin_addr, buffer, sizeof(buffer));
    _ip = buffer;
}

InetAddr::InetAddr(std::string ip, uint16_t port)
    : _port(port), _ip(std::move(ip))
{
    memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);
    inet_pton(AF_INET, _ip.c_str(), &_addr.sin_addr);
}

InetAddr::InetAddr(uint16_t port)
    : _port(port), _ip("00.00.00.00")
{
    memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);
    _addr.sin_addr.s_addr = INADDR_ANY;
}

InetAddr::InetAddr(InetAddr& addr)
{
    *this = addr;
}

InetAddr::InetAddr(const InetAddr& addr)
{
    _port = addr.get_port();
    _ip = addr.get_ip();
    _addr = addr.get_addr();
}

InetAddr& InetAddr::operator=(const InetAddr& addr)
{
    _port = addr.get_port();
    _ip = addr.get_ip();
    _addr = addr.get_addr();
    return *this;
}

void InetAddr::set_len(socklen_t len)
{
    _len = len;
}

const struct sockaddr_in& InetAddr::get_addr() const
{
    return _addr;
}

struct sockaddr_in* InetAddr::get_addr_ptr()
{
    return &_addr;
}

socklen_t* InetAddr::get_len_ptr()
{
    return &_len;
}

const socklen_t& InetAddr::get_len()
{
    return _len;
}

std::string InetAddr::get_string() const
{
    return _ip + ":" + std::to_string(_port);
}

std::string InetAddr::get_ip() const
{
    return _ip;
}

uint16_t InetAddr::get_port() const
{
    return _port;
}