#include "inetAddress.hpp"

#include <stdlib.h>
#include <string.h>

inetAddress::inetAddress(uint16_t port, std::string ip)
{
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

const sockaddr *inetAddress::GetSockAddr() const
{
    return (sockaddr *)&addr_;
}

std::string inetAddress::toIp() const
{
    char buf[64] = {0};
    inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    return buf;
}

uint16_t inetAddress::toPort() const
{
    return ntohs(addr_.sin_port);
}

std::string inetAddress::toIpPort() const
{
    char buf[64] = {0};
    inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    int length = strlen(buf);
    uint16_t port = ntohs(addr_.sin_port);
    sprintf(buf + length, " : %u", port);
    return buf;
}
