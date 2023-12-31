#ifndef SERVERMEDIASESSION_H
#define SERVERMEDIASESSION_H

#include <vector>
#include <string>
#include "ServerMediaSubSession.h"

class ServerMediaSession
{
public:
    ServerMediaSession(std::string name);
    ~ServerMediaSession();
    static ServerMediaSession* createNew(std::string name);
    std::string name();
    void addSubSession(ServerMediaSubSession* subsession);
    std::string generateSdp();
    std::vector<ServerMediaSubSession *> getMediaSubSessions();
    ServerMediaSubSession* getMediaSubsessionBySuffix(std::string suffix);


private:
    std::vector<ServerMediaSubSession *> _subSessions;
    std::string _name;
    std::string _sdp;
};

#endif