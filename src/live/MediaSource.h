#ifndef MEDIASOURCE_H
#define MEDIASOURCE_H

#include <string>
#include "../base/log.h"
static inline int startCode33(uint8_t *buf)
{
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 1)
        return 1;
    else
        return 0;
}

static inline int startCode44(uint8_t *buf)
{
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1)
        return 1;
    else
        return 0;
}

static uint8_t *findNextStartCode1(uint8_t *buf, int len)
{
    int i;

    if (len < 3)
        return NULL;

    for (i = 0; i < len - 3; ++i)
    {
        if (startCode33(buf) || startCode44(buf))
            return buf;

        ++buf;
    }

    if (startCode33(buf))
        return buf;

    return NULL;
}

class MediaSource
{

public:
    MediaSource(std::string filename);
    ~MediaSource();
    virtual int getNextFrame(uint8_t *frame, int &frameSize) = 0;

protected:
    std::string _filename;
    FILE *_fp;
};

#endif