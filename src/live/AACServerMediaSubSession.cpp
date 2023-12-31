#include "AACServerMediaSubSession.h"
#include "StreamState.h"
#include "AACRtpSink.h"

AACServerMediaSubSession::AACServerMediaSubSession(std::string filename,std::string sessionName,float fps) : ServerMediaSubSession(filename,sessionName,fps)
{

}

AACServerMediaSubSession *AACServerMediaSubSession::createNew(std::string filename,std::string sessionName,float fps)
{
    return new AACServerMediaSubSession(filename,sessionName,fps);
}

AACServerMediaSubSession::~AACServerMediaSubSession()
{
}

char *AACServerMediaSubSession::sdpLines()
{
    char *buffer = new char[2048];
    snprintf(buffer, 2048,
             "m=audio 0 RTP/AVP 97\r\n"
             "a=rtpmap:97 mpeg4-generic/44100/2\r\n"
             "a=fmtp:97 profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3;config=1210;\r\n"
             "a=control:%s\r\n",_sessionName.c_str());
    return buffer;
}

StreamState *AACServerMediaSubSession::createNewStreamOverUdp(RtspClientSession *clientSession, int rtpPort, int rtcpPort)
{
    MediaSource *mediaSource = AACMediaSource::createNew(_filename);
    RtpSink *rtpSink = AACRtpSink::createNew(mediaSource,_fps);

    StreamState *streamState = StreamState::createNewOverUdp(clientSession, rtpPort, rtcpPort, mediaSource, rtpSink);

    return streamState;
}

StreamState *AACServerMediaSubSession::createNewStreamOverTcp(RtspClientSession *clientSession)
{
    MediaSource *mediaSource = AACMediaSource::createNew(_filename);
    RtpSink *rtpSink = AACRtpSink::createNew(mediaSource,_fps);

    StreamState *streamState = StreamState::createNewOverTcp(clientSession, mediaSource, rtpSink);

    return streamState;
}
