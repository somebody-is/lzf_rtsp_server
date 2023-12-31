#ifndef RTSPCLIENTSESSION_H
#define RTSPCLIENTSESSION_H

#include<string>
#include<vector>
#include"Ipv4Address.h"
#include"../live/StreamState.h"


class RtspClientConnection;
class RtspClientSession
{
public:
    RtspClientSession(RtspClientConnection *clientConnection,std::string sessionId);
    static RtspClientSession *createNew(RtspClientConnection *clientConnection,std::string sessionId);
    ~RtspClientSession();

    void handleUDPSetup(std::string url,std::string suffix,std::string requestMsg);
    void handleTCPSetup(std::string url,std::string suffix,std::string requestMsg);
    void handlePlay(std::string url,std::string suffix,std::string requestMsg);

    void closeSession();

protected:
    friend class RtspClientConnection;
    friend class RtspServer;
    friend class StreamState;

    RtspClientConnection *_clientConn;
    std::string _sessionId;

    int _interleaved = 0;
    static int _rtpPort;
    int _rtcpPort;
    
    std::vector<StreamState *>_streamStates;

};

#endif