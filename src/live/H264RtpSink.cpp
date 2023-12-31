#include "H264RtpSink.h"
#include <thread>
#include <chrono>

H264RtpSink::H264RtpSink(MediaSource *mediaSource,float fps) : RtpSink(mediaSource,fps)
{
    init(&_packet, 0, 0, 0, 2, 96, 0, 0, 0, 0x88923423);
}

H264RtpSink::~H264RtpSink()
{
}

H264RtpSink *H264RtpSink::createNew(MediaSource *mediaSource,float fps)
{
    return new H264RtpSink(mediaSource,fps);
}

RtpPacket *H264RtpSink::getNextPacket()
{
    return nullptr;
}

int H264RtpSink::read(double &nextSendTime)
{
    // 开始播放，发送RTP包
    int frameSize, startCode;
    uint8_t *frame = (uint8_t *)malloc(500000);

    frameSize = _mediaSource->getNextFrame(frame, frameSize);
    if (frameSize < 0)
    {
        LOGD("读取结束");
        return -1;
    }
    int ret = rtpSendH264Frame(&_packet, frame, frameSize);
    if(ret==1)
        nextSendTime = 0.1;
    else
        nextSendTime = (1000.0000/_fps);

    free(frame);
    return 1;
}

int H264RtpSink::rtpSendH264Frame(RtpPacket *rtpPacket, uint8_t *frame, uint32_t frameSize)
{

    uint8_t naluType; // nalu第一个字节
    int sendBytes = 0;
    int ret;

    naluType = frame[0];
    if (frameSize <= 1400) // nalu长度小于最大包长：单一NALU单元模式
    {
        //*   0 1 2 3 4 5 6 7 8 9
        //*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //*  |F|NRI|  Type   | a single NAL unit ... |
        //*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

        rtpPacket->payload = (uint8_t *)malloc(frameSize);
        memcpy(rtpPacket->payload, frame, frameSize);

        rtpPacket->dataSize = frameSize;
        // sendPacketOverUdp(rtpPacket);
        
        _sendPacketCallback(_streamState, createPacket(rtpPacket));
        //free(rtpPacket->payload);
        rtpPacket->seq++;
        sendBytes += frameSize;
        if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8) // 如果是SPS、PPS就不需要加时间戳
            return 1;
    }
    else // nalu长度小于最大包场：分片模式
    {
        //*  0                   1                   2
        //*  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
        //* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        //* | FU indicator  |   FU header   |   FU payload   ...  |
        //* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

        //*     FU Indicator
        //*    0 1 2 3 4 5 6 7
        //*   +-+-+-+-+-+-+-+-+
        //*   |F|NRI|  Type   |
        //*   +---------------+

        //*      FU Header
        //*    0 1 2 3 4 5 6 7
        //*   +-+-+-+-+-+-+-+-+
        //*   |S|E|R|  Type   |
        //*   +---------------+

        int pktNum = frameSize / 1400;        // 有几个完整的包
        int remainPktSize = frameSize % 1400; // 剩余不完整包的大小
        int i, pos = 1;
        uint8_t *data = (uint8_t *)malloc(1500);
        // 发送完整的包
        for (i = 0; i < pktNum; i++)
        {

            data[0] = (naluType & 0x60) | 28;
            data[1] = naluType & 0x1F;

            if (i == 0)                                     // 第一包数据
                data[1] |= 0x80;                            // start
            else if (remainPktSize == 0 && i == pktNum - 1) // 最后一包数据
                data[1] |= 0x40;                            // end

            memcpy(data + 2, frame + pos, 1400);

            rtpPacket->payload = (uint8_t *)malloc(1400 + 2);
            memcpy(rtpPacket->payload, data, 1400 + 2);
            rtpPacket->dataSize = 1400 + 2;

            _sendPacketCallback(_streamState, createPacket(rtpPacket));

            rtpPacket->seq++;
            sendBytes += 1400;
            pos += 1400;
        }

        // 发送剩余的数据
        if (remainPktSize > 0)
        {
            data[0] = (naluType & 0x60) | 28;
            data[1] |= 0x40; // end

            memcpy(data + 2, frame + pos, remainPktSize + 2);

            rtpPacket->payload = (uint8_t *)malloc(remainPktSize + 2);
            memcpy(rtpPacket->payload, data, remainPktSize + 2);

            rtpPacket->dataSize = remainPktSize + 2;

            _sendPacketCallback(_streamState, createPacket(rtpPacket));

            rtpPacket->seq++;
            sendBytes += remainPktSize;
        }
        free(data);
    }
    rtpPacket->timestamp += (90000 /_fps);
out:

    return sendBytes;
}
