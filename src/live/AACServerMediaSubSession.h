#ifndef AACSERVERMEDIASUBSESSION_H
#define AACSERVERMEDIASUBSESSION_H

#include<string>
#include"ServerMediaSubSession.h"
#include"AACMediaSource.h"


class AACServerMediaSubSession:public ServerMediaSubSession
{
public:
    AACServerMediaSubSession(std::string filename,std::string sessionName,float fps);
    static AACServerMediaSubSession*createNew(std::string filename,std::string sessionName,float fps);
    ~AACServerMediaSubSession();

    virtual char *sdpLines();
    virtual StreamState* createNewStreamOverUdp(RtspClientSession *clientSession,int rtpPort,int rtcpPort);
    virtual StreamState* createNewStreamOverTcp(RtspClientSession *clientSession);
};

#endif