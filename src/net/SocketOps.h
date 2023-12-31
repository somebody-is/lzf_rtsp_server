#ifndef LZF_SOCKETOPS_H
#define LZF_SOCKETOPS_H

#include<string>
#include<sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include"../base/log.h"


namespace socketOps
{
    int createTcpSocket();
    int createUdpSocket();
    void setPortReuse(int fd,int on);
    int bind(int fd,std::string ip,int port);
    int listen(int fd,int num);
    int accept(int fd,struct sockaddr_in *addr);
    void setNoBlock(int fd);
}   


#endif