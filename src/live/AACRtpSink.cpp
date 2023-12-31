#include "AACRtpSink.h"
#include<thread>

AACRtpSink::AACRtpSink(MediaSource *mediaSource,float fps):RtpSink(mediaSource,fps)
{
    init(&_packet,0, 0, 0, 2, 97, 1, 0, 0, 0x32411);
}

AACRtpSink::~AACRtpSink()
{
}

AACRtpSink *AACRtpSink::createNew(MediaSource *mediaSource,float fps)
{
    return new AACRtpSink(mediaSource,fps);
}

RtpPacket *AACRtpSink::getNextPacket()
{
    return nullptr;
}

int AACRtpSink::read(double &nextSendTime)
{

    _packet.payload = (uint8_t *)malloc(50000);
    int ret = _mediaSource->getNextFrame(_packet.payload,_dataSize);
    if(ret<0)
        return -1;
    _packet.dataSize = _dataSize;
    _packet.timestamp += 90000 /_fps;

    nextSendTime = 1000/_fps;


    _sendPacketCallback(_streamState,createPacket(&_packet));
    _packet.seq++;

    return 1;
}
