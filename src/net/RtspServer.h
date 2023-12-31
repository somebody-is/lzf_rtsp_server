#ifndef LZF_RTSP_SERVER
#define LZF_RTSP_SERVER

#include<map>
#include <sys/socket.h>
#include <sys/epoll.h>
#include"../base/log.h"
#include"Ipv4Address.h"
#include"SocketOps.h"
#include"../scheduler/Event.h"
#include"../usageEnvironment/UsageEnvironment.h"
#include"RtspClientConnection.h"
#include"../live/ServerMediaSession.h"
#include"RtspClientSession.h"

class RtspServer
{
    public:
    RtspServer(UsageEnvironment *env);
    ~RtspServer();
    static void commingCallback(void *arg);
    void handleCommingConn();
    UsageEnvironment *envir();
    static void disconnCallback(void *arg,int clientFd);
    void handleDisconn(int clientFd);
    void start();

    ServerMediaSession* lookForMediaSession(std::string name);
    void addMediaSession(ServerMediaSession*session);
    RtspClientSession* createNewClientSession(RtspClientConnection* clientConnection);

    private:
    Event*_socketEvent;
    Ipv4Address *_addr;
    int _serverFd;
    UsageEnvironment *_env;
    std::map<int, RtspClientConnection*> _connMap;
    std::map<std::string,ServerMediaSession*>_mediaSessions;
    std::map<std::string,RtspClientSession*>_clientSessions;
};


#endif