#include "H264ServerMediaSubSession.h"
#include "H264MediaSource.h"
#include"H264RtpSink.h"

H264ServerMediaSubSession::H264ServerMediaSubSession(std::string filename,std::string sessionName,float fps):ServerMediaSubSession(filename,sessionName,fps)
{
}

H264ServerMediaSubSession *H264ServerMediaSubSession::createNew(std::string filename,std::string sessionName,float fps)
{
    return new H264ServerMediaSubSession(filename,sessionName,fps);
}

H264ServerMediaSubSession::~H264ServerMediaSubSession()
{
}

char *H264ServerMediaSubSession::sdpLines()
{
    char *buffer = new char[2048];
    snprintf(buffer, 2048,
             "m=video 0 RTP/AVP 96\r\n"
             //"c=IN IP4 0.0.0.0\r\n"
             "a=rtpmap:96 H264/90000\r\n"
             "a=framerate:30\r\n"
             "a=control:%s\r\n",_sessionName.c_str());
    return buffer;
}


StreamState *H264ServerMediaSubSession::createNewStreamOverUdp(RtspClientSession *clientSession, int rtpPort, int rtcpPort)
{
    MediaSource *mediaSource = H264MediaSource::createNew(_filename);
    RtpSink* rtpSink = H264RtpSink::createNew(mediaSource,_fps);

    StreamState *streamState= StreamState::createNewOverUdp(clientSession,rtpPort,rtcpPort,mediaSource,rtpSink);

    return streamState;
}

StreamState *H264ServerMediaSubSession::createNewStreamOverTcp(RtspClientSession *clientSession)
{
    MediaSource *mediaSource = H264MediaSource::createNew(_filename);
    RtpSink* rtpSink = H264RtpSink::createNew(mediaSource,_fps);

    StreamState *streamState= StreamState::createNewOverTcp(clientSession,mediaSource,rtpSink);

    return streamState;
}
