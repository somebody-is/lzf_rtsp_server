#include "Ipv4Address.h"

Ipv4Address::Ipv4Address()
{
    _ip = "";
    _port = 8080;
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);
}

Ipv4Address::Ipv4Address(std::string ip, uint16_t port)
{
    setAddr(ip, port);
}

Ipv4Address *Ipv4Address::createNew(std::string ip, uint16_t port)
{
    return new Ipv4Address(ip, port);
}

Ipv4Address *Ipv4Address::createNew(sockaddr_in addr)
{
    char clientIp[20];
    inet_ntop(AF_INET, &addr.sin_addr.s_addr, clientIp, sizeof(clientIp));
    uint16_t port = ntohs(addr.sin_port);
    return new Ipv4Address(clientIp,port);
}

void Ipv4Address::setAddr(std::string ip, uint16_t port)
{
    _ip = ip;
    _port = port;
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

std::string Ipv4Address::ip()
{
    return _ip;
}

uint16_t Ipv4Address::port()
{
    return _port;
}

struct sockaddr_in Ipv4Address::getAddr()
{
    return _addr;
}

void Ipv4Address::setPort(uint16_t port)
{
    _addr.sin_port = htons(port);
    _port = port;
}
