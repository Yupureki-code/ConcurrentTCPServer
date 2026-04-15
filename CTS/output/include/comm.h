#pragma once

#include <any>
#include <cstdlib>
#include <fcntl.h>
#include <functional>
#include <memory>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>

#include <Logger/logstrategy.h>

class InetAddr;

#define CONV(x) ((struct sockaddr*)(&x))
#define DEFAULT_BACKLOG 1024
#define MAXNUM 1024
#define DEFAULT_PORT 8080
#define DEFAULT_SOCKFD -1
#define DEFAULT_IP "127.0.0.1"

#define ACCEPT_DONE -1
#define ACCEPT_CONTINUE -2
#define ACCEPT_ERR -3

using Context = std::any;

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
    EVENTFD_WRITE,
    TIMERFD_CREATE,
    TIMERFD_READ
};

enum ResultCode
{
    OK = 0,
};

class nocopy
{
public:
    nocopy() {}
    ~nocopy() {}
    nocopy(const nocopy&) = delete;
    const nocopy& operator=(const nocopy&) = delete;
};

void set_nonblock(int fd);