#include "../include/socket.h"
#include "../include/InetAddr.hpp"

#include <cerrno>

using namespace ns_log;

Socket::Socket()
{}

Socket::~Socket()
{}

void Socket::InitTcpServer(uint16_t port, int backlog)
{
    create_socket();
    Bind(port);
    Listen(backlog);
}

void Socket::InitTcpClient(std::string ip, uint16_t port)
{
    create_socket();
    Connect(ip, port);
}

TcpSocket::TcpSocket(int sockfd)
    : _sockfd(sockfd)
{}

void TcpSocket::create_socket()
{
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (_sockfd < 0)
    {
        logger(LogLevel::FATAL) << "socket error";
        exit(ExitCode::SOCKET);
    }

    int flag = fcntl(_sockfd, F_GETFL);
    fcntl(_sockfd, F_SETFL, flag | O_NONBLOCK);

    logger(LogLevel::INFO) << "sockect create success : " << _sockfd;
}

void TcpSocket::Bind(uint16_t port)
{
    InetAddr addr(port);
    int n = bind(_sockfd, CONV(addr.get_addr()), sizeof(addr.get_addr()));
    if (n < 0)
    {
        logger(LogLevel::FATAL) << "bind error!";
        exit(ExitCode::BIND);
    }
    logger(LogLevel::INFO) << "bind success";
}

void TcpSocket::Listen(int backlog)
{
    int n = listen(_sockfd, backlog);
    if (n < 0)
    {
        logger(LogLevel::FATAL) << "listen error!";
        exit(ExitCode::LISTEN);
    }
    logger(LogLevel::INFO) << "listen success";
}

int TcpSocket::Accept(InetAddr& addr)
{
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    int sockfd = accept(_sockfd, CONV(peer), &len);
    if (sockfd < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return ACCEPT_DONE;
        if (errno == EINTR)
            return ACCEPT_CONTINUE;

        logger(LogLevel::WARNING) << "accept error!";
        return ACCEPT_ERR;
    }
    addr = InetAddr(peer);
    logger(LogLevel::INFO) << addr.get_string() << " accpet success " << "client:" << addr.get_string();
    return sockfd;
}

bool TcpSocket::Connect(const std::string& ip, const uint16_t& port)
{
    InetAddr addr(ip, port);
    int n = connect(_sockfd, CONV(addr.get_addr()), sizeof(addr.get_addr()));
    if (n < 0)
    {
        logger(LogLevel::FATAL) << "connect error!";
        return false;
    }
    logger(LogLevel::INFO) << "connect success";
    return true;
}

int TcpSocket::Recv(std::string& out, int flag)
{
    char buffer[MAXNUM];
    ssize_t n = recv(_sockfd, buffer, sizeof(buffer) - 1, flag);
    if (n > 0)
    {
        buffer[n] = '\0';
        out += buffer;
    }
    return n;
}

int TcpSocket::NonBlockRecv(std::string& out)
{
    int n = Recv(out, MSG_DONTWAIT);
    if (n > 0)
        return n;
    if (n == 0)
        return -1;
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
        return 0;
    return -2;
}

int TcpSocket::Send(const std::string& out, int flag)
{
    return send(_sockfd, out.c_str(), out.size(), flag);
}

int TcpSocket::NonBlockSend(const std::string& out)
{
    return Send(out, MSG_DONTWAIT);
}

int TcpSocket::get_sockfd()
{
    return _sockfd;
}

void TcpSocket::set_sockfd(int fd)
{
    _sockfd = fd;
}

void TcpSocket::Close()
{
    if (_sockfd >= 0)
    {
        close(_sockfd);
        _sockfd = -1;
    }
}

TcpSocket::~TcpSocket()
{
    Close();
}