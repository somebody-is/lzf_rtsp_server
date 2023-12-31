#include "Epoller.h"

Epoller::Epoller()
{
    _epollFd = epoll_create(200);
    if (_epollFd < 0)
    {
        LOGE("创建epoll失败");
    }
}

Epoller::~Epoller()
{
}

Epoller *Epoller::createNew()
{
    return new Epoller();
}

int Epoller::addEvent(Event *event)
{
    LOGD("注册监听事件,fd:%d", event->getFd());
    struct epoll_event *ev = new epoll_event;
    ev->events = event->getEvents() | EPOLLET;

    ev->data.ptr = event;
    _eventMap.insert(std::make_pair(event->getFd(), ev));
    int ret = epoll_ctl(_epollFd, EPOLL_CTL_ADD, event->getFd(), ev);
    if (ret < 0)
    {
        LOGE("添加监听事件失败");
        close(event->getFd());
        _eventMap.erase(event->getFd());
    }
    return 0;
}

int Epoller::updateEvent(Event *event)
{
    LOGD("更新监听事件,fd:%d", event->getFd());
    if (_eventMap.count(event->getFd()) == 0)
        return -1;
    struct epoll_event *ev = _eventMap[event->getFd()];
    ev->events = event->getEvents() | EPOLLET;
    ev->data.ptr = event;
    int ret = epoll_ctl(_epollFd, EPOLL_CTL_MOD, event->getFd(), ev);
    if (ret < 0)
    {
        LOGE("更新监听事件失败");
        close(event->getFd());
        _eventMap.erase(event->getFd());
    }

    return 0;
}

int Epoller::removeEvent(Event *event)
{
    LOGD("删除监听事件,fd:%d", event->getFd());
    struct epoll_event *ev = _eventMap[event->getFd()];
    int ret = epoll_ctl(_epollFd, EPOLL_CTL_DEL, event->getFd(), ev);
    //close(event->getFd());
    _eventMap.erase(event->getFd());
    delete ev;
    //delete event;
    return 0;
}

void Epoller::handleEvent()
{
    while (1)
    {
        struct epoll_event events[EVENT_MAX];
        int num = epoll_wait(_epollFd, events, EVENT_MAX, -1);
        if (num < 0)
        {
            LOGE("epoll_wait出错");
            exit(0);
        }
        for (int i = 0; i < num; i++)
        {
            int a = 0;
            SocketEvent *event = (SocketEvent *)events[i].data.ptr;
            if(event->isEnd()){
                delete event;
                continue;
            }

            
            if (events[i].events & EPOLLIN)
            {
                event->handleRead();
            }
            if (events[i].events & EPOLLOUT)
            {
                event->handleWrite();
            }
        }
    }
}
