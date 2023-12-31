#ifndef AACRTPSINK
#define AACRTPSINK

#include"RtpSink.h"

class AACRtpSink: public RtpSink
{
    public:
    AACRtpSink(MediaSource *mediaSource,float fps);
    ~AACRtpSink();

    static AACRtpSink *createNew(MediaSource *mediaSource,float fps);

    struct RtpPacket *getNextPacket();
    virtual int read(double &nextSendTime);

};


#endif