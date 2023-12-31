#ifndef LZF_EVENT_H
#define LZF_EVENT_H

#include <iostream>

typedef void EventCallback(void *);

class Event
{
    public:
    Event();
    Event(int fd, uint32_t events);

    int getFd();
    void setEvents(uint32_t events);
    uint32_t getEvents();
    virtual void handleRead() =  0;
    virtual void handleWrite() = 0;
    virtual void setTimeout(double milliseconds)=0;
    void setEnd();
    int isEnd();

    protected:
    int _fd;
    uint32_t _events;
    int _end = 0;
};


class SocketEvent:public Event
{
public:
    SocketEvent(int fd, uint32_t events, EventCallback *readCallback, void *readArg, EventCallback *writeCallback, void *writeArg);
    ~SocketEvent();
    static SocketEvent *createNew(int fd, uint32_t events, EventCallback *readCallback, void *readArg, EventCallback *writeCallback, void *writeArg);
    virtual void handleRead();
    virtual void handleWrite();
    virtual void setTimeout(double milliseconds){}

private:

    EventCallback *_readCallback;
    EventCallback *_writeCallback;
    void *_readArg;
    void *_writeArg;

};

class TimerEvent:public Event
{
public:
    TimerEvent(EventCallback *timeoutCallback, void *arg,double milliseconds);
    static TimerEvent*createNew(EventCallback *timeoutCallback, void *arg,double milliseconds);
    ~TimerEvent();
    virtual void handleRead();
    virtual void handleWrite();
    virtual void setTimeout(double milliseconds);

private:
    EventCallback *_timeoutCallback;
    void *_arg;
};

#endif