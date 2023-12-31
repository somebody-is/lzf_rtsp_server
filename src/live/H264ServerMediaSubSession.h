#ifndef H264SERVERMEDIASUBSESSION_H
#define H264SERVERMEDIASUBSESSION_H

#include<string>
#include"ServerMediaSubSession.h"
#include"StreamState.h"

class H264ServerMediaSubSession:public ServerMediaSubSession
{
public:
    H264ServerMediaSubSession(std::string filename,std::string sessionName,float fps);
    static H264ServerMediaSubSession*createNew(std::string filename,std::string sessionName,float fps);
    ~H264ServerMediaSubSession();

    virtual char *sdpLines();
    virtual StreamState* createNewStreamOverUdp(RtspClientSession *clientSession,int rtpPort,int rtcpPort);
    virtual StreamState* createNewStreamOverTcp(RtspClientSession *clientSession);
};

#endif