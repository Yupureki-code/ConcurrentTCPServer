#pragma once

#include "comm.h"

class Socket
{
public:
    Socket();
    virtual ~Socket();
    virtual void create_socket() = 0;
    virtual void Bind(uint16_t) = 0;
    virtual void Listen(int) = 0;
    virtual int Accept(InetAddr&) = 0;
    virtual bool Connect(const std::string&, const uint16_t&) = 0;
    virtual int Recv(std::string&, int) = 0;
    virtual int NonBlockRecv(std::string&) = 0;
    virtual int Send(const std::string&, int) = 0;
    virtual int NonBlockSend(const std::string&) = 0;
    virtual int get_sockfd() = 0;
    virtual void set_sockfd(int) = 0;
    virtual void Close() = 0;
    void InitTcpServer(uint16_t port = DEFAULT_PORT, int backlog = DEFAULT_BACKLOG);
    void InitTcpClient(std::string ip = DEFAULT_IP, uint16_t port = DEFAULT_PORT);
};

class TcpSocket : public Socket
{
private:
    using func_t = std::function<void()>;

public:
    TcpSocket(int sockfd = DEFAULT_SOCKFD);
    void create_socket() override;
    void Bind(uint16_t port) override;
    void Listen(int backlog) override;
    int Accept(InetAddr& addr) override;
    bool Connect(const std::string& ip, const uint16_t& port) override;
    int Recv(std::string& out, int flag = 0) override;
    int NonBlockRecv(std::string& out) override;
    int Send(const std::string& out, int flag = 0) override;
    int NonBlockSend(const std::string& out) override;
    int get_sockfd() override;
    void set_sockfd(int fd) override;
    void Close() override;
    ~TcpSocket();

private:
    int _sockfd;
};