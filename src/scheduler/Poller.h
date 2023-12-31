#ifndef LZF_POLLER_H
#define LZF_POLLER_H

#include<iostream>
#include <unistd.h>
#include <map>
#include "Event.h"

class Poller
{
public:
    Poller(){};
    virtual ~Poller() {}
    virtual int addEvent(Event *event) = 0;
    virtual int updateEvent(Event *event) = 0;
    virtual int removeEvent(Event *event) = 0;
    virtual void handleEvent() = 0;

protected:
    std::map<int, Event*> _eventMap;
};

#endif