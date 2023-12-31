#ifndef H264MEDIASOURCE_H
#define H264MEDIASOURCE_H

#include"MediaSource.h"
#include"../base/log.h"
#include<list>

struct Nalu
{
    char *frame;
};

class H264MediaSource: public MediaSource
{
public:
    H264MediaSource(std::string filename);
    static H264MediaSource* createNew(std::string filename);
    ~H264MediaSource();

    int getNextFrame(uint8_t *frame,int &frameSize);
    void read(uint8_t *frame);
private:
    void getOneNalu();

};

#endif