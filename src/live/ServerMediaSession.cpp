#include "ServerMediaSession.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

ServerMediaSession::ServerMediaSession(std::string name)
{
    _name = name;
}

ServerMediaSession::~ServerMediaSession()
{
}

ServerMediaSession *ServerMediaSession::createNew(std::string name)
{
    return new ServerMediaSession(name);
}

std::string ServerMediaSession::name()
{
    return _name;
}

void ServerMediaSession::addSubSession(ServerMediaSubSession *subsession)
{
    _subSessions.push_back(subsession);
}

std::string ServerMediaSession::generateSdp()
{
    if (!_sdp.empty())
        return _sdp;
    char buffer[2048] = {0};
    snprintf(buffer, sizeof(buffer),
             "v=0\r\n"
             "o=- 9%ld 1 IN IP4 %s\r\n"
             "t=0 0\r\n"
             "a=control:*\r\n",
             (long)time(NULL), "127.0.0.1");

    for (ServerMediaSubSession *subSession : _subSessions)
    {
        char *tmp = subSession->sdpLines();
        snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer),
                 "%s", tmp);
        delete[] tmp;
        //break;
    }
    _sdp.clear();
    _sdp.append(buffer);
    return _sdp;
}

std::vector<ServerMediaSubSession *> ServerMediaSession::getMediaSubSessions()
{
    return std::vector<ServerMediaSubSession *>(_subSessions);
}

ServerMediaSubSession *ServerMediaSession::getMediaSubsessionBySuffix(std::string suffix)
{

    for(ServerMediaSubSession *subSession:_subSessions)
    {
        int a = suffix.find(subSession->name().c_str());
        if(a>=0)
        {
            return subSession;
        }
    }
    return nullptr;
}
