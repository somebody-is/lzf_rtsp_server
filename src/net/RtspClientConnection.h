#ifndef LZF_RTSPCLIENTCONECTION_H
#define LZF_RTSPCLIENTCONECTION_H

#include<map>
#include"Ipv4Address.h"
#include"../base/log.h"
#include"../scheduler/Event.h"
#include"RtspClientSession.h"
#include"../usageEnvironment/UsageEnvironment.h"

typedef void DisConnectCallback(void*,int);

class RtspServer;
class RtspClientConnection
{
    enum Method
    {
        OPTIONS, DESCRIBE, SETUP, PLAY, TEARDOWN,
        NONE,
    };
    public:
    RtspClientConnection(RtspServer *server,int clientFd,Ipv4Address *ipv4Addr);
    ~RtspClientConnection();
    static RtspClientConnection* createNewClientConnection(RtspServer *server,int clientFd,Ipv4Address *ipv4Addr);
    
    

    void setCloseCallback(DisConnectCallback *callback,void *arg);
    static void requestCallback(void *arg);
    void handleRequest();
    static void sendCallback(void *arg);
    void sendResponse();
    int parseRequest();

    void handleOptions();
    void handleDescribe();
    void handleNotFound();
    void handleError();

    Event* socketEvent();
    void disConnection();
    void closeConnection();


protected:
    UsageEnvironment*_env;
    int _state = 0;
    friend class RtspClientSession;
    friend class RtspServer;
    friend class StreamState;
    RtspServer *_server;
    Event *_socketEvent;
    int _clientFd;
    std::string _requestMsg;
    std::string _responseMsg;
    Ipv4Address *_ipv4Addr;
    int _cseq;
    int _hasSession = 0;
    RtspClientSession *_session = NULL;
    private:
    

    Method _method;
    std::string _url;
    std::string _suffix;
    std::string _transport;
    
    std::string _userAgent;

    int _isOverTcp=0;
    int _clientRtpUdpPort;
    int _clientRtcpUdpPort;
    int _interleaved;

    DisConnectCallback *_disConnCallback;
    void* _disconnArg;



};


#endif