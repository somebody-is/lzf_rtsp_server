#ifndef LZF_USAGEENVIRONMENT_H
#define LZF_USAGEENVIRONMENT_H

#include"../scheduler/EventScheduler.h"

class UsageEnvironment
{
    public:
    UsageEnvironment(EventScheduler *eventScheduler);
    static UsageEnvironment* createNew(EventScheduler *eventScheduler);
    ~UsageEnvironment();

    EventScheduler* scheduler();

    private:
    EventScheduler *_eventScheduler;

};

#endif