#include<iostream>
#include <stdlib.h>
#include<string>
#include"base/log.h"
#include"scheduler/EventScheduler.h"
#include"net/RtspServer.h"
#include"usageEnvironment/UsageEnvironment.h"
#include"live/ServerMediaSession.h"

// ffplay -i -rtsp_transport tcp  rtsp://192.168.1.130:8081/song1
// ffplay rtsp://192.168.1.130:8081/song1
// ffplay rtsp://192.168.1.130:8081/movie1
// ffplay -i -rtsp_transport tcp  rtsp://192.168.1.130:8081/movie1

int main()
{
    srand(time(NULL));
    EventScheduler *eventScheduler = EventScheduler::createNew();
    UsageEnvironment *env = UsageEnvironment::createNew(eventScheduler);
    RtspServer *rtspServer = new RtspServer(env);

    rtspServer->start();


    env->scheduler()->loop();
    return 0;
}

