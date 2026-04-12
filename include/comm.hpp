#pragma once

#include "logstrategy.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "InetAddr.hpp"
#include <cstdlib>
#include <functional>
#include <unistd.h>
#include <string>
#include <jsoncpp/json/json.h>
#include <memory>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define CONV(x) ((struct sockaddr*)(&x))
#define DEFAULT_BACKLOG 1024
#define MAXNUM 1024
#define DEFAULT_PORT 8080
#define DEFAULT_SOCKFD -1
#define DEFAULT_IP "127.0.0.1"

#define ACCEPT_DONE -1
#define ACCEPT_CONTINUE -2
#define ACCEPT_ERR -3

enum ExitCode
{
    NORMAL = 0,
    SOCKET,
    BIND,
    LISTEN,
    ACCEPT,
    FORMAT,
    CONNECT,
    FORK,
    EPOLL_CREATE,
    EPOLL_CTL,
    EPOLL_WAIT,
    EVENTFD_CREATE,
    EVENTFD_READ,
    EVENTFD_WRITE
};

enum ResultCode
{
    OK = 0,
};

class nocopy{
public:
    nocopy()
    {}
    ~nocopy()
    {}
    nocopy(const nocopy&) = delete;
    const nocopy& operator=(const nocopy&) = delete;
};

inline void set_nonblock(int fd)
{
    int fl = fcntl(fd,F_GETFL);
    if(fl > 0)
    {
        fcntl(fd,F_SETFL,fl | O_NONBLOCK);
    }
}
