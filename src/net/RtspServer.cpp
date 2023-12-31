#include "RtspServer.h"
#include <thread>
#include"../live/H264ServerMediaSubSession.h"
#include"../live/AACServerMediaSubSession.h"

RtspServer::RtspServer(UsageEnvironment *env)
{
    _addr = new Ipv4Address();
    _env = env;
    _serverFd = socketOps::createTcpSocket();
    socketOps::bind(_serverFd, "127.0.0.1", 8081);
    
}

RtspServer::~RtspServer()
{
    delete _addr;
    delete _socketEvent;
}

void RtspServer::commingCallback(void *arg)
{
    RtspServer *server = (RtspServer *)arg;
    server->handleCommingConn();
}

void RtspServer::handleCommingConn()
{
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = socketOps::accept(_serverFd, &clientAddr);
    if (clientFd < 0)
    {
        LOGE("建立连接失败");
        return;
    }
    char clientIp[20];
    inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIp, sizeof(clientIp));
    LOGI("建立连接成功! fd: %d, ip: %s, port: %d", clientFd,clientIp , ntohs(clientAddr.sin_port));

    RtspClientConnection *conn = RtspClientConnection::createNewClientConnection(this, clientFd, Ipv4Address::createNew(clientAddr));
    conn->setCloseCallback(disconnCallback, this);
    _connMap.insert(std::make_pair(clientFd, conn));
}

UsageEnvironment *RtspServer::envir()
{
    return _env;
}

void RtspServer::disconnCallback(void *arg, int clientFd)
{
    RtspServer *rtspServer = (RtspServer *)arg;
    rtspServer->handleDisconn(clientFd);
}

void RtspServer::handleDisconn(int clientFd)
{
    RtspClientConnection *conn = _connMap[clientFd];
    _env->scheduler()->removeEvent(conn->socketEvent());
    if(_clientSessions.find(conn->_session->_sessionId)==_clientSessions.end())
        _clientSessions.erase(conn->_session->_sessionId);
    delete conn;
    _connMap.erase(clientFd);
}

void RtspServer::start()
{
    socketOps::listen(_serverFd, 60);
    _socketEvent = SocketEvent::createNew(_serverFd, EPOLLIN, commingCallback, (void *)this, NULL, NULL);
    _env->scheduler()->addEvent(_socketEvent);
    LOGI("创建RtspServer成功,ip:[%s],port:[%d]", "0.0.0.0", 8081);

    ServerMediaSession*mediaSession1 = ServerMediaSession::createNew("song1");
    ServerMediaSubSession* h264SubSession1 = H264ServerMediaSubSession::createNew("/home/adminlzf/Desktop/myproject/LZF_RtspServer/data/banma.h264","h264",25);
    ServerMediaSubSession* aacSubSession1 = AACServerMediaSubSession::createNew("/home/adminlzf/Desktop/myproject/LZF_RtspServer/data/banma.aac","aac",46.875);
    mediaSession1->addSubSession(h264SubSession1);
    mediaSession1->addSubSession(aacSubSession1);

    ServerMediaSession*mediaSession2 = ServerMediaSession::createNew("song2");
    ServerMediaSubSession* h264SubSession2 = H264ServerMediaSubSession::createNew("/home/adminlzf/Desktop/myproject/LZF_RtspServer/data/saddle_of_heart.h264","h264",25);
    ServerMediaSubSession* aacSubSession2 = AACServerMediaSubSession::createNew("/home/adminlzf/Desktop/myproject/LZF_RtspServer/data/saddle_of_heart.aac","aac",46.875);
    mediaSession2->addSubSession(h264SubSession2);
    mediaSession2->addSubSession(aacSubSession2);

    ServerMediaSession*mediaSession3 = ServerMediaSession::createNew("movie1");
    ServerMediaSubSession* h264SubSession3 = H264ServerMediaSubSession::createNew("/home/adminlzf/Desktop/myproject/LZF_RtspServer/data/output.h264","h264",24);
    ServerMediaSubSession* aacSubSession3 = AACServerMediaSubSession::createNew("/home/adminlzf/Desktop/myproject/LZF_RtspServer/data/output.aac","aac",46.875);
    mediaSession3->addSubSession(h264SubSession3);
    mediaSession3->addSubSession(aacSubSession3);
    
    addMediaSession(mediaSession1);
    addMediaSession(mediaSession3);
    addMediaSession(mediaSession2);
}

ServerMediaSession *RtspServer::lookForMediaSession(std::string name)
{
    for (auto it = _mediaSessions.begin(); it != _mediaSessions.end(); ++it) {
        if(name.size()>=it->first.size() && name.substr(0,it->first.size())==it->first)
            return it->second;
    }
    return nullptr;
}

void RtspServer::addMediaSession(ServerMediaSession *session)
{
    _mediaSessions.insert(std::make_pair(session->name(),session));
}

RtspClientSession *RtspServer::createNewClientSession(RtspClientConnection* clientConnection)
{
    char hex_str[9]; // 需要生成的 8 位十六进制数，加上字符串结束符号 '\0'
    srand(time(NULL)); // 初始化随机数种子

    // 生成一个 8 位的随机十六进制数
    snprintf(hex_str, sizeof(hex_str), "%08X", rand());
    RtspClientSession *clientSession = RtspClientSession::createNew(clientConnection,hex_str);
    _clientSessions.insert(std::make_pair(hex_str,clientSession));
    return clientSession;
}