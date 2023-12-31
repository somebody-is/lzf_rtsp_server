#ifndef LZF_IPV4ADDRESS_H
#define LZF_IPV4ADDRESS_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Ipv4Address
{
public:
    Ipv4Address();
    Ipv4Address(std::string ip, uint16_t port);
    static Ipv4Address *createNew(std::string ip, uint16_t port);
    static Ipv4Address *createNew(struct sockaddr_in addr);
    void setAddr(std::string ip, uint16_t port);
    std::string ip();
    uint16_t port();
    struct sockaddr_in getAddr();
    void setPort(uint16_t port);


private:
    std::string _ip;
    uint16_t _port;
    struct sockaddr_in _addr;
};

#endif