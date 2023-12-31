#include "RtspClientSession.h"
#include "RtspClientConnection.h"
#include "RtspServer.h"
#include "SocketOps.h"
#include <thread>

int RtspClientSession::_rtpPort = 55532;

RtspClientSession::RtspClientSession(RtspClientConnection *clientConnection, std::string sessionId) : _clientConn(clientConnection), _sessionId(sessionId)
{
}

RtspClientSession *RtspClientSession::createNew(RtspClientConnection *clientConnection, std::string sessionId)
{
    return new RtspClientSession(clientConnection, sessionId);
}

RtspClientSession::~RtspClientSession()
{
    for(StreamState * stream:_streamStates)
    {
        delete stream;
    }
    _streamStates.clear();
    
}

void RtspClientSession::handleUDPSetup(std::string url, std::string suffix, std::string requestMsg)
{
    char responseMsg[2048];
    int rtpPort = _rtpPort++;
    int rtcpPort = _rtpPort++;
    snprintf(responseMsg, 2048,
             "RTSP/1.0 200 OK\r\n"
             "CSeq: %d\r\n"
             "Session: %s\r\n"
             "Transport: %s;server_port=%d-%d\r\n"
             "\r\n",
             _clientConn->_cseq, _sessionId.c_str(),
             _clientConn->_transport.c_str(), rtpPort, rtcpPort);

    ServerMediaSession *mediaSession = _clientConn->_server->lookForMediaSession(suffix);
    if (!mediaSession)
    {
        _clientConn->disConnection();
        return;
    }
    ServerMediaSubSession *subSession = mediaSession->getMediaSubsessionBySuffix(suffix);
    if (!subSession)
    {
        _clientConn->disConnection();
        return;
    }
    StreamState *streamState;
    if (_clientConn->_isOverTcp)
        streamState = subSession->createNewStreamOverTcp(this);
    else
        streamState = subSession->createNewStreamOverUdp(this, rtpPort, rtcpPort);
    _streamStates.push_back(streamState);
    _clientConn->_responseMsg.clear();
    _clientConn->_responseMsg = responseMsg;
    _clientConn->_socketEvent->setEvents(EPOLLIN | EPOLLOUT);
    _clientConn->_server->envir()->scheduler()->updateEvent(_clientConn->_socketEvent);
}

void RtspClientSession::handleTCPSetup(std::string url, std::string suffix, std::string requestMsg)
{
    char responseMsg[2048];
    snprintf(responseMsg, 2048,
             "RTSP/1.0 200 OK\r\n"
             "CSeq: %d\r\n"
             "Session: %s\r\n"
             "Transport: %s;\r\n"
             "\r\n",
             _clientConn->_cseq, _sessionId.c_str(),
             _clientConn->_transport.c_str());
    _interleaved++;
    ServerMediaSession *mediaSession = _clientConn->_server->lookForMediaSession(suffix);
    if (!mediaSession)
    {
        _clientConn->disConnection();
        return;
    }
    ServerMediaSubSession *subSession = mediaSession->getMediaSubsessionBySuffix(suffix);
    if (!subSession)
    {
        _clientConn->disConnection();
        return;
    }
    StreamState *streamState;
    streamState = subSession->createNewStreamOverTcp(this);
    _streamStates.push_back(streamState);
    _clientConn->_responseMsg.clear();
    _clientConn->_responseMsg = responseMsg;
    _clientConn->_socketEvent->setEvents(EPOLLIN | EPOLLOUT);
    _clientConn->_server->envir()->scheduler()->updateEvent(_clientConn->_socketEvent);
}

void RtspClientSession::handlePlay(std::string url, std::string suffix, std::string requestMsg)
{
    char buffer[2048];
    snprintf((char *)buffer, sizeof(buffer),
             "RTSP/1.0 200 OK\r\n"
             "CSeq: %d\r\n"
             "Range: npt=0.000-\r\n"
             "Session: %s; timeout=10\r\n"
             "\r\n",
             _clientConn->_cseq, _sessionId.c_str());
    _clientConn->_responseMsg = buffer;
    _clientConn->_socketEvent->setEvents(EPOLLIN | EPOLLOUT);
    _clientConn->_server->envir()->scheduler()->updateEvent(_clientConn->_socketEvent);
}

void RtspClientSession::closeSession()
{
    for (StreamState *streamState : _streamStates)
    {
        streamState->endPlaying();
    }
}
