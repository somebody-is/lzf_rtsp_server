#ifndef RTPSINK_H
#define RTPSINK_H

#include "MediaSource.h"
#include <list>

#define RTP_MAX_PKT_SIZE 1400
#define RTP_HEADER_SIZE 12

struct RtpPacket
{
    // byte 0
    uint8_t csrcLen : 4; // contributing source identifiers count
    uint8_t extension : 1;
    uint8_t padding : 1;
    uint8_t version : 2;

    // byte 1
    uint8_t payloadType : 7;
    uint8_t marker : 1;

    // bytes 2,3
    uint16_t seq;

    // bytes 4-7
    uint32_t timestamp;

    // bytes 8-11
    uint32_t ssrc;

    int dataSize;

    uint8_t *payload;
};

typedef void SendPacketCallback(void *, void *);
class RtpSink
{
public:
    RtpSink(MediaSource *mediaSource, float fps);
    ~RtpSink();

    static int init(RtpPacket *packet, uint8_t csrcLen, uint8_t extension,
                    uint8_t padding, uint8_t version, uint8_t payloadType, uint8_t marker,
                    uint16_t seq, uint32_t timestamp, uint32_t ssrc);
    virtual struct RtpPacket *getNextPacket() = 0;
    struct RtpPacket *createPacket(RtpPacket *packet);
    void setSendPacketCallback(SendPacketCallback *callback, void *streamState);

    virtual int read(double &nextSendTime) = 0;

protected:
    float _fps;
    MediaSource *_mediaSource;
    int _dataSize;
    std::list<RtpPacket *> _packetQueue;
    struct RtpPacket _packet;
    SendPacketCallback *_sendPacketCallback;
    void *_streamState;
};

#endif