#ifndef _INETADDRESS_HPP_
#define _INETADDRESS_HPP_

#include <arpa/inet.h>
#include <string>

class inetAddress
{
public:
    explicit inetAddress(uint16_t port = 9190, std::string ip = "127.0.0.1");
    explicit inetAddress(const sockaddr_in &addr) : addr_(addr) {}

    std::string toIp() const;
    uint16_t toPort() const;
    std::string toIpPort() const;

    const sockaddr *GetSockAddr() const;
    void setSockAddr(const sockaddr_in &addr) { addr_ = addr; }

private:
    sockaddr_in addr_;
};

#endif // _INETADDRESS_HPP_
