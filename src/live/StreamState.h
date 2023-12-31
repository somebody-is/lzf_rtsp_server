#ifndef STREAMSTATE_H
#define STREAMSTATE_H

#include "../net/Ipv4Address.h"
#include "MediaSource.h"
#include "RtpSink.h"
#include "../scheduler/Event.h"
#include "../base/log.h"
#include"../usageEnvironment/UsageEnvironment.h"
#include <list>

class ServerMediaSubSession;
class RtspClientSession;

class StreamState
{
public:
    StreamState(RtspClientSession *clientSession, int rtpPort, int rtcpPort, MediaSource *mediaSource, RtpSink *rtpSink);
    StreamState(RtspClientSession *clientSession, MediaSource *mediaSource, RtpSink *rtpSink);
    static StreamState *createNewOverUdp(RtspClientSession *clientSession, int rtpPort, int rtcpPort, MediaSource *mediaSource, RtpSink *rtpSink);
    static StreamState *createNewOverTcp(RtspClientSession *clientSession, MediaSource *mediaSource, RtpSink *rtpSink);
    ~StreamState();

    static void sendCallback(void *arg, void *packet);

    void startPlaying();
    static void continuePlaying(void *arg);
    void pause();
    void endPlaying();

private:
    void sendPacketOverUdp(RtpPacket *packet);
    void sendPacketOverTcp(RtpPacket *packet);

private:
    UsageEnvironment *_env;
    int _state=0;
    ServerMediaSubSession *_subSession;
    RtspClientSession *_clientSession;

    int _rtpOverUdpPort;
    int _rtcpOverUdpPort;
    int _rtpUdpFd=0;
    int _rtcpUdpFd=0;
    int _clientRtpPort;
    int _clientRtcpPort;
    Ipv4Address *_clientRtpAddress;
    Ipv4Address *_clientRtcpAddress;
    int _interleaved;

    struct sockaddr_in _clientRtpAddr;
    struct sockaddr_in _clientRtcpAddr;

    Event *_rtpSocketEvent;
    Event *_rtcpSocketEvent;
    Event *_sendTimerEvent;

    double _nextSendTime = 0;

    MediaSource *_mediaSource;
    RtpSink *_rtpSink;
    int _isOverTcp = 0;
    std::list<RtpPacket *> _packetQueue;
};

#endif