#include "EventScheduler.h"
#include "Epoller.h"

EventScheduler::EventScheduler()
{
    _poller = Epoller::createNew();
}

EventScheduler *EventScheduler::createNew()
{
    return new EventScheduler();
}

EventScheduler::~EventScheduler()
{
    delete _poller;
}

int EventScheduler::addEvent(Event *event)
{
    return _poller->addEvent(event);
}

int EventScheduler::removeEvent(Event *event)
{
    return _poller->removeEvent(event);
}

int EventScheduler::updateEvent(Event *event)
{
    return _poller->updateEvent(event);
}

void EventScheduler::loop()
{
    while(!_quit)
    {
        _poller->handleEvent();
    }
}
