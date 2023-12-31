#include "Event.h"
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>

Event::Event()
{
}

Event::Event(int fd, uint32_t events) : _fd(fd), _events(events)
{
}

int Event::getFd()
{
    return _fd;
}

void Event::setEvents(uint32_t events)
{
    _events = events;
}

uint32_t Event::getEvents()
{
    return _events;
}

void Event::setEnd()
{
    _end = 1;
}

int Event::isEnd()
{
    return _end;
}

SocketEvent::SocketEvent(int fd,uint32_t events,EventCallback *readCallback, void *readArg,EventCallback *writeCallback, void *writeArg):Event(fd,events)
{
    _readCallback = readCallback;
    _writeCallback = writeCallback;
    _readArg = readArg;
    _writeArg = writeArg;
}

SocketEvent::~SocketEvent()
{
}

void SocketEvent::handleRead()
{
    (*_readCallback)(_readArg);
}

void SocketEvent::handleWrite()
{
    (*_writeCallback)(_writeArg);
}

SocketEvent *SocketEvent::createNew(int fd,uint32_t events,EventCallback *readCallback, void *readArg,EventCallback *writeCallback, void *writeArg)
{
    return new SocketEvent(fd,events,readCallback,readArg,writeCallback,writeArg);
}


TimerEvent::TimerEvent(EventCallback *timeoutCallback, void *arg, double milliseconds):_timeoutCallback(timeoutCallback),_arg(arg)
{
    _fd = timerfd_create(CLOCK_MONOTONIC, 0);
    _events = EPOLLIN;
    // 设置定时器的超时时间（例如，1秒）
    struct itimerspec timer_spec;
    timer_spec.it_value.tv_sec = 0;
    timer_spec.it_value.tv_nsec = 1000000 * milliseconds;
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = 0;
    timerfd_settime(_fd, 0, &timer_spec, NULL);
}

TimerEvent *TimerEvent::createNew(EventCallback *timeoutCallback, void *arg, double milliseconds)
{
    return new TimerEvent(timeoutCallback,arg,milliseconds);
}

TimerEvent::~TimerEvent()
{
    close(_fd);
}

void TimerEvent::handleRead()
{
    _timeoutCallback(_arg);
}

void TimerEvent::handleWrite()
{
}

void TimerEvent::setTimeout(double milliseconds)
{
    struct itimerspec timer_spec;
    timer_spec.it_value.tv_sec = 0;
    timer_spec.it_value.tv_nsec = 1000000 * milliseconds;
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = 0;
    timerfd_settime(_fd, 0, &timer_spec, NULL);
}
