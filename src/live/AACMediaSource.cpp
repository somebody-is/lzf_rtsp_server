#include "AACMediaSource.h"
#include <string.h>

AACMediaSource::AACMediaSource(std::string filename) : MediaSource(filename)
{
}

AACMediaSource *AACMediaSource::createNew(std::string filename)
{
    return new AACMediaSource(filename);
}

AACMediaSource::~AACMediaSource()
{
}

int AACMediaSource::initAdtsHeader(uint8_t *headData, AdtsHeader &header)
{
    memset(&header, 0, sizeof(header));

    if ((headData[0] == 0xFF) && ((headData[1] & 0xF0) == 0xF0))
    {
        header.id = ((uint8_t)headData[1] & 0x08) >> 3;    // 第二个字节与0x08与运算之后，获得第13位bit对应的值
        header.layer = ((uint8_t)headData[1] & 0x06) >> 1; // 第二个字节与0x06与运算之后，右移1位，获得第14,15位两个bit对应的值
        header.protectionAbsent = (uint8_t)headData[1] & 0x01;
        header.profile = ((uint8_t)headData[2] & 0xc0) >> 6;
        header.samplingFreqIndex = ((uint8_t)headData[2] & 0x3c) >> 2;
        header.privateBit = ((uint8_t)headData[2] & 0x02) >> 1;
        header.channelCfg = ((((uint8_t)headData[2] & 0x01) << 2) | (((unsigned int)headData[3] & 0xc0) >> 6));
        header.originalCopy = ((uint8_t)headData[3] & 0x20) >> 5;
        header.home = ((uint8_t)headData[3] & 0x10) >> 4;
        header.copyrightIdentificationBit = ((uint8_t)headData[3] & 0x08) >> 3;
        header.copyrightIdentificationStart = (uint8_t)headData[3] & 0x04 >> 2;

        header.aacFrameLength = (((((unsigned int)headData[3]) & 0x03) << 11) |
                                 (((unsigned int)headData[4] & 0xFF) << 3) |
                                 ((unsigned int)headData[5] & 0xE0) >> 5);

        header.adtsBufferFullness = (((unsigned int)headData[5] & 0x1f) << 6 |
                                     ((unsigned int)headData[6] & 0xfc) >> 2);
        header.numberOfRawDataBlockInFrame = ((uint8_t)headData[6] & 0x03);
    }
    else
    {
        return -1;
    }
    return 1;
}

int AACMediaSource::getNextFrame(uint8_t *frame,int &frameSize)
{
    struct AdtsHeader adtsHeader;
    uint8_t tmpBuf[7];
    int ret = fread(tmpBuf, 1, 7, _fp);
    if (ret < 0)
    {
        LOGD("文件读取完毕");
        _isEnd = 1;
        return -1;
    }
    ret = initAdtsHeader(tmpBuf, adtsHeader);
    if(ret<0)
    {
        _isEnd = 1;
        return -1;
    }

    frameSize = adtsHeader.aacFrameLength - 7;


    
    frame[0] = 0x00;
    frame[1] = 0x10;
    frame[2] = (frameSize & 0x1FE0) >> 5; // 高8位
    frame[3] = (frameSize & 0x1F) << 3;   // 低5位
    ret = fread(frame+4, 1, adtsHeader.aacFrameLength - 7, _fp);
    if (ret < 0)
    {
        LOGD("文件读取完毕");
        _isEnd = 1;
        return -1;
    }

    frameSize+=4;

    return 1;
}
