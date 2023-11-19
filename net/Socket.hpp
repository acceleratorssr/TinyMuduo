#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include "noncopyable.hpp"
#include "inetAddress.hpp"


class Socket : noncopyable
{
public:
    explicit Socket(int sockfd) : sockfd_(sockfd) {}
    ~Socket();
    int GetFd() const {return sockfd_;}
    void bindAddress(const inetAddress &localaddr);
    void listen();
    int accept(inetAddress *peeraddr);
    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int sockfd_;
};

sockaddr_in getLocalAddr(int sockfd);
sockaddr_in getPeerAddr(int sockfd);

#endif
