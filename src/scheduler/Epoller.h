#ifndef LZF_EPOLLER_H
#define LZF_EPOLLER_H
#define EVENT_MAX 100

#include <sys/epoll.h>
#include "Poller.h"
#include "../base/log.h"
#include "../net/SocketOps.h"

class Epoller :public Poller
{
public:
    Epoller();
    virtual ~Epoller();
    static Epoller *createNew();
    virtual int addEvent(Event *event);
    virtual int updateEvent(Event *event);
    virtual int removeEvent(Event *event);
    virtual void handleEvent();

private:
    int _epollFd;
    std::map<int, struct epoll_event*> _eventMap;
};
#endif