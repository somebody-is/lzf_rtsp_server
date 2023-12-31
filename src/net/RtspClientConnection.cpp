#include <string.h>
#include "RtspClientConnection.h"
#include "RtspServer.h"

RtspClientConnection::RtspClientConnection(RtspServer *server, int clientFd, Ipv4Address *ipv4Addr):_env(server->envir())
{
    _server = server;
    _clientFd = clientFd;
    _ipv4Addr = ipv4Addr;
    _socketEvent = SocketEvent::createNew(clientFd, EPOLLIN, requestCallback, this, sendCallback, this);
    _server->envir()->scheduler()->addEvent(_socketEvent);
}

RtspClientConnection::~RtspClientConnection()
{
    close(_clientFd);
    delete _session;
    delete _ipv4Addr;
    delete _socketEvent;
}

RtspClientConnection *RtspClientConnection::createNewClientConnection(RtspServer *server, int clientFd, Ipv4Address *ipv4Addr)
{
    return new RtspClientConnection(server, clientFd, ipv4Addr);
}

void RtspClientConnection::setCloseCallback(DisConnectCallback *callback, void *arg)
{
    _disConnCallback = callback;
    _disconnArg = arg;
}

void RtspClientConnection::requestCallback(void *arg)
{
    RtspClientConnection *rtspClientConn = (RtspClientConnection *)arg;
    rtspClientConn->handleRequest();
}

void RtspClientConnection::handleRequest()
{
    char buf[BUFSIZ];
    int bytesRead;
    _requestMsg.clear();
    while ((bytesRead = read(_clientFd, buf, sizeof(buf))) > 0)
    {
        _requestMsg.append(buf, bytesRead);
        memset(buf, 0, sizeof(buf));
    }
    if (_requestMsg.size() == 0)
    {
        LOGI("客户端:fd=[%d]断开连接", _clientFd);
        disConnection();
        return;
    }
    LOGD("收到客户端fd:[%d]消息,message:[%s]", _clientFd, _requestMsg.c_str());

    if (!parseRequest())
    {
        LOGE("请求格式错误!fd:[%d]", _clientFd);
        disConnection();
        return;
    }

    switch (_method)
    {
    case OPTIONS:
        handleOptions();
        break;
    case DESCRIBE:
        handleDescribe();
        break;
    case SETUP:
        if (!_hasSession)
            _session = _server->createNewClientSession(this);
        if(_isOverTcp)
            _session->handleTCPSetup(_url, _suffix, _requestMsg);
        else
            _session->handleUDPSetup(_url, _suffix, _requestMsg);
        break;
    case PLAY:
        if (!_hasSession)
            handleError();
        else
            _session->handlePlay(_url, _suffix, _requestMsg);
        break;
    default:
        break;
    }

}

void RtspClientConnection::sendCallback(void *arg)
{
    RtspClientConnection *rtspClientConn = (RtspClientConnection *)arg;
    rtspClientConn->sendResponse();
}

void RtspClientConnection::sendResponse()
{
    LOGD("返回数据,fd:[%d],msg:[%s]", _clientFd, _responseMsg.c_str());
    if(_method == PLAY||_method == OPTIONS||_method == DESCRIBE||_method == SETUP)
    send(_clientFd, _responseMsg.c_str(), _responseMsg.size(), 0);

    _socketEvent->setEvents(EPOLLIN);
    _server->envir()->scheduler()->updateEvent(_socketEvent);

    if (_method == PLAY)
    {
        for (StreamState *streamState : _session->_streamStates)
        {
            streamState->startPlaying();
        }
    }
}

int RtspClientConnection::parseRequest()
{
    char method[20] = {0};
    char url[512] = {0};
    char version[64] = {0};

    if (sscanf(_requestMsg.c_str(), "%s %s %s", method, url, version) != 3)
        return false;

    if (strcmp(method, "OPTIONS") == 0)
        _method = OPTIONS;
    else if (strcmp(method, "DESCRIBE") == 0)
        _method = DESCRIBE;
    else if (strcmp(method, "SETUP") == 0)
        _method = SETUP;
    else if (strcmp(method, "PLAY") == 0)
        _method = PLAY;
    else if (strcmp(method, "TEARDOWN") == 0)
        _method = TEARDOWN;
    else
    {
        _method = NONE;
        return -1;
    }

    if (strncmp(url, "rtsp://", 7) != 0)
        return -1;

    uint16_t port = 0;
    char ip[64] = {0};
    char suffix[512] = {0};

    if (sscanf(url + 7, "%[^:]:%hu/%s\r\n", ip, &port, suffix) != 3)
    {
        return -1;
    }

    _url = url;
    _suffix = suffix;

    const char *p = strstr(_requestMsg.c_str(), "CSeq");
    if (p)
        sscanf(strstr(_requestMsg.c_str(), "CSeq"), "CSeq: %d\r\n", &_cseq);

    p = strstr(_requestMsg.c_str(), "User-Agent");
    char userAgent[50];
    if (p && sscanf(p, "User-Agent: %s\r\n", userAgent) == 1)
    {
        _userAgent = userAgent;
    }

    p = strstr(_requestMsg.c_str(), "Session");
    char session[20];
    if (p && sscanf(p, "Session: %s\r\n", session) == 1)
    {
        _hasSession = 1;
    }

    p = strstr(_requestMsg.c_str(), "Transport");
    char transport[100];
    if (p && sscanf(p, "Transport: %s\r\n", transport) == 1)
    {
        _transport = transport;
    }

    if (strstr(transport, "RTP/AVP/TCP"))
    {
        _isOverTcp = 1;
        _interleaved = 0;
    }
    else
    {
        p = strstr(transport, "client_port");
        if (p)
        {
            sscanf(p, "client_port=%d-%d\r\n", &_clientRtpUdpPort, &_clientRtcpUdpPort);
        }
    }

    return 1;
}

void RtspClientConnection::handleOptions()
{
    if (!_server->lookForMediaSession(_suffix))
    {
        handleNotFound();
        return;
    }
    char msg[2048];
    snprintf(msg, sizeof(msg),
             "RTSP/1.0 200 OK\r\n"
             "CSeq: %d\r\n"
             "Public: OPTIONS,DESCRIBE, SETUP, PLAY\r\n"
             "\r\n",
             _cseq);
    _responseMsg = msg;
    _socketEvent->setEvents(EPOLLIN | EPOLLOUT);
    _server->envir()->scheduler()->updateEvent(_socketEvent);
}

void RtspClientConnection::handleDescribe()
{
    ServerMediaSession *mediaSession = _server->lookForMediaSession(_suffix);
    if (!mediaSession)
    {
        handleNotFound();
        return;
    }
    std::string sdp = mediaSession->generateSdp();
    char msg[2048];

    snprintf(msg, sizeof(msg),
             "RTSP/1.0 200 OK\r\n"
             "CSeq: %d\r\n"
             "Content-Base: %s\r\n"
             "Content-Type: application/sdp\r\n"
             "Content-Length: %ld\r\n"
             "\r\n"
             "%s",
             _cseq, _url.c_str(), sdp.size(), sdp.c_str());
    _responseMsg = msg;
    _socketEvent->setEvents(EPOLLIN | EPOLLOUT);
    _server->envir()->scheduler()->updateEvent(_socketEvent);
}

void RtspClientConnection::handleNotFound()
{
    char msg[2048];
    snprintf(msg, sizeof(msg),
             "RTSP/1.0 404 Session Not Found\r\n"
             "CSeq: %d\r\n"
             "\r\n",
             _cseq);
    _responseMsg = msg;
    _socketEvent->setEvents(EPOLLIN | EPOLLOUT);
    _server->envir()->scheduler()->updateEvent(_socketEvent);
}

void RtspClientConnection::handleError()
{
    char msg[2048];
    snprintf(msg, sizeof(msg),
             "RTSP/1.0 400 Error\r\n"
             "CSeq: %d\r\n"
             "\r\n",
             _cseq);
    _responseMsg = msg;
}

Event *RtspClientConnection::socketEvent()
{
    return _socketEvent;
}

void RtspClientConnection::disConnection()
{
    closeConnection();
    _disConnCallback(_disconnArg, _clientFd);
}

void RtspClientConnection::closeConnection()
{
    if(_session)
        _session->closeSession();
}
