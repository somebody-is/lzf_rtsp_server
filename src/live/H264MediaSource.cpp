#include "H264MediaSource.h"

H264MediaSource::H264MediaSource(std::string filename) : MediaSource(filename)
{
}

H264MediaSource *H264MediaSource::createNew(std::string filename)
{
    return new H264MediaSource(filename);
}

H264MediaSource::~H264MediaSource()
{
}

static inline int startCode3(uint8_t *buf)
{
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 1)
        return 1;
    else
        return 0;
}

static inline int startCode4(uint8_t *buf)
{
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1)
        return 1;
    else
        return 0;
}

static uint8_t *findNextStartCode(uint8_t *buf, int len)
{
    int i;

    if (len < 3)
        return NULL;

    for (i = 0; i < len - 3; ++i)
    {
        if (startCode3(buf) || startCode4(buf))
            return buf;

        ++buf;
    }

    if (startCode3(buf))
        return buf;

    return NULL;
}

int H264MediaSource::getNextFrame(uint8_t *frame, int &size)
{
    int rSize, frameSize;
    uint8_t *nextStartCode;
    uint8_t *buffer = (uint8_t *)malloc(500000);
    if (_fp == NULL)
        return -1;

    rSize = fread(buffer, 1, 500000, _fp);

    if (!startCode33(buffer) && !startCode44(buffer))
        return -1;

    nextStartCode = findNextStartCode1(buffer + 3, rSize - 3);
    if (!nextStartCode)
    {
        return -1;
    }
    else
    {
        frameSize = (nextStartCode - buffer);
        fseek(_fp, frameSize - rSize, SEEK_CUR);
    }

    int startCode;
    if (startCode33(buffer))
        startCode = 3;
    else
        startCode = 4;

    frameSize -= startCode;
    memcpy(frame, buffer + startCode, frameSize);
    free(buffer);
    size = frameSize;
    return frameSize;
}

void H264MediaSource::read(uint8_t *frame)
{
}

void H264MediaSource::getOneNalu()
{
}
