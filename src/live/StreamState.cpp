#include "StreamState.h"
#include "../net/SocketOps.h"
#include "ServerMediaSubSession.h"
#include "../net/RtspClientSession.h"
#include "../net/RtspClientConnection.h"
#include "../net/RtspServer.h"
#include "../scheduler/Event.h"
#include <sys/epoll.h>
#include <thread>
#include <chrono>

StreamState::StreamState(RtspClientSession *clientSession, int rtpPort, int rtcpPort, MediaSource *mediaSource, RtpSink *rtpSink)
    : _env(clientSession->_clientConn->_env),_clientSession(clientSession), _mediaSource(mediaSource), _rtpSink(rtpSink)
{
    _isOverTcp = 0;
    _rtpOverUdpPort = rtpPort;
    _rtcpOverUdpPort = rtcpPort;

    _clientRtpPort = clientSession->_clientConn->_clientRtpUdpPort;
    _clientRtcpPort = clientSession->_clientConn->_clientRtcpUdpPort;

    _rtpUdpFd = socketOps::createUdpSocket();
    socketOps::bind(_rtpUdpFd, "127.0.0.1", rtpPort);
    _rtpSocketEvent = SocketEvent::createNew(_rtpUdpFd, EPOLLIN, NULL, NULL, NULL, NULL);
    //_env->scheduler()->addEvent(_rtpSocketEvent);
    _clientRtpAddress = Ipv4Address::createNew(_clientSession->_clientConn->_ipv4Addr->getAddr());
    _clientRtpAddress->setPort(_clientRtpPort);

    _rtcpUdpFd = socketOps::createUdpSocket();
    socketOps::bind(_rtcpUdpFd, "127.0.0.1", rtcpPort);
    _rtcpSocketEvent = SocketEvent::createNew(_rtcpUdpFd, EPOLLIN, NULL, NULL, NULL, NULL);
    //_env->scheduler()->addEvent(_rtcpSocketEvent);
    _clientRtcpAddress = Ipv4Address::createNew(_clientSession->_clientConn->_ipv4Addr->getAddr());
    _clientRtcpAddress->setPort(_clientRtcpPort);

    _rtpSink->setSendPacketCallback(sendCallback, this);
}

StreamState::StreamState(RtspClientSession *clientSession, MediaSource *mediaSource, RtpSink *rtpSink)
    :_env(clientSession->_clientConn->_env), _clientSession(clientSession),_interleaved(clientSession->_interleaved), _mediaSource(mediaSource), _rtpSink(rtpSink)
{
    _isOverTcp = 1;
    _rtpSink->setSendPacketCallback(sendCallback, this);
}

StreamState *StreamState::createNewOverUdp(RtspClientSession *clientSession, int rtpPort, int rtcpPort, MediaSource *mediaSource, RtpSink *rtpSink)
{
    return new StreamState(clientSession, rtpPort, rtcpPort, mediaSource, rtpSink);
}

StreamState *StreamState::createNewOverTcp(RtspClientSession *clientSession, MediaSource *mediaSource, RtpSink *rtpSink)
{
    return new StreamState(clientSession, mediaSource, rtpSink);
}

StreamState::~StreamState()
{
    if(_rtcpUdpFd!=0){
        close(_rtcpUdpFd);
        delete _rtpSocketEvent;
    }if(_rtpUdpFd!=0){
        close(_rtpUdpFd);
        delete _rtcpSocketEvent;
    }

    delete _mediaSource;
    delete _rtpSink;
    //delete _sendTimerEvent;
    _sendTimerEvent->setEnd();
}



void StreamState::sendCallback(void *arg, void *arg2)
{
    StreamState *streamState = (StreamState *)arg;
    RtpPacket *packet = (RtpPacket *)arg2;
    if (streamState->_isOverTcp)
        streamState->sendPacketOverTcp(packet);
    else
        streamState->sendPacketOverUdp(packet);
}


void StreamState::startPlaying()
{
    if (_isOverTcp==1){
        LOGI("开始向TCP客户端fd[%d],ip[%s],port[%d]传输数据", _clientSession->_clientConn->_clientFd, _clientSession->_clientConn->_ipv4Addr->ip().c_str(), _clientSession->_clientConn->_ipv4Addr->port());
    }else{
        LOGI("开始向UDP客户端fd[%d],ip[%s],port[%d]传输数据", _rtpUdpFd, _clientRtpAddress->ip().c_str(), _clientRtpAddress->port());
    }
    _sendTimerEvent = TimerEvent::createNew(continuePlaying, this, 1);
    _clientSession->_clientConn->_server->envir()->scheduler()->addEvent(_sendTimerEvent);

}

void StreamState::continuePlaying(void *arg)
{
    StreamState *streamState = (StreamState *)arg;
    if(streamState&&streamState->_state==-1)
    {
        close(streamState->_sendTimerEvent->getFd());
        return;
    }else if(!streamState)
    return;
    streamState->_rtpSink->read(streamState->_nextSendTime);
    streamState->_sendTimerEvent->setEvents(EPOLLIN);
    streamState->_env->scheduler()->updateEvent(streamState->_sendTimerEvent);
    streamState->_sendTimerEvent->setTimeout(streamState->_nextSendTime);
}

void StreamState::endPlaying()
{
    _clientSession->_clientConn->_server->envir()->scheduler()->removeEvent(_rtpSocketEvent);
    _clientSession->_clientConn->_server->envir()->scheduler()->removeEvent(_rtcpSocketEvent);
    _state = -1;
}

void StreamState::sendPacketOverUdp(RtpPacket *packet)
{
    int ret;

    packet->seq = htons(packet->seq); // 从主机字节顺序转变成网络字节顺序
    packet->timestamp = htonl(packet->timestamp);
    packet->ssrc = htonl(packet->ssrc);

    uint8_t buffer[12 + packet->dataSize];
    memcpy(buffer, packet, 12);                             // 将RTP头部拷贝到buffer中
    memcpy(buffer + 12, packet->payload, packet->dataSize); // 将payload数据拷贝到buffer中
    struct sockaddr_in clientAddr = _clientRtpAddress->getAddr();
    ret = sendto(_rtpUdpFd, buffer, packet->dataSize + 12, 0,
                 (struct sockaddr *)&clientAddr, sizeof(clientAddr));

    packet->seq = ntohs(packet->seq);
    packet->timestamp = ntohl(packet->timestamp);
    packet->ssrc = ntohl(packet->ssrc);

    free(packet->payload);
    free(packet);
}

void StreamState::sendPacketOverTcp(RtpPacket *packet)
{
    int ret;
    packet->seq = htons(packet->seq); // 从主机字节顺序转变成网络字节顺序
    packet->timestamp = htonl(packet->timestamp);
    packet->ssrc = htonl(packet->ssrc);

    uint8_t data[packet->dataSize + 12 + 4];
    data[0] = 0x24;
    data[1] = 2*_interleaved-2;
    data[2] = (uint8_t)(((packet->dataSize + 12) & 0xFF00) >> 8);
    data[3] = (uint8_t)((packet->dataSize + 12) & 0xFF);
    memcpy(data+4, packet, 12);
    memcpy(data+4+12, packet->payload, packet->dataSize);

    ret = send(_clientSession->_clientConn->_clientFd, data, packet->dataSize + 12 + 4, 0);
    if(ret<0)
        LOGE("发送数据失败");

    packet->seq = ntohs(packet->seq);
    packet->timestamp = ntohl(packet->timestamp);
    packet->ssrc = ntohl(packet->ssrc);

    free(packet->payload);
    free(packet);
}
