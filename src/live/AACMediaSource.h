#ifndef AACMEDIASOURCE_H
#define AACMEDIASOURCE_H

#include "MediaSource.h"
#include <list>

struct AdtsHeader
{
    unsigned int syncword;     // 12 bit 同步字 '1111 1111 1111'，一个ADTS帧的开始
    uint8_t id;                // 1 bit 0代表MPEG-4, 1代表MPEG-2。
    uint8_t layer;             // 2 bit 必须为0
    uint8_t protectionAbsent;  // 1 bit 1代表没有CRC，0代表有CRC
    uint8_t profile;           // 1 bit AAC级别（MPEG-2 AAC中定义了3种profile，MPEG-4 AAC中定义了6种profile）
    uint8_t samplingFreqIndex; // 4 bit 采样率
    uint8_t privateBit;        // 1bit 编码时设置为0，解码时忽略
    uint8_t channelCfg;        // 3 bit 声道数量
    uint8_t originalCopy;      // 1bit 编码时设置为0，解码时忽略
    uint8_t home;              // 1 bit 编码时设置为0，解码时忽略

    uint8_t copyrightIdentificationBit;   // 1 bit 编码时设置为0，解码时忽略
    uint8_t copyrightIdentificationStart; // 1 bit 编码时设置为0，解码时忽略
    unsigned int aacFrameLength;          // 13 bit 一个ADTS帧的长度包括ADTS头和AAC原始流
    unsigned int adtsBufferFullness;      // 11 bit 缓冲区充满度，0x7FF说明是码率可变的码流，不需要此字段。CBR可能需要此字段，不同编码器使用情况不同。这个在使用音频编码的时候需要注意。

    /* number_of_raw_data_blocks_in_frame
     * 表示ADTS帧中有number_of_raw_data_blocks_in_frame + 1个AAC原始帧
     * 所以说number_of_raw_data_blocks_in_frame == 0
     * 表示说ADTS帧中有一个AAC数据块并不是说没有。(一个AAC原始帧包含一段时间内1024个采样及相关数据)
     */
    uint8_t numberOfRawDataBlockInFrame; // 2 bit
};


class AACMediaSource : public MediaSource
{
public:
    AACMediaSource(std::string filename);
    static AACMediaSource *createNew(std::string filename);
    ~AACMediaSource();

    int initAdtsHeader(uint8_t *headData, struct AdtsHeader &header);
    int getNextFrame(uint8_t *frame,int &frameSize);
    void read();

private:
    void getOneAdts();

    int _isEnd;
};

#endif