#include"SocketOps.h"

int socketOps::createTcpSocket()
{
    int fd = ::socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK,0);
    if(fd<0)
    {
        LOGE("创建TCP socket失败");
        return -1;
    }
    setPortReuse(fd,1);
    return fd;
}

int socketOps::createUdpSocket()
{
    int fd = ::socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0)
    {
        LOGE("创建TCP socket失败");
        return -1;
    }
    setPortReuse(fd,1);
    return fd;
}

void socketOps::setPortReuse(int fd, int on)
{
    int reuse = on?1:0;
    int ret = setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
    if(ret<0)
    {
        LOGE("重用地址失败");
    }
}

int socketOps::bind(int fd, std::string ip, int port)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //inet_aton("0.0.0.0",&addr.sin_addr);
    int ret = ::bind(fd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret<0)
    {
        LOGE("绑定IP端口失败");
        return ret;
    }
    return ret;
}

int socketOps::listen(int fd, int num)
{
    if(::listen(fd,num)<0)
    {
        LOGE("监听socket失败");
        return -1;
    }
    return 1;
}

int socketOps::accept(int fd,struct sockaddr_in *addr)
{
    socklen_t addrLen = sizeof(*addr);
    int clientFd = ::accept(fd,(struct sockaddr*)addr,&addrLen);
    setNoBlock(clientFd);
    return clientFd;
}

void socketOps::setNoBlock(int fd)
{
    int flags = ::fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    ::fcntl(fd, F_SETFL, flags);
}
