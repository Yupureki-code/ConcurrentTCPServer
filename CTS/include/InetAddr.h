#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <string>

class InetAddr
{
public:
    InetAddr();
    InetAddr(struct sockaddr_in& addr);
    InetAddr(std::string ip, uint16_t port);
    InetAddr(uint16_t port);
    InetAddr(InetAddr& addr);
    InetAddr(const InetAddr& addr);
    InetAddr& operator=(const InetAddr& addr);
    void set_len(socklen_t len);
    const struct sockaddr_in& get_addr() const;
    struct sockaddr_in* get_addr_ptr();
    socklen_t* get_len_ptr();
    const socklen_t& get_len();
    std::string get_string() const;
    std::string get_ip() const;
    uint16_t get_port() const;

private:
    uint16_t _port;
    std::string _ip;
    struct sockaddr_in _addr;
    socklen_t _len;
};
