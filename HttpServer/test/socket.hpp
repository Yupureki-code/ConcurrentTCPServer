#pragma once

#include "com.hpp"

class Socket
{
public:
    Socket()
    {
    }
    ~Socket()
    {
    }
    virtual void create_socket() = 0;
    virtual void Bind(uint16_t) = 0;
    virtual void Listen(int) = 0;
    virtual int Accept(InetAddr &) = 0;
    virtual bool Connect(const std::string &, const uint16_t &) = 0;
    virtual int Recv(std::string &) = 0;
    virtual int Send(const std::string &) = 0;
    virtual int get_sockfd() = 0;
    virtual void set_sockfd(int) = 0;
    virtual void Close() = 0;
    void InitTcpServer(uint16_t port = DEFAULT_PORT, int backlog = DEFAULT_BACKLOG)
    {
        create_socket();
        Bind(port);
        Listen(backlog);
    }
    void InitTcpClient(std::string ip = DEFAULT_IP, uint16_t port = DEFAULT_PORT)
    {
        create_socket();
        Connect(ip, port);
    }
};

class TcpSocket : public Socket
{
private:
    using func_t = std::function<void()>;

public:
    TcpSocket(int sockfd = DEFAULT_SOCKFD)
        : _sockfd(sockfd)
    {
    }
    void create_socket() override
    {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (_sockfd < 0)
        {
            exit(ExitCode::SOCKET);
        }
    }
    void Bind(uint16_t port) override
    {
        InetAddr addr(port);
        int n = bind(_sockfd, CONV(addr.get_addr()), sizeof(addr.get_addr()));
        if (n < 0)
        {
            exit(ExitCode::BIND);
        }
    }
    void Listen(int backlog) override
    {
        int n = listen(_sockfd, backlog);
        if (n < 0)
        {
            exit(ExitCode::LISTEN);
        }
    }
    int Accept(InetAddr &addr) override
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int sockfd = accept(_sockfd, CONV(peer), &len);
        if (sockfd < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return ACCEPT_DONE;
            }
            else if (errno == EINTR)
            {
                return ACCEPT_CONTINUE;
            }
            else
            {
                return ACCEPT_ERR;
            }
        }
        InetAddr tmp(peer);
        addr = tmp;
        return sockfd;
    }
    bool Connect(const std::string &ip, const uint16_t &port) override
    {
        InetAddr addr(ip, port);
        int n = connect(_sockfd, CONV(addr.get_addr()), sizeof(addr.get_addr()));
        if (n < 0)
        {
            return false;
        }
        return true;
    }
    int Recv(std::string &out) override
    {
        char buffer[MAXNUM];
        ssize_t n = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n] = '\0';
            out += buffer;
            return n;
        }
        else if (n == 0)
            return 0;
        else
            return -1;
    }
    int Send(const std::string &out) override
    {
        std::cout<<"send:"<<out;
        int n = send(_sockfd, out.c_str(), out.size(), 0);
        return n;
        // logger(LogLevel::INFO)<<"success";
    }
    int get_sockfd() override
    {
        return _sockfd;
    }
    void set_sockfd(int fd) override
    {
        _sockfd = fd;
    }
    void Close() override
    {
        if (_sockfd)
            close(_sockfd);
    }

private:
    int _sockfd;
};