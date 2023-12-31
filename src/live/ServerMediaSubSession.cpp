#include "ServerMediaSubSession.h"

ServerMediaSubSession::ServerMediaSubSession(std::string filename,std::string sessionName,float fps)
{
    _filename = filename;
    _sessionName = sessionName;
    _fps = fps;
}

ServerMediaSubSession::~ServerMediaSubSession()
{
}

std::string ServerMediaSubSession::name()
{
    return _sessionName;
}
