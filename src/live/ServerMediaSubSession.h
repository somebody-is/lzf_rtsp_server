#ifndef ServerMediaSubSession_H
#define ServerMediaSubSession_H

#include<string>
#include"MediaSource.h"
#include"RtpSink.h"

class RtspClientSession;
class StreamState;
class ServerMediaSubSession
{
public:
    ServerMediaSubSession(std::string filename,std::string sessionName,float fps);
    ~ServerMediaSubSession();

    virtual char *sdpLines() = 0;
    virtual std::string name();
    virtual StreamState* createNewStreamOverUdp(RtspClientSession *clientSession,int rtpPort,int rtcpPort) = 0;
    virtual StreamState* createNewStreamOverTcp(RtspClientSession *clientSession) = 0;
protected:
    std::string _filename;
    std::string _sessionName;
    float _fps;

    
};

#endif