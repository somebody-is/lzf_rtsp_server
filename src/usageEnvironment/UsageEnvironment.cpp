#include "UsageEnvironment.h"

UsageEnvironment::UsageEnvironment(EventScheduler *eventScheduler)
{
    _eventScheduler = eventScheduler;
}

UsageEnvironment *UsageEnvironment::createNew(EventScheduler *eventScheduler)
{
    return new UsageEnvironment(eventScheduler);
}

UsageEnvironment::~UsageEnvironment()
{
    delete _eventScheduler;
}

EventScheduler *UsageEnvironment::scheduler()
{
    return _eventScheduler;
}
