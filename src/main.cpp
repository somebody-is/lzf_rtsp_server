#include<iostream>
#include <stdlib.h>
#include<string>
#include"base/log.h"
#include"scheduler/EventScheduler.h"
#include"net/RtspServer.h"
#include"usageEnvironment/UsageEnvironment.h"
#include"live/ServerMediaSession.h"
#include"live/H264ServerMediaSubSession.h"
#include"live/AACServerMediaSubSession.h"

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

    ServerMediaSession*mediaSession1 = ServerMediaSession::createNew("song1");
    ServerMediaSubSession* h264SubSession1 = H264ServerMediaSubSession::createNew("/home/adminlzf/Desktop/myproject/LZF_RtspServer/data/banma.h264","h264",25);
    ServerMediaSubSession* aacSubSession1 = AACServerMediaSubSession::createNew("/home/adminlzf/Desktop/myproject/LZF_RtspServer/data/banma.aac","aac",46.875);
    mediaSession1->addSubSession(h264SubSession1);
    mediaSession1->addSubSession(aacSubSession1);

    ServerMediaSession*mediaSession2 = ServerMediaSession::createNew("song2");
    ServerMediaSubSession* h264SubSession2 = H264ServerMediaSubSession::createNew("/home/adminlzf/Desktop/myproject/LZF_RtspServer/data/saddle_of_heart.h264","h264",25);
    ServerMediaSubSession* aacSubSession2 = AACServerMediaSubSession::createNew("/home/adminlzf/Desktop/myproject/LZF_RtspServer/data/saddle_of_heart.aac","aac",46.875);
    mediaSession2->addSubSession(h264SubSession2);
    mediaSession2->addSubSession(aacSubSession2);

    ServerMediaSession*mediaSession3 = ServerMediaSession::createNew("movie1");
    ServerMediaSubSession* h264SubSession3 = H264ServerMediaSubSession::createNew("/home/adminlzf/Desktop/myproject/LZF_RtspServer/data/movie1.h264","h264",24);
    ServerMediaSubSession* aacSubSession3 = AACServerMediaSubSession::createNew("/home/adminlzf/Desktop/myproject/LZF_RtspServer/data/movie1.aac","aac",46.875);
    mediaSession3->addSubSession(h264SubSession3);
    mediaSession3->addSubSession(aacSubSession3);
    
    rtspServer->addMediaSession(mediaSession1);
    rtspServer->addMediaSession(mediaSession3);
    rtspServer->addMediaSession(mediaSession2);
    rtspServer->start();


    env->scheduler()->loop();
    return 0;
}

