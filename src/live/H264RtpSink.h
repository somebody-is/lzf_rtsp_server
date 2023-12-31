#ifndef H264RTPSINK
#define H264RTPSINK

#include"RtpSink.h"

class H264RtpSink: public RtpSink
{
    public:
    H264RtpSink(MediaSource *mediaSource,float fps);
    ~H264RtpSink();

    static H264RtpSink *createNew(MediaSource *mediaSource,float fps);

    struct RtpPacket *getNextPacket();
    int rtpSendH264Frame(RtpPacket *rtpPacket, uint8_t *frame, uint32_t frameSize);
    virtual int read(double &nextSendTime);

};


#endif