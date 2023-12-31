#ifndef LZF_EVENTSCHEDULER_H
#define LZF_EVENTSCHEDULER_H

#include<vector>
#include"../base/log.h"
#include"Event.h"
#include"Poller.h"

class EventScheduler
{
public:
    EventScheduler();
    static EventScheduler*createNew();
    ~EventScheduler();

    int addEvent(Event *event);
    int removeEvent(Event *event);
    int updateEvent(Event *event);

    void loop();

private:
    Poller *_poller;
    int _quit=0;

};

#endif