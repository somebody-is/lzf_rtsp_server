#include "RtpSink.h"

RtpSink::RtpSink(MediaSource *mediaSource,float fps) : _mediaSource(mediaSource),_fps(fps)
{
    
}

RtpSink::~RtpSink()
{
}

int RtpSink::init(RtpPacket *packet,uint8_t csrcLen, uint8_t extension,
                  uint8_t padding, uint8_t version, uint8_t payloadType, uint8_t marker,
                  uint16_t seq, uint32_t timestamp, uint32_t ssrc)
{
    packet->csrcLen = csrcLen;
    packet->extension = extension;
    packet->padding = padding;
    packet->version = version;
    packet->payloadType = payloadType;
    packet->marker = marker;
    packet->seq = seq;
    packet->timestamp = timestamp;
    packet->ssrc = ssrc;
    packet->payload=NULL;
    return 1;
}

RtpPacket *RtpSink::createPacket(RtpPacket *packet)
{
    RtpPacket *newPacket = new struct RtpPacket;
    (*newPacket) = (*packet);
    return newPacket;
}

void RtpSink::setSendPacketCallback(SendPacketCallback *callback,void *streamState)
{
    _sendPacketCallback = callback;
    _streamState = streamState;
}
